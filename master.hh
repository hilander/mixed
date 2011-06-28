#ifndef __MASTER_HH__
#define __MASTER_HH__

#include <tr1/memory>

#include "fiber.hh"

namespace masters
{
	class master
	{
		public:
			typedef std::tr1::shared_ptr< master > ptr;

			static ptr create();

			void run();

			void init();

			void spawn( fibers::fiber::ptr f );

		private:
			master();

			std::tr1::shared_ptr< workers::worker > get_worker_with_smallest_workload();

			std::tr1::shared_ptr< workers::worker > own_slave;

			vector< std::tr1::shared_ptr< workers::worker > > slaves;
	};
}

#endif
