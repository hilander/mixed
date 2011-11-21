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

struct my_message : public fiber_message
{
    typedef shared_ptr< my_message > ptr;
    int my_int;
};

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
    }

  private:
    fiber::ptr rec;
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

  private:
    fiber::ptr rec;
};

class receiver : public fiber
{
  public:
    typedef shared_ptr< receiver > ptr;
    receiver()
    {
    }
        virtual ~receiver()
        {
        }

    void set_receiver( fiber::ptr r )
    {
      rec = r;
    }
    virtual void go()
    {
    }

  private:
    fiber::ptr rec;
};

//int main(int,char**)
TEST( libmixed, fiber_messaging )
{
  stopwatch sw( stopwatch::USEC );
  sw.reset();
  receiver::ptr rp( new receiver() );
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
