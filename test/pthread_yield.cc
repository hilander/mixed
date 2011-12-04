#include <iostream>
#include <algorithm>
using namespace std;

#include <tr1/memory>
#include <tr1/functional>
using namespace std::tr1;

#include <pthread.h>

#include "stopwatch-tool.hh"
#include "getopt.hh"

class yielder
{
  public:
    typedef shared_ptr< yielder > ptr;
    yielder( int yields_number )
    : yields( yields_number )
    {}

    virtual void go()
    {
      for ( int i = 0; i < yields; i++ )
      {
        ::pthread_yield();
      }
    }
    virtual ~yielder()
    {}

  private:
    int yields;
};

void* run( int* params )
{
  yielder::ptr y( new yielder( *params ) );
  y->go();
  return 0;
}

int main( int argc, char* argv[] )
{
  getopts g;
  g.set( 's' );
  g.set( 'y' );
  g.parse( argc, argv );
  int yields_size = g.get< int >( 's' );
  int yields_per = g.get< int >( 'y' );
  stopwatch sw( stopwatch::USEC );
  sw.reset();
  vector< ::pthread_t > thread_handles( yields_size );
  for ( int thread_cnt = 0; thread_cnt < yields_size; thread_cnt++ )
  {
    ::pthread_create( &thread_handles[ thread_cnt ]
                    , 0
                    , reinterpret_cast< void*(*)(void*) >( &run )
                    , &yields_per );
  }
  
  for_each( thread_handles.begin(), thread_handles.end()
          , bind( &::pthread_join, placeholders::_1, (void**)0 ) );
  sw.stop();
  cout << sw.get_time() << endl;
  return EXIT_SUCCESS;
}
