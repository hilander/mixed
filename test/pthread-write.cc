#include <iostream>
#include <string>
using namespace std;

#include <pthread.h>
#include <cstdlib>
#include <sys/time.h>
#include <fcntl.h>

#include "stopwatch-tool.hh"

#include "getopt.hh"

int how_many, how_long;

class starter
{
  public:

    starter()
    {
    }

    virtual ~starter()
    {
    }

    virtual void go()
    {
      string buf( how_long, '.' );
      buf.push_back( '\n' );
      stopwatch sw( stopwatch::USEC );
      sw.reset();
      for ( int i = 0; i < how_many; i++ )
      {
        ::write( STDERR_FILENO, buf.c_str(), buf.size() );
      }
      sw.stop();
      cout << sw.get_time() << endl;
    }
};

void* do_start( starter* s )
{
  s->go();
  return 0;
}

int main( int argc, char* argv[] )
{
  getopts g;
  g.set( 'a' );
  g.set( 'b' );
  g.parse( argc, argv );
  how_many = g.get< int >( 'a' );
  how_long = g.get< int >( 'b' );
  starter s;

  ::pthread_t starter_handle;
  if ( ::pthread_create( &starter_handle
                       , 0
                       , reinterpret_cast< void*(*)(void*) >( &do_start )
                       , static_cast< void* >( &s ) ) )
  {
    return EXIT_FAILURE;
  }

  ::pthread_join( starter_handle, 0 );
  return EXIT_SUCCESS;
}
