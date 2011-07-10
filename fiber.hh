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
  class fiber : public std::tr1::enable_shared_from_this< fiber >, public coroutines::coroutine
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
			BLOCKED_FOR_ACCEPT,
			BLOCKED_FOR_MESSAGE
    };

    public:
      fiber();

      virtual ~fiber();

      virtual void go() = 0;

      virtual void start();

			ssize_t do_read( int f, ssize_t s );

			ssize_t do_write( int f, ssize_t s );

			int do_accept( int f );

			void send_message( std::tr1::shared_ptr< message_queues::fiber_message > m );

			void receive_message( std::tr1::shared_ptr< message_queues::fiber_message >& p);
			std::tr1::shared_ptr< message_queues::fiber_message > receive_message_nonblock();

			void spawn( ptr f );

			// methods used by other components, not for normal user

      current_state get_state();

			void set_state( current_state s );

			std::tr1::shared_ptr< std::vector< char > > get_buffer();

			ssize_t get_rw_size();

			void set_last_accepted_fd( int f );

			void set_last_read( ssize_t s );

			void set_last_write( ssize_t s );

			void set_owner( std::tr1::shared_ptr< workers::worker > o );

			void put_into_message_buffer( std::tr1::shared_ptr< message_queues::fiber_message > m );

		protected:

			std::list< std::tr1::shared_ptr< message_queues::fiber_message > > message_buffer;

    private:
      current_state state;
			std::tr1::shared_ptr< std::vector< char > > rw_buffer;
			ssize_t rw_size;
			ssize_t last_read;
			ssize_t last_write;
			int last_accepted_fd;
			std::tr1::shared_ptr< workers::worker > owner;
  };
}

#endif
