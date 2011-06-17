#ifndef __WORKER_HH__
#define __WORKER_HH__

#include <tr1/memory>

#include "coroutine.hh"
#include "scheduler.hh"

namespace workers
{
  class worker
  {
    public:
      typedef std::shared_ptr< worker > ptr;

      static void run();
      
    private:
      schedulers::scheduler::ptr sched;
  };
}

#endif
