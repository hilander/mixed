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

#include <iostream>
using namespace std;
void* worker_pthread_starter( worker* w )
{
  w->run();
  return 0;
}

master* master::create()
{
  master* p = new master();
  p->init();
  return p;
}

bool master::its_time_to_end()
{
  workload = workload > 0 ? workload : 0;
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
      //cout << "#"; cout.flush();
    }
  }

  /*
  if ( total_workload == 0 )
  {
    cout << "total_workload = 0" << endl;
  }
  */

  return total_workload == 0;
}

void master::run()
{
  while ( ! its_time_to_end() )
  {
    read_message_queues();
    own_slave->iteration();
  }
  //cout << "own_slave: workload = " << own_slave->workload() << endl;
  //cout << "Master: workload = " << workload << endl;
  //cout << "Master: Exit." << endl;
}

void master::init()
{
  own_slave.reset( worker::create() );
  own_slave->set_master( this );

  cpu_set_t cs;
  if ( sched_getaffinity(0, sizeof(cs), &cs) == 0 )
  {
    for ( int free_cores = 0; free_cores < ( CPU_COUNT( &cs ) ) - 1; free_cores++ )
    {
      worker::ptr w( worker::create() );
      w->set_master( this );
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
void master::spawn( fiber::ptr& f )
{
  service_message::ptr p( new service_message( service_message::SPAWN ) );
  p->fiber_to_spawn = f;
  worker::ptr s = get_worker_with_smallest_workload();
  message::ptr m = dynamic_pointer_cast< message >( p );
  s->write_to_slave( m );
  workload++;
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
          fiber::ptr fp = sm->fiber_to_spawn;
          worker::ptr s = get_worker_with_smallest_workload();
          s->write_to_slave( m );
          workload++;
          //cout << "spawn\n";
          break;
        }

      case service_message::SPAWN_REPLY:
        workload--;
        break;

      case service_message::BROADCAST_MESSAGE:
        {
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

void master::read_message_queues()
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

