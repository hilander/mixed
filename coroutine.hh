#ifndef __COROUTINE_HH__
#define __COROUTINE_HH__

#include <tr1/memory>
#include <ucontext.h>

namespace coroutines
{
  class coroutine
  {
    public:
      typedef std::tr1::shared_ptr< coroutine > ptr;
      static const size_t StackSize;

    public:
      coroutine();

      virtual ~coroutine();

      virtual void go() = 0;

      virtual void start() = 0;

      void run( ::ucontext_t* return_to );

      void init();

      void yield();

			::ucontext_t* get_context();

    private:
      ::ucontext_t own_context;
      ::ucontext_t* return_context;
  };
}

#endif
