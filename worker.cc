#include <tr1/memory>
using namespace std::tr1;

#include <vector>
#include <map>
using namespace std;

#include "message.hh"
#include "message_queue.hh"
using namespace message_queues;

#include "fiber.hh"
using namespace fibers;

#include "scheduler.hh"
using namespace schedulers;

#include "epoller.hh"
using namespace epollers;

#include "worker.hh"
using namespace workers;

worker::worker()
: master_allowed( false )
{
}

worker::~worker()
{
}

worker::ptr worker::create()
{
	worker::ptr p( new worker() );
	p->init();
	return p;
}
void worker::init()
{
  sched = scheduler::create( shared_ptr< worker >( this ) );
  io_facility = epoller::create();
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
  return master_allowed && ( sched->workload() == 0 );
}

void worker::iteration()
{
  sched->run();
  do_epolls();
  process_incoming_messages();
}

int worker::workload()
{
	return sched->workload();
}

////////////////////////////////////////////////////////////////////////////////
// I/O
////////////////////////////////////////////////////////////////////////////////

void worker::do_epolls()
{
  int how_many = 0;
  vector< shared_ptr< ::epoll_event > > affected_fds = io_facility->do_epolls( how_many );
  if ( how_many > 0 )
  {
		vector< shared_ptr< ::epoll_event > >::iterator ev = affected_fds.begin();
		for (
				; ev != affected_fds.end()
				; ev++ )
		{
			shared_ptr< ::epoll_event > wev( *ev );
			map< int, fiber::ptr >::iterator fib = blocked_fds.find( wev->data.fd );
			fiber::ptr f = fib->second;
			shared_ptr< vector< char > > buf;
			switch ( f->get_state() )
			{
				case fiber::BLOCKED_FOR_READ:
					if ( wev->events & EPOLLIN )
					{
						buf = f->get_buffer();
						f->set_last_read( ::read( fib->first, buf.get(), buf->size() ) );
						f->set_state( fiber::READY );
						blocked_fds.erase( fib );
					}
					break;

				case fiber::BLOCKED_FOR_WRITE:
					if ( wev->events & EPOLLOUT )
					{
						buf = f->get_buffer();
						f->set_last_write( ::write( fib->first, buf.get(), buf->size() ) );
						f->set_state( fiber::READY );
						blocked_fds.erase( fib );
						io_facility->del( wev->data.fd );
					}
					break;

				default:
					break;
			}
		}
  }
}

void worker::block_on_io( int f, fiber::ptr fp, fiber::current_state s )
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

void worker::process_incoming_messages()
{
	shared_ptr< message > mess;
	while ( pipe.read_for_slave( mess ) )
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

void worker::process_service_message( message::ptr m )
{
	service_message::ptr sm = dynamic_pointer_cast< service_message >( m );
	switch ( sm->service )
	{
		case service_message::FINISH_WORK:
			master_allowed = true;
			break;

		case service_message::SPAWN:
			{
				serv_message< service_message::SPAWN >::ptr ssm 
					= dynamic_pointer_cast< serv_message< service_message::SPAWN > >( m ); // I love c++ ;>

				sched->insert( ssm->fiber_to_spawn );
				serv_message< service_message::SPAWN_REPLY >::ptr reply( new serv_message< service_message::SPAWN_REPLY >() );
			}
			break;

		case service_message::BROADCAST_MESSAGE:
			{
				serv_message< service_message::BROADCAST_MESSAGE >::ptr ssm 
					= dynamic_pointer_cast< serv_message< service_message::BROADCAST_MESSAGE > >( m ); // I love c++ (again);>

				pass_message_to_fiber( ssm->fiber_data );
			}
			break;

		default:
			break;
	}
}

// from outside to fiber
void worker::pass_message_to_fiber( shared_ptr< message > m )
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

void worker::send_message( std::tr1::shared_ptr< message_queues::fiber_message > m )
{
	if ( sched->has_fiber( m->receiver ) )
	{
		message::ptr mm( dynamic_pointer_cast< message >( m ) );
		pass_message_to_fiber( mm );
	}
	else
	{
		serv_message< service_message::BROADCAST_MESSAGE >::ptr mm( new serv_message< service_message::BROADCAST_MESSAGE >() );
		mm->fiber_data = m;
		message::ptr mp = dynamic_pointer_cast< message >( mm );
		pipe.write_to_master( mp );
	}
}

void worker::spawn( fiber::ptr f )
{
	serv_message< service_message::SPAWN >::ptr m( new serv_message< service_message::SPAWN >() );
	message::ptr sm = dynamic_pointer_cast< message >( m );
	m->fiber_to_spawn = f;
	pipe.write_to_master( sm );
}

void worker::read_for_master( std::tr1::shared_ptr< message_queues::message >& m )
{
	pipe.read_for_master( m );
}

void worker::write_to_slave( std::tr1::shared_ptr< message_queues::message >& m )
{
	pipe.write_to_slave( m );
}

////////////////////////////////////////////////////////////////////////////////
// End of Messaging
////////////////////////////////////////////////////////////////////////////////

