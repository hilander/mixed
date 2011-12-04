#ifndef __MASTER_HH__
#define __MASTER_HH__

#include <tr1/memory>

#include "fiber.hh"
#include "worker.hh"

namespace masters
{
  class master
  {
    public:
      typedef std::tr1::shared_ptr< master > ptr;

      static master* create( bool eio = true );

      void run();

      void init( bool enable_io = true );

      void spawn( fibers::fiber::ptr& f );

      virtual ~master() {}

    private:
      master();

      bool its_time_to_end();

      void read_from_slave( workers::worker::ptr s );
      void read_message_queues();

      std::tr1::shared_ptr< workers::worker > get_worker_with_smallest_workload();

      std::tr1::shared_ptr< workers::worker > own_slave;

      std::vector< std::tr1::shared_ptr< workers::worker > > slaves;
      std::vector< ::pthread_t* > slave_threads;

      int workload;
  };
}

#endif
