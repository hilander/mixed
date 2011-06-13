#include <iostream>
#include <coroutine.hh>
#include <tr1/memory>

using namespace std;
using namespace std::tr1;
using namespace coroutines;

class my_coroutine : public coroutines::coroutine
{
  public:
  typedef shared_ptr< my_coroutine > ptr;

  public:
  static ptr get()
  {
    ptr p( new my_coroutine() );
    p->init();
    return p;
  }

  virtual void go()
  {
    cout << "ok. gone" << endl;
    yield();
    cout << "ok. gone for the second time" << endl;
    yield();
  }

   ~my_coroutine()
  {
  }

};

int main(int,char**)
{
  char stack[16384];
  ::ucontext_t ctx;
  ctx.uc_stack.ss_sp = stack;
  ctx.uc_stack.ss_size = 16384;
  ::getcontext( &ctx );

  coroutine::ptr c = my_coroutine::get();
  c->run( &ctx );
  c->run( &ctx );
  cout << "main thread: done." << endl;
  return 0;
}
