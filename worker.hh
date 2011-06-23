#ifndef __WORKER_HH__
#define __WORKER_HH__

#include <tr1/memory>
#include <map>

#include "fiber.hh"
#include "scheduler.hh"
#include "message_queue.hh"
#include "epoller.hh"

namespace workers
{
  class worker : std::tr1::enable_shared_from_this< worker >
  {
    public:
      typedef std::tr1::shared_ptr< worker > ptr;

      virtual ~worker();

      void init();

      void run();

      void iteration();
      
			void block_on_io( int f, fibers::fiber::ptr fp, fiber::current_state s );

			void block_on_message( int m_id, fibers::fiber::ptr fp );

    private:
      worker();

      void do_epolls();
      void process_incoming_messages();
      void process_service_message( message::ptr m );
      void pass_message_to_fiber( message::ptr m );
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
