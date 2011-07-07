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

			static master* create();

			void run();

			void init();

			void spawn( fibers::fiber::ptr f );

		private:
			master();

			bool its_time_to_end();

			void read_from_slave( workers::worker::ptr s );
			void read_messages();

			std::tr1::shared_ptr< workers::worker > get_worker_with_smallest_workload();

			std::tr1::shared_ptr< workers::worker > own_slave;

			vector< std::tr1::shared_ptr< workers::worker > > slaves;

			int workload;
	};
}

#endif
