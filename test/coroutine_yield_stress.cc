#include <iostream>
#include <coroutine.hh>
#include <tr1/memory>
//#include <gtest/gtest.h>

#include "stopwatch-tool.hh"
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
    //cout << "ok. gone" << endl;
    for ( int i = 0; i < 1000000; i++ )
    yield();
    //cout << "ok. gone for the second time" << endl;
    //yield();
  }

  virtual void start()
  {
    go();
  }

   ~my_coroutine()
  {
  }

};

int main( int argc, char* argv[] )
{
  char stack[16384];
  ::ucontext_t ctx;
  ctx.uc_stack.ss_sp = stack;
  ctx.uc_stack.ss_size = 16384;
  ::getcontext( &ctx );

  coroutine::ptr c = my_coroutine::get();
  
  for ( int i = 0; i < 1000000; i++ )
  {
    c->run( &ctx );
  }
  //cout << "main thread: done." << endl;
  return 0;
}
