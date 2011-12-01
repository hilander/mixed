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
#include <fcntl.h>

#include <gtest/gtest.h>

#include "stopwatch-tool.hh"

#include "getopt.hh"

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
      rw_buffer.assign( msg.begin(), msg.end() );
      for ( int i = 0; i < 1000; i++ )
      {
        //::write( STDOUT_FILENO, (void*)&rw_buffer[0], rw_buffer.size() );
        do_write( STDOUT_FILENO, rw_buffer.size() );
      }
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
