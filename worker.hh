#ifndef __WORKER_HH__
#define __WORKER_HH__

#include <tr1/memory>

#include "scheduler.hh"
#include "message_queue.hh"
#include "epoller.hh"

namespace workers
{
  class worker
  {
    public:
      typedef std::tr1::shared_ptr< worker > ptr;

      virtual ~worker();

      void init();

      void run();
      void iteration();
      
    private:
      worker();

      void do_epolls();
      void process_messages();
      bool finished();

      schedulers::scheduler::ptr sched;
      message_queues::message_queue pipe;
      epollers::epoller::ptr io_facility;

      bool master_allowed;
  };
}

#endif
