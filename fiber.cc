#include <tr1/memory>
using namespace std::tr1;

#include <vector>
using namespace std;

#include "fiber.hh"

using namespace fibers;

fiber::fiber()
: state( READY )
{
}

fiber::~fiber()
{
}

void fiber::run( ::ucontext_t* return_to )
{
  coroutines::coroutine::run( return_to );
  state = FINISHED;
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
