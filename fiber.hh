#ifndef __FIBER_HH__
#define __FIBER_HH__

#include <tr1/memory>
#include <vector>

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
			BLOCKED_FOR_READ,
			BLOCKED_FOR_WRITE,
			BLOCKED_FOR_MESSAGE,
      RUNNING
    };

    public:
      fiber();

      virtual ~fiber();

      virtual void go() = 0;

      virtual void run( ::ucontext_t* return_to );

      current_state get_state();

			void set_state( current_state s );

			std::tr1::shared_ptr< std::vector< char > > get_buffer();

			void set_last_read( ssize_t s );

			void set_last_write( ssize_t s );

    private:
      current_state state;
			std::tr1::shared_ptr< std::vector< char > > rw_buffer;
			ssize_t last_read;
			ssize_t last_write;
  };
}

#endif
