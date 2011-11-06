#include <tr1/memory>
using namespace std::tr1;

#include <iostream>
using namespace std;

#include <fiber.hh>
using namespace fibers;

#include <scheduler.hh>
using namespace schedulers;

#include <gtest/gtest.h>

class f : public fiber
{
  public:
    virtual void go()
    {
 //     cout << "f::go(): 1." << endl;
      yield();
  //    cout << "f::go(): 2." << endl;
    }
};

TEST( Scheduler, Run )
{
  scheduler::ptr s = scheduler::create();
  fiber::ptr my_f( new f );
  my_f->init();
  s->insert( my_f );
  my_f.reset( new f );
  my_f->init();
  s->insert( my_f );
  s->run();
  s->run();
}

int main( int argc, char* argv[] )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
