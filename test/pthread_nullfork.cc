#include <iostream>
#include <algorithm>
using namespace std;

#include <tr1/memory>
#include <tr1/functional>
using namespace std::tr1;

#include <pthread.h>

#include "stopwatch-tool.hh"
#include "getopt.hh"

class empty_thread
{
  public:
    typedef shared_ptr< empty_thread > ptr;
    empty_thread()
    {}

    virtual void go()
    {
    }
    virtual ~empty_thread()
    {}
};

void* run( void* )
{
  empty_thread::ptr et( new empty_thread() );
  et->go();
  return 0;
}

int main( int argc, char* argv[] )
{
  getopts g;
  g.set( 's' );
  g.parse( argc, argv );
  int et_size = g.get< int >( 's' );
  stopwatch sw( stopwatch::USEC );
  sw.reset();
  vector< ::pthread_t > thread_handles( et_size );
  for ( int thread_cnt = 0; thread_cnt < et_size; thread_cnt++ )
  {
    ::pthread_create( &thread_handles[ thread_cnt ]
                    , 0
                    , &run
                    , (void*)0 );
  }
  
  for_each( thread_handles.begin(), thread_handles.end()
          , bind( &::pthread_join, placeholders::_1, (void**)0 ) );
  sw.stop();
  cout << sw.get_time() << endl;
  return EXIT_SUCCESS;
}
