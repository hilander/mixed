#include <tr1/memory>
using namespace std::tr1;

#include <vector>
using namespace std;

#include "message.hh"
using namespace message_queues;

#include "fiber.hh"
using namespace fibers;

#include "worker.hh"
using namespace workers;

#include "master.hh"
using namespace masters;

master::ptr master::create()
{
	master::ptr p( new master() );
	p->init();
	return p;
}

void master::run()
{
}

void master::init()
{
	own_slave = worker::create();

	cpu_set_t cs;
	if ( sched_getaffinity(0, sizeof(cs), &cs) == 0 )
	{
		for ( int free_cores = 0; free_cores < ( CPU_COUNT( &cs ) ) - 1; free_cores++ )
		{
			slaves.push_back( worker::create() );
		}
	}
}

worker::ptr master::get_worker_with_smallest_workload()
{
	worker::ptr sp = own_slave;

	for ( vector< worker::ptr >::iterator si = slaves.begin()
			; si != slaves.end()
			; si++ )
	{
		worker::ptr st = *si;
		if ( st.get() != 0 )
		{
			if ( sp->workload() > st->workload() )
			{
				sp = st;
			}
		}
	}

	return sp;
}

void master::spawn( fiber::ptr f )
{
	serv_message< service_message::SPAWN >::ptr p( new serv_message< service_message::SPAWN >() );
	p->fiber_to_spawn = f;
	worker::ptr s = get_worker_with_smallest_workload();
	message::ptr m = dynamic_pointer_cast< message >( p );
	s->write_to_slave( m );
}

master::master()
: workload( 0 )
{
}

