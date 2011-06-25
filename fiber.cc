#include <tr1/memory>
using namespace std::tr1;

#include <vector>
using namespace std;

#include "fiber.hh"
using namespace fibers;

#include "message.hh"
using namespace message_queues;

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
