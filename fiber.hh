#ifndef __FIBER_HH__
#define __FIBER_HH__

#include <tr1/memory>
#include <vector>
#include <list>

#include "coroutine.hh"

namespace message_queues
{
	struct fiber_message;
}

namespace workers
{
	class worker;
}

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
			BLOCKED_FOR_MESSAGE
    };

    public:
      fiber();

      virtual ~fiber();

      virtual void go() = 0;

      virtual void start();

			ssize_t do_read( int f );

			ssize_t do_write( int f );

			// always non-blocking
			void send_message( std::tr1::shared_ptr< message_queues::fiber_message > m );

			std::tr1::shared_ptr< message_queues::fiber_message > receive_message();
			std::tr1::shared_ptr< message_queues::fiber_message > receive_message_nonblock();

			void spawn( ptr f );

			// methods used by other components, not for normal user

      current_state get_state();

			void set_state( current_state s );

			std::tr1::shared_ptr< std::vector< char > > get_buffer();

			void set_last_read( ssize_t s );

			void set_last_write( ssize_t s );

			void set_owner( std::tr1::shared_ptr< workers::worker > o );

			void put_into_message_buffer( std::tr1::shared_ptr< message_queues::fiber_message > m );

		protected:

			std::list< std::tr1::shared_ptr< message_queues::fiber_message > > message_buffer;

    private:
      current_state state;
			std::tr1::shared_ptr< std::vector< char > > rw_buffer;
			ssize_t last_read;
			ssize_t last_write;
			std::tr1::shared_ptr< workers::worker > owner;
  };
}

#endif
