#include <sys/socket.h>
#include <netinet/in.h>

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

worker::worker()
: master_allowed( false )
{
}

worker::~worker()
{
}

worker* worker::create()
{
	worker* p = new worker();
	p->init();
	return p;
}
void worker::init()
{
  sched = scheduler::create( shared_ptr< worker >( this ) );
  io_facility = epoller::create();
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
  return master_allowed && ( sched->workload() == 0 );
}

void worker::iteration()
{
  process_incoming_message_queues();
  sched->run();
  do_epolls();
}

int worker::workload()
{
	return sched->workload();
}

void worker::set_master( masters::master* m )
{
	my_master = m;
}
////////////////////////////////////////////////////////////////////////////////
// I/O
////////////////////////////////////////////////////////////////////////////////

#include <iostream>
using namespace std;
void worker::do_epolls()
{
  int how_many =  io_facility->do_epolls();
  if ( how_many > 0 )
  {
		::epoll_event* ev = io_facility->get_last_epoll_result();
		for ( int i = 0; i < how_many; i++)
		{
			::epoll_event wev = ev[ i ];
			int ffd = wev.data.fd;
			map< int, fiber::ptr >::iterator fib = blocked_fds.find( ffd );
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
						ssize_t write_res = ::write( fib->first, buf, ss );
						f->set_last_write( write_res );
						if ( write_res > 0 )
						{
							f->put_into_rw_buffer( buf, write_res );
						}
						f->set_state( fiber::READY );
						blocked_fds.erase( fib );
						io_facility->del( wev.data.fd );
						delete[] buf;
					}
					break;

				case fiber::BLOCKED_FOR_ACCEPT:
					if ( wev.events & EPOLLIN )
					{
						::sockaddr_in sin;
						::socklen_t sin_size = sizeof( ::sockaddr_in );
						int afd = ::accept( fib->first, (::sockaddr*)&sin, &sin_size ); // C-call -- C-cast ;P
						if ( afd > 0 )
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
					if ( wev.events & ( EPOLLIN | EPOLLOUT ) )
					{
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
				sm.reset( new service_message( service_message::SPAWN_REPLY ) );
				message::ptr reply = dynamic_pointer_cast< message >( sm );
				pipe->write_to_master( reply );
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
	service_message::ptr m( new service_message( service_message::SPAWN ) );
	message::ptr sm = dynamic_pointer_cast< message >( m );
	m->fiber_to_spawn = f;
	pipe->write_to_master( sm );
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

