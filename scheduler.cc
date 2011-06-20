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

scheduler::ptr scheduler::create()
{
  scheduler::ptr p( new scheduler() );
  p->init();
  return p;
}

void scheduler::init()
{
}

int scheduler::workload()
{
  return runners.size();
}

void scheduler::run()
{
  fiber_runner run_part( own_context );
  for_each( runners.begin(), runners.end(), run_part );
}
