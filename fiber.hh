#ifndef __FIBER_HH__
#define __FIBER_HH__

#include "coroutine.hh"

namespace fibers
{
  class fiber : public coroutines::coroutine
  {
    public:
    typedef std::tr1::shared_ptr< fiber > ptr;

    enum current_state
    {
      READY,
      FINISHED,
      BLOCKED,
      RUNNING
    };

    public:
      fiber();

      virtual ~fiber();

      virtual void go() = 0;

      virtual void run( ::ucontext_t* return_to );

      current_state get_state();
    private:
      current_state state;
  };
}

#endif
