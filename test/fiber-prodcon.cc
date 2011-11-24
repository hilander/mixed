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

#include "stopwatch-tool.hh"

class producer : public fiber
{
  public:
    typedef shared_ptr< producer > ptr;
    producer( int p )
    : port( p )
    {
    }

    virtual ~producer()
    {
    }

    virtual void go()
    {
    }
};

class consumer : public fiber
{
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
    }
};

//int main(int,char**)
TEST( libmixed, fiber_messaging )
{
  stopwatch sw( stopwatch::USEC );
  sw.reset();
  receiver::ptr rp( new starter() );
  rp->init();

  master* m = master::create();
  fiber::ptr rpf = dynamic_pointer_cast< fiber >( rp );
  m->spawn( rpf );
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
