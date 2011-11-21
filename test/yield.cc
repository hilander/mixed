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

class yielder : public fiber
{
  public:
    typedef shared_ptr< yielder > ptr;
    yielder() {}
    virtual void go()
    {
      //yield();
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
    cout << "main: ok." << endl;
    }
    virtual ~starter()
    {}
};

TEST(Fiber, Yield)
{
  //master::ptr mp( master::create() );
  master* mp = master::create();
  starter::ptr sp( new starter() );
  sp->init();
  fiber::ptr fp = dynamic_pointer_cast< fiber >( sp );
  mp->spawn( fp );
  mp->run();
  cout << "main: ok." << endl;
}

int main( int argc, char* argv[] )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
