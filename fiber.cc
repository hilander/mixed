#include <tr1/memory>
using namespace std::tr1;

#include <vector>
using namespace std;

#include "fiber.hh"
using namespace fibers;

#include "message.hh"
using namespace message_queues;

#include "worker.hh"
using namespace workers;

fiber::fiber()
: state( READY )
{
}

fiber::~fiber()
{
}

void fiber::start()
{
	go();
  state = FINISHED;
	yield();
}

ssize_t fiber::do_read( int f )
{
	set_last_read( 0 );
	owner->block_on_io( f, shared_ptr< fiber >( this ), BLOCKED_FOR_READ );
	yield();
	return last_read;
}

ssize_t fiber::do_write( int f )
{
	set_last_write( 0 );
	owner->block_on_io( f, shared_ptr< fiber >( this ), BLOCKED_FOR_WRITE );
	yield();
	return last_write;
}

void fiber::send_message( fiber_message::ptr m )
{
	owner->send_message( m );
}

fiber_message::ptr fiber::receive_message()
{
	owner->block_on_message( shared_ptr< fiber >( this ) );
	yield();
	fiber_message::ptr p = message_buffer.front();
	message_buffer.pop_front();
	return p;
}

fiber_message::ptr fiber::receive_message_nonblock()
{
	fiber_message::ptr p = message_buffer.front();
	message_buffer.pop_front();
	return p;
}

fiber::current_state fiber::get_state()
{
  return state;
}

void fiber::set_state( fiber::current_state s )
{
  state = s;
}

shared_ptr< vector< char > > fiber::get_buffer()
{
	return rw_buffer;
}

void fiber::set_last_read( ssize_t s )
{
	last_read = s;
}

void fiber::set_last_write( ssize_t s )
{
	last_read = s;
}

void fiber::set_owner( std::tr1::shared_ptr< workers::worker > o )
{
	owner = o;
}

void fiber::put_into_message_buffer( shared_ptr< fiber_message > m )
{
	message_buffer.push_back( m );
}
