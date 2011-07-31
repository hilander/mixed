#ifndef __SCHEDULER__HH__
#define __SCHEDULER__HH__

#include <tr1/memory>

#include <list>

#include "fiber.hh"

namespace workers
{
	class worker;
}

namespace schedulers
{
	class scheduler
	{
		public:
			typedef std::tr1::shared_ptr< scheduler > ptr;

			static ptr create();

			static ptr create( std::tr1::shared_ptr< workers::worker > o );

			void run();

			int workload();

			void init();

			void insert( fibers::fiber::ptr f );

			void set_owner( std::tr1::shared_ptr< workers::worker > o );

			bool has_fiber( fibers::fiber::ptr f );

		private:
			scheduler();

			void remove_finished();

			std::list< fibers::fiber::ptr > runners;
			std::tr1::shared_ptr< ::ucontext_t > own_context;
			std::tr1::shared_ptr< vector< char > > own_stack;
			std::tr1::shared_ptr< workers::worker > owner;
	};
}

#endif
