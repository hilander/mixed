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

      static worker* create( bool eio = true );

      virtual ~worker();

      void init();

      void run();

      void iteration();

      void insert_fd( int32_t f );

      void remove_fd( int32_t f );

      void block_on_io( int32_t f, fibers::fiber::ptr fp, fiber::current_state s );

      void do_connect( int32_t f, fiber::ptr fp, fiber::current_state s );

      void block_on_message( fibers::fiber::ptr fp );

      void send_message( std::tr1::shared_ptr< message_queues::fiber_message > m );

      void spawn( fibers::fiber::ptr& f );

      bool read_for_master( std::tr1::shared_ptr< message_queues::message >& m );

      void write_to_slave( std::tr1::shared_ptr< message_queues::message >& m );

      int32_t workload();

      void set_master( std::tr1::shared_ptr< masters::master > m );

    private:
      worker( bool eio = true );

      void do_epolls();

      void process_incoming_message_queues();
      void process_service_message( message_queues::message::ptr& m );
      void pass_message_to_fiber( std::tr1::shared_ptr< message_queues::message >& m );
      bool finished();

      schedulers::scheduler::ptr sched;
      message_queues::message_queue::ptr pipe;
      epollers::epoller::ptr io_facility;

      std::map< int32_t, fibers::fiber::ptr > blocked_fds;
      std::map< int32_t, fibers::fiber::ptr > blocked_msgs;
      std::tr1::shared_ptr< masters::master > my_master;

      bool master_allowed;
      int32_t unspawned_fibers;
      bool enable_io;
  };
}

#endif
