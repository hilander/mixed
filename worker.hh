#ifndef __WORKER_HH__
#define __WORKER_HH__

#include <tr1/memory>
#include <map>

#include "fiber.hh"
#include "scheduler.hh"
#include "message_queue.hh"
#include "epoller.hh"

namespace masters
{
	class master;
}

namespace workers
{
  class worker
  {
    public:
      typedef std::tr1::shared_ptr< worker > ptr;

			static ptr create();

      virtual ~worker();

      void init();

      void run();

      void iteration();
      
			void block_on_io( int f, fibers::fiber::ptr fp, fiber::current_state s );

			void block_on_message( fibers::fiber::ptr fp );

			void send_message( std::tr1::shared_ptr< message_queues::fiber_message > m );

			void read_for_master( std::tr1::shared_ptr< message_queues::message >& m );

			void write_to_slave( std::tr1::shared_ptr< message_queues::message >& m );

			int workload();

    private:
      worker();

      void do_epolls();

      void process_incoming_messages();
      void process_service_message( message::ptr m );
      void pass_message_to_fiber( std::tr1::shared_ptr< message_queues::message > m );
      bool finished();

      schedulers::scheduler::ptr sched;
      message_queues::message_queue pipe;
      epollers::epoller::ptr io_facility;

			std::map< int, fibers::fiber::ptr > blocked_fds;
			std::map< int, fibers::fiber::ptr > blocked_msgs;

      bool master_allowed;
  };
}

#endif
