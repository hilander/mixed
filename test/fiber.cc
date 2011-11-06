#include <iostream>
#include <fiber.hh>
#include <gtest/gtest.h>

using namespace std;
using namespace fibers;

class myfiber : public fiber
{
  public:
    virtual void go()
    {
      //cout << "myfiber done." << endl;
    }
};

TEST( Fiber, Run )
{
  myfiber mf;
  mf.init();
  char stack[16384];
  ::ucontext_t ctx;
  ctx.uc_stack.ss_sp = stack;
  ctx.uc_stack.ss_size = 16384;
  ::getcontext( &ctx );
  mf.run( &ctx );
}

int main( int argc, char* argv[] )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
