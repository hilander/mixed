#include <iostream>
using namespace std;

#include <tr1/memory>
using namespace std::tr1;

#include "fiber.hh"
using namespace fibers;

#include "message.hh"
using namespace message_queues;

#include <master.hh>
using namespace masters;

#include <cstdlib>
#include <sys/time.h>

#include <gtest/gtest.h>

#include "s_err.hh"
#include "stopwatch-tool.hh"

class producer : public fiber
{
  public:

    typedef shared_ptr< producer > ptr;

    producer()
    {
    }

    virtual ~producer()
    {
    }

    virtual void go()
    {
      /*
      initialize_socket();
      for ( int connection = 0; connection < all_connections; connection++ )
      {
        accept_connection();
      }
      */
    }
    
    void initialize_socket()
    {
    }
};

class consumer : public fiber {
  public:

    typedef shared_ptr< consumer > ptr;

    consumer()
    {
    }

    virtual ~consumer()
    {
    }

    virtual void go()
    {
    }
};

class starter : public fiber
{
  public:

    typedef shared_ptr< starter > ptr;

    starter()
    {
    }

    virtual ~starter()
    {
    }

    virtual void go()
    {
      string msg( "producer: ok\n" );
      rw_buffer.resize( msg.size() );
      copy( &msg.c_str()[0], &msg.c_str()[msg.size()], rw_buffer.begin() );
      int s=0;
      do
      {
        s += do_write( STDOUT_FILENO, rw_buffer.size() );
      }
      while ( s < msg.size() ) ;

      fiber::ptr p( new producer );
      p->init();
      spawn( p );
    }
};

TEST( libmixed, fiber_messaging )
{
  stopwatch sw( stopwatch::USEC );
  sw.reset();
  starter::ptr s( new starter() );
  s->init();

  master* m = master::create();
  fiber::ptr sf = dynamic_pointer_cast< fiber >( s );
  m->spawn( sf );
  m->run();
  sw.stop();
  cout << "main: ok. run in " << sw.get_time() << " " << sw.str() << endl;
}

static void action_int( int action )
{
  cout << ::getpid() << "got " << action << endl;
  ::exit( EXIT_FAILURE );
}

int main( int argc, char* argv[] )
{
  signal( SIGPIPE, SIG_IGN );
  signal( SIGINT, &action_int );
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
