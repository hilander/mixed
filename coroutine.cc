#include <ucontext.h>
#include <stdlib.h>
#include "coroutine.hh"

using namespace coroutines;
using std::tr1::shared_ptr;

const size_t coroutine::StackSize = 16384;

void runner( coroutine* c )
{
  c->go();
}

coroutine::coroutine()
{
}

coroutine::~coroutine()
{
  ::free( own_context.uc_stack.ss_sp ); // no valgrind warnings
}

class bad_getcontext
{
};

void coroutine::init()
{
  if ( ::getcontext( &own_context ) == -1 )
  {
    throw bad_getcontext();
  }
  own_context.uc_stack.ss_sp = ::malloc( StackSize ); // no valgrind warnings
  own_context.uc_stack.ss_size = StackSize;
  own_context.uc_link = 0;
  ::makecontext( &own_context, ( void(*)() )( &runner ), 1, this );
  
}

void coroutine::run( ::ucontext_t* return_to )
{
  own_context.uc_link = return_to;
  return_context = return_to;
  ::swapcontext( return_context, &own_context );
}

void coroutine::yield()
{
  ::swapcontext( &own_context, return_context );
}
