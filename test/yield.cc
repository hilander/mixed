#include <iostream>
using namespace std;

#include <tr1/memory>
using namespace std::tr1;

#include "fiber.hh"
using namespace fibers;

#include "master.hh"
using namespace masters;

#include <gtest/gtest.h>

#include <iostream>
#include "stopwatch-tool.hh"

class yielder : public fiber
{
  public:
    typedef shared_ptr< yielder > ptr;
    yielder() {}
    virtual void go()
    {
      for ( int i = 0; i < 1000; i++ )
      {
        yield();
      }
    }
    virtual ~yielder()
    {}
};

class starter : public fiber
{
  public:
    typedef shared_ptr< starter > ptr;
    starter() {}
    virtual void go()
    {
      for ( int i = 0; i < 1000; i++ )
      {
        yielder::ptr yp( new yielder() );
        yp->init();
        fiber::ptr fp = dynamic_pointer_cast< fiber >( yp );
        spawn( fp );
      }
    }
    virtual ~starter()
    {}
};

TEST(Fiber, Yield)
{
  //master::ptr mp( master::create() );
  stopwatch sw;
  sw.reset();
  master* mp = master::create();
  starter::ptr sp( new starter() );
  sp->init();
  fiber::ptr fp = dynamic_pointer_cast< fiber >( sp );
  mp->spawn( fp );
  mp->run();
  sw.stop();
  cout << "time: " << sw.get_time() << " " << sw.str() << endl;
}

int main( int argc, char* argv[] )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
