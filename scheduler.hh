#ifndef __SCHEDULER__HH__
#define __SCHEDULER__HH__

#include <tr1/memory>

#include <list>

namespace schedulers
{
  class scheduler
  {
    public:
      typedef std::tr1::shared_ptr< scheduler > ptr;

      void run();
  };
}

#endif
