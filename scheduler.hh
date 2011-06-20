#ifndef __SCHEDULER__HH__
#define __SCHEDULER__HH__

#include <tr1/memory>

#include <list>

#include "fiber.hh"

namespace schedulers
{
  class scheduler
  {
    public:
      typedef std::tr1::shared_ptr< scheduler > ptr;

      static ptr create();

      void run();

      int workload();

    private:
      scheduler();

      void init();

      std::list< std::tr1::shared_ptr< fibers::fiber > > runners;
      std::tr1::shared_ptr< ::ucontext_t > own_context;
  };
}

#endif
