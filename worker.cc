#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#include <tr1/memory>
using namespace std::tr1;

#include <vector>
#include <map>
using namespace std;

#include "message.hh"
using namespace message_queues;

#include "message_queue.hh"
using namespace message_queues;

#include "fiber.hh"
using namespace fibers;

#include "scheduler.hh"
using namespace schedulers;

#include "epoller.hh"
using namespace epollers;

#include "master.hh"

#include "worker.hh"
using namespace workers;

  worker::worker( bool eio )
: master_allowed( false )
, unspawned_fibers( 0 )
, enable_io( eio )
{
}

worker::~worker()
{
}

worker* worker::create( bool eio )
{
  worker* p = new worker( eio );
  p->init();
  return p;
}
void worker::init()
{
  sched = scheduler::create( shared_ptr< worker >( this ) );
  if ( enable_io )
  {
    io_facility = epoller::create();
  }
  pipe.reset( new message_queue() );
}

void worker::run()
{
  while ( !finished() )
  {
    iteration();
  }
}

bool worker::finished()
{
  return master_allowed && ( workload() <= 0 );
}

void worker::iteration()
{
  process_incoming_message_queues();
  sched->run();
  if ( enable_io )
  {
    do_epolls();
  }
}

int32_t worker::workload()
{
  return sched->workload() + unspawned_fibers;
}

void worker::set_master( masters::master::ptr m )
{
  my_master = m;
}
////////////////////////////////////////////////////////////////////////////////
// I/O
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
#include <errno.h>
using namespace std;
void worker::do_epolls()
{
  int32_t how_many =  io_facility->do_epolls();
  if ( how_many > 0 )
  {
    ::epoll_event* ev = io_facility->get_last_epoll_result();
    for ( int32_t i = 0; i < how_many; i++)
    {
      ::epoll_event wev = ev[ i ];
      int32_t ffd = wev.data.fd;
      map< int32_t, fiber::ptr >::iterator fib = blocked_fds.find( ffd );
      fiber::ptr f = fib->second;
      shared_ptr< vector< char > > buf;
      switch ( f->get_state() )
      {
        case fiber::BLOCKED_FOR_READ:
          if ( wev.events & EPOLLIN )
          {
            ssize_t ss = f->get_rw_size();
            char* buf = new char[ ss ];
            ssize_t read_res = ::read( fib->first, buf, ss );
            f->set_last_read( read_res );
            if ( read_res > 0 )
            {
              f->put_into_rw_buffer( buf, read_res );
            }
            f->set_state( fiber::READY );
            blocked_fds.erase( fib );
            delete[] buf;
          }
          break;

        case fiber::BLOCKED_FOR_WRITE:
          if ( wev.events & EPOLLOUT )
          {
            ssize_t ss = fib->second->get_rw_size();
            char* buf = new char[ ss ];
            f->get_from_rw_buffer( buf );
            ssize_t write_res = ::write( fib->first, buf, ss );
            f->set_last_write( write_res );
            f->set_state( fiber::READY );
            blocked_fds.erase( fib );
            io_facility->del( wev.data.fd );
            delete[] buf;
          }
          break;

        case fiber::BLOCKED_FOR_ACCEPT:
          if ( wev.events & EPOLLIN )
          {
            //::sockaddr_in sin;
            //::socklen_t sin_size = sizeof( ::sockaddr_in );
            int32_t afd = ::accept( fib->first, 0, 0 ); //(::sockaddr*)&sin, &sin_size ); // C-call -- C-cast ;P
            if ( afd >= 0 )
            {
              f->set_last_accepted_fd( afd );
              f->set_state( fiber::READY );
              blocked_fds.erase( fib );
              io_facility->del( wev.data.fd );
            }
          }
          break;

        case fiber::BLOCKED_FOR_CONNECT:
          //cout << "worker: connect " << wev.events << ": " << ( wev.events & ( EPOLLIN | EPOLLOUT ) ) << endl;
          if ( wev.events & ( /*EPOLLIN |*/ EPOLLOUT ) )
          {
            if ( wev.events & EPOLLERR )
            {
              //cout << "CONNECT: error: " << errno << endl;
              f->set_connect_status( errno );
            }
            else
            {
              f->set_connect_status( 0 );
            }

            f->set_state( fiber::READY );
            blocked_fds.erase( fib );
            io_facility->del( wev.data.fd );
          }
          break;

        default:
          break;
      }
    }
  }
}

void worker::insert_fd( int32_t f )
{
  io_facility->add( f );
}

void worker::remove_fd( int32_t f )
{
  io_facility->del( f );
}

void worker::do_connect( int32_t f, fiber::ptr fp, fiber::current_state s )
{
  fp->set_state( s );
  blocked_fds[ f ] = fp;
}

void worker::block_on_io( int32_t f, fiber::ptr fp, fiber::current_state s )
{
  fp->set_state( s );
  blocked_fds[ f ] = fp;
  io_facility->add( f );
}

////////////////////////////////////////////////////////////////////////////////
// End of I/O
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// Messaging
////////////////////////////////////////////////////////////////////////////////

void worker::block_on_message( fiber::ptr fp )
{
  fp->set_state( fiber::BLOCKED_FOR_MESSAGE );
}

void worker::process_incoming_message_queues()
{
  shared_ptr< message > mess;
  while ( pipe->read_for_slave( mess ) )
  {
    switch ( mess->m_type )
    {
      case message_type::ServiceMessage:
        process_service_message( mess );
        break;

      case message_type::FiberMessage:
        pass_message_to_fiber( mess );
        break;

      default:
        break;
    }
  }
}

void worker::process_service_message( message::ptr& m )
{
  service_message::ptr sm = dynamic_pointer_cast< service_message >( m );
  switch ( sm->service )
  {
    case service_message::FINISH_WORK:
      master_allowed = true;
      break;

    case service_message::SPAWN:
      {
        sched->insert( sm->fiber_to_spawn );
      }
      break;

    case service_message::BROADCAST_MESSAGE:
      {
        pass_message_to_fiber( sm->fiber_data );
      }
      break;

    default:
      break;
  }
}

// from outside to fiber
void worker::pass_message_to_fiber( shared_ptr< message >& m )
{
  fiber_message::ptr fm( dynamic_pointer_cast< fiber_message >( m ) );
  fiber::ptr f = fm->receiver;
  if ( sched->has_fiber( f ) )
  {
    f->put_into_message_buffer( fm );
    if ( f->get_state() == fiber::BLOCKED_FOR_MESSAGE )
    {
      f->set_state( fiber::READY );
    }
  }
}

void worker::send_message( fiber_message::ptr m )
{
  if ( sched->has_fiber( m->receiver ) )
  {
    message::ptr mm( dynamic_pointer_cast< message >( m ) );
    pass_message_to_fiber( mm );
  }
  else
  {
    service_message::ptr mm( new service_message( service_message::BROADCAST_MESSAGE ) );
    mm->fiber_data = m;
    message::ptr mp = dynamic_pointer_cast< message >( mm );
    pipe->write_to_master( mp );
  }
}

void worker::spawn( fiber::ptr& f )
{
  fiber::ptr nf;
  nf.swap( f );
  service_message::ptr m;
  m.reset( new service_message( service_message::SPAWN ) );
  m->fiber_to_spawn = nf;
  message::ptr sm = static_pointer_cast< message >( m );
  pipe->write_to_master( sm );
  // pipe->write_to_slave( sm );
}

bool worker::read_for_master( message::ptr& m )
{
  return pipe->read_for_master( m );
}

void worker::write_to_slave( message::ptr& m )
{
  pipe->write_to_slave( m );
}

////////////////////////////////////////////////////////////////////////////////
// End of Messaging
////////////////////////////////////////////////////////////////////////////////

