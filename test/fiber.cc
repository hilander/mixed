#include <iostream>
#include <fiber.hh>

using namespace std;
using namespace fibers;

class myfiber : public fiber
{
  public:
    virtual void go()
    {
      cout << "myfiber done." << endl;
      yield();
    }
};

int main(int,char**)
{
  myfiber mf;
  mf.init();
  char stack[16384];
  ::ucontext_t ctx;
  ctx.uc_stack.ss_sp = stack;
  ctx.uc_stack.ss_size = 16384;
  ::getcontext( &ctx );
  mf.run( &ctx );

  cout << "main thread done. Fiber state: " << ( mf.get_state() == fiber::FINISHED ? "FINISHED" : "Not changed" ) << endl;
  return 0;
}
