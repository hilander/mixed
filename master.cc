#include <algorithm>
#include <exception>
#include <iostream>
using namespace std;

#include <pthread.h>
#include <unistd.h>

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

#include <iostream>
using namespace std;
void* worker_pthread_starter( worker* w )
{
  w->run();
  return 0;
}

master::ptr master::create( bool eio )
{
  master::ptr p( new master() );
  p->init( eio );
  return p;
}

void internal_join( ::pthread_t* pt )
{
  ::pthread_join( *pt, 0 );
}

void master::run()
{
  while ( true )
  {
    read_message_queues();
    own_slave->iteration();
    ::usleep( 100 );
  }
}

void master::init( bool enable_io )
{
  own_slave.reset( worker::create( enable_io ) );
  own_slave->set_master( shared_from_this() );

  cpu_set_t cs;
  if ( sched_getaffinity(0, sizeof(cs), &cs) == 0 )
  {
    for ( int32_t free_cores = 0; free_cores < ( CPU_COUNT( &cs ) ) - 1; free_cores++ )
    {
      worker::ptr w( worker::create( enable_io ) );
      w->set_master( shared_from_this() );
      slaves.push_back( w );
      ::pthread_t* pt = new ::pthread_t;
      ::pthread_create( pt
                      , 0
                      , reinterpret_cast< void*(*)(void*) >( &worker_pthread_starter )
                      , reinterpret_cast< void* >( w.get() ) );
      slave_threads.push_back( pt );
    }
  }
  cout << "master::init(): spawned " << slave_threads.size() + 1 << " slaves " << endl;
}

worker::ptr master::get_worker_with_smallest_workload()
{
  worker::ptr sp;
  static size_t current_slave = 0;
  if ( current_slave > 0 )
  {
    sp = slaves[ current_slave - 1 ];
  }
  else
  {
    sp = own_slave;
  }
  ++current_slave;
  current_slave %= slaves.size() + 1;
  cout << "current slave: " << current_slave << endl;
  return sp;
}

//using namespace std;
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
    static int num = 1;
    cout << "master::read_from_slave(): got message #" << num++ << endl;
    service_message::ptr sm = dynamic_pointer_cast< service_message >( m );
    switch ( sm->service )
    {
      case service_message::SPAWN:
        {
          cout << "Type = SPAWN" << endl;
          fiber::ptr fp = sm->fiber_to_spawn;
          worker::ptr s = get_worker_with_smallest_workload();
          s->write_to_slave( m );
          workload++;
          break;
        }

      case service_message::SPAWN_REPLY:
        cout << "Type = SPAWN_REPLY" << endl;
        workload--;
        break;

      case service_message::BROADCAST_MESSAGE:
        {
          cout << "Type = BROADCAST_MESSAGE" << endl;
          own_slave->write_to_slave( m );
          for_each( slaves.begin(), slaves.end()
                  , bind( &worker::write_to_slave, _1, m ) );
          break;
        }

      case service_message::SHUTDOWN:
      {
        cout << "Type = SHUTDOWN" << endl;
        service_message::ptr msg( new service_message( service_message::FINISH_WORK ) );
        message::ptr mm = static_pointer_cast< message >( msg );
        for_each( slaves.begin(), slaves.end()
                , bind( &worker::write_to_slave, _1, mm ) );
        own_slave->write_to_slave( mm );
      }
        break;

      default:
        cout << "Type = exception!" << endl;
        throw exception();
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

