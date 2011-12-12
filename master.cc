#include <algorithm>
using namespace std;

#include <pthread.h>

#include <tr1/memory>
#include <tr1/functional>
using namespace std::tr1;
using namespace std::tr1::placeholders;

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

#include <cassert>

#include <iostream>
using namespace std;
void* worker_pthread_starter( worker* w )
{
  w->run();
  return 0;
}

master* master::create( bool eio )
{
  master* p = new master();
  p->init( eio );
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
    //if ( vi->get() != 0 )
    //{
      worker::ptr tp = *vi;
      total_workload += tp->workload();
    //}
  }

  //send FINISH_WORK to all workers
  if ( total_workload == 0 )
  {
    vector< ::pthread_t* >::iterator wit;
    //for ( wit = slave_threads.begin(); wit != slave_threads.end(); wit++ )
    for ( vi = slaves.begin(); vi != slaves.end(); vi++ )
    {
      message::ptr m( new service_message( service_message::FINISH_WORK ) );
      (*vi)->write_to_slave( m );
      //::pthread_cancel( *(*wit) );
    }
    message::ptr m( new service_message( service_message::FINISH_WORK ) );
    own_slave->write_to_slave( m );
    //std::cout << "its_time_to_end(): finishing work of child threads\n";
  }

  if ( total_workload <= 0 )
  {
    cout << "master: total_workload = " << total_workload << endl;
  }
  return total_workload == 0;
}

void internal_join( ::pthread_t* pt )
{
  ::pthread_join( *pt, 0 );
}

void master::run()
{
  for_each( waiting_spawn_requests.begin(), waiting_spawn_requests.end()
          , bind( &master::process_spawn_reqs, this, _1 ) );
  while ( ! its_time_to_end() )
  {
    read_message_queues();
    own_slave->iteration();
  }
  //own_slave->run();
  cout << "Master: waiting for slave threads." << its_time_to_end() << endl;
  //if ( its_time_to_end() > 0 )
  //{
   // goto once_again;
  //}
  //for_each( slave_threads.begin(), slave_threads.end(), &internal_join );
  //cout << "own_slave: workload = " << own_slave->workload() << endl;
  //cout << "Master: workload = " << workload << endl;
//once_again:
  //;
  //cout << "Master: Exit." << endl;
}

void master::init( bool enable_io )
{
  own_slave.reset( worker::create( enable_io ) );
  own_slave->set_master( this );

  cpu_set_t cs;
  if ( sched_getaffinity(0, sizeof(cs), &cs) == 0 )
  {
    for ( int free_cores = 0; free_cores < ( CPU_COUNT( &cs ) ) - 1; free_cores++ )
    {
      worker::ptr w( worker::create( enable_io ) );
      w->set_master( this );
      slaves.push_back( w );
      ::pthread_t* pt = new ::pthread_t;
      ::pthread_create( pt
          , 0
          , reinterpret_cast< void*(*)(void*) >( &worker_pthread_starter )
          , reinterpret_cast< void* >( w.get() ) );
      slave_threads.push_back( pt );
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
  //message::ptr m = static_pointer_cast< message >( p );
  waiting_spawn_requests.push_back( p );
  //own_slave->write_to_master( m );
  //workload++;
}

void master::read_from_slave( worker::ptr s )
{
  message::ptr m;

  while ( s->read_for_master( m ) )
  {
    cout << "."; cout.flush();
    service_message::ptr sm = dynamic_pointer_cast< service_message >( m );

    assert( sm.get() != 0 );

    switch ( sm->service )
    {
      case service_message::SPAWN:
        {
          fiber::ptr fp = sm->fiber_to_spawn;
          worker::ptr s = get_worker_with_smallest_workload();
          s->write_to_slave( m );
          workload++;
          cout << "SPAWN " << ( s.get() == own_slave.get() ? "to own_slave" : "to foreign slave" ) << s.get() << ", wload=" << workload << endl;
          break;
        }

      case service_message::SPAWN_REPLY:
        workload--;
          cout << "SPAWN_REPLY from " << s.get() << "; wload=" << workload << endl;
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

void master::process_spawn_reqs( service_message::ptr r )
{
  fiber::ptr fp = r->fiber_to_spawn;
  worker::ptr s = get_worker_with_smallest_workload();
  message::ptr m = static_pointer_cast< message >( r );
  s->write_to_slave( m );
  workload++;
  cout << "process_spawn_reqs(): " << ( s.get() == own_slave.get() ? "to own_slave" : "to foreign slave" ) << s.get() << ", wload=" << workload << endl;
}

master::master()
: workload( 0 )
{
}

