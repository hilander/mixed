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

void worker::init()
{
  sched = scheduler::create( shared_from_this() );
  io_facility = epoller::create();
}

void worker::run()
{
  while ( !finished() )
  {
    iteration();
  }
}

void worker::iteration()
{
  sched->run();
  do_epolls();
  process_incoming_messages();
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

void worker::block_on_message( int m_id, fiber::ptr fp )
{
	fp->set_state( fiber::BLOCKED_FOR_MESSAGE );
	blocked_msgs[ m_id ] = fp;
}

void worker::process_incoming_messages()
{
	message::ptr mess;
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
}

void worker::pass_message_to_fiber( message::ptr m )
{
	fiber_message::ptr sm = dynamic_pointer_cast< fiber_message >( m );
}

////////////////////////////////////////////////////////////////////////////////
// End of Messaging
////////////////////////////////////////////////////////////////////////////////

bool worker::finished()
{
  return true;
}

