#include <tr1/memory>
using namespace std::tr1;

#include <algorithm>
using namespace std;

#include "scheduler.hh"
using namespace schedulers;

#include "fiber.hh"
using namespace fibers;

class fiber_runner
{
  public:
    fiber_runner( shared_ptr< ::ucontext_t > c )
    : worker_ctx( c )
    {
    }

    void operator() ( fiber::ptr fp )
    {
      if ( fp->get_state() == fiber::READY )
      {
        fp->run( worker_ctx.get() );
      }
    }

  private:
    shared_ptr< ::ucontext_t > worker_ctx;
};

scheduler::scheduler()
{
}

scheduler::ptr scheduler::create()
{
  scheduler::ptr p( new scheduler() );
  p->init();
  return p;
}

scheduler::ptr scheduler::create( std::tr1::shared_ptr< workers::worker > o )
{
  scheduler::ptr p( new scheduler() );
  p->init();
	p->set_owner( o );
  return p;
}

void scheduler::init()
{
	const int own_stack_size = 16384;
	own_context.reset( new ::ucontext_t() );
	own_stack.reset( new vector< char >( own_stack_size ) );
	own_context->uc_stack.ss_sp = own_stack.get();
	own_context->uc_stack.ss_size = own_stack->size();
	::getcontext( own_context.get() );
}

int scheduler::workload()
{
  return runners.size();
}

void scheduler::run()
{
  fiber_runner run_part( own_context );
  for_each( runners.begin(), runners.end(), run_part );
	remove_finished();
}

void scheduler::insert( fibers::fiber::ptr f )
{
	f->set_owner( owner );
	runners.push_back( f );
}

bool fiber_finished_work( fiber::ptr f )
{
	return f->get_state() == fiber::FINISHED;
}

#include <iostream>
using namespace std;
void scheduler::remove_finished()
{
	runners.remove_if( &fiber_finished_work );
}

void scheduler::set_owner( std::tr1::shared_ptr< workers::worker > o )
{
	owner = o;
}

bool scheduler::has_fiber( fibers::fiber::ptr f )
{
	return find( runners.begin(), runners.end(), f ) != runners.end();
}

