#include <pthread.h>

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

void* worker_pthread_starter( worker* w )
{
	return 0;
}

master* master::create()
{
	master* p = new master();
	p->init();
	return p;
}

#include <iostream>
using namespace std;
bool master::its_time_to_end()
{
	int total_workload = own_slave->workload() + workload;

	vector< worker::ptr >::iterator vi = slaves.begin();
	for (
			; vi != slaves.end()
			; vi++ )
	{
		if ( vi->get() != 0 )
		{
			worker::ptr tp = *vi;
			total_workload += tp->workload();
		}
	}

//	cout << "total workload: " << total_workload << endl;
	return total_workload == 0;
}

void master::run()
{
	while ( ! its_time_to_end() )
	{
		read_messages();
		own_slave->iteration();
	}
}

void master::init()
{
	own_slave.reset( worker::create() );

	cpu_set_t cs;
	if ( sched_getaffinity(0, sizeof(cs), &cs) == 0 )
	{
		for ( int free_cores = 0; free_cores < ( CPU_COUNT( &cs ) ) - 1; free_cores++ )
		{
			worker::ptr w( worker::create() );
			slaves.push_back( w );
			::pthread_t pt;
			::pthread_create( &pt
                            , 0
                            , reinterpret_cast< void*(*)(void*) >( &worker_pthread_starter )
                            , reinterpret_cast< void* >( w.get() ) );
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

#include <iostream>
using namespace std;
void master::spawn( fiber::ptr f )
{
	serv_message< service_message::SPAWN >::ptr p( new serv_message< service_message::SPAWN >() );
	p->fiber_to_spawn = f;
	worker::ptr s = get_worker_with_smallest_workload();
	message::ptr m = dynamic_pointer_cast< message >( p );
	s->write_to_slave( m );
	workload++;
    cout << "write_to_slave done" << endl;
}

void master::read_from_slave( worker::ptr s )
{
	message::ptr m;

	while ( s->read_for_master( m ) )
	{
		service_message::ptr sm = dynamic_pointer_cast< service_message >( m );
		switch ( sm->service )
		{
			case service_message::SPAWN:
				{
					serv_message< service_message::SPAWN >::ptr spm = dynamic_pointer_cast< serv_message< service_message::SPAWN > >( sm );
                    if ( spm.get() != 0 )
                    {
                        fiber::ptr fp = spm->fiber_to_spawn;
                        spawn( fp );
                    }
                    cout << "master: SPAWN" << workload << endl;
				workload--;
					break;
				}

			case service_message::SPAWN_REPLY:
				workload--;
                    cout << "master: SPAWN_REPLY" << endl;
				break;

			case service_message::BROADCAST_MESSAGE:
				{
                    cout << "master: BROADCAST_MESSAGE" << endl;
					own_slave->write_to_slave( m );
					vector< worker::ptr >::iterator si = slaves.begin();
					for (
							; si != slaves.end()
							; si++ )
					{
						worker::ptr sl = *si;
						if ( sl.get() != 0 )
						{
							sl->write_to_slave( m );
						}
					}
					break;
				}

			case service_message::FINISH_WORK:
				break;
		}
	}
}

void master::read_messages()
{
	vector< worker::ptr >::iterator sli = slaves.begin();
	for (
			; sli != slaves.end()
			; sli++ )
	{
		worker::ptr sl = *sli;
		if ( sl.get() != 0 )
		{
			read_from_slave( sl );
		}
	}
	read_from_slave( own_slave );
}

master::master()
: workload( 0 )
{
}

