#include <ucontext.h>
#include <stdlib.h>
#include "coroutine.hh"
#include <valgrind/valgrind.h>

using namespace coroutines;
using std::tr1::shared_ptr;

const size_t coroutine::StackSize = 16384;

void runner( coroutine* c )
{
	c->start();
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
	VALGRIND_STACK_REGISTER(own_context.uc_stack.ss_sp, (void*)((long)(own_context.uc_stack.ss_sp)+StackSize)); 
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

::ucontext_t* coroutine::get_context()
{
	return &own_context;
}
