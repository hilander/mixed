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
