#include <iostream>
using namespace std;

#include <tr1/memory>
using namespace std::tr1;

#include "fiber.hh"
using namespace fibers;

#include "message.hh"
using namespace message_queues;

#include <master.hh>
using namespace masters;

#include <cstdlib>
#include <sys/time.h>
#include <fcntl.h>

#include "stopwatch-tool.hh"

#include "getopt.hh"

int how_many, how_long;

class starter : public fiber
{
  public:

    typedef shared_ptr< starter > ptr;

    starter()
    {
    }

    virtual ~starter()
    {
    }

    virtual void go()
    {
      rw_buffer.assign( how_long+1, '.' );
      rw_buffer.at( how_long ) = '\n';
      stopwatch sw( stopwatch::USEC );
      sw.reset();
      for ( int i = 0; i < how_many; i++ )
      {
        do_write( STDERR_FILENO, rw_buffer.size() );
      }
      sw.stop();
      cout << sw.get_time() << endl;
    }
};

int main( int argc, char* argv[] )
{
  getopts g;
  g.set( 'a' );
  g.set( 'b' );
  g.parse( argc, argv );
  how_many = g.get< int >( 'a' );
  how_long = g.get< int >( 'b' );
  starter::ptr s( new starter() );
  s->init();
  master* m = master::create();
  fiber::ptr sf = dynamic_pointer_cast< fiber >( s );

  m->spawn( sf );
  m->run();
  return EXIT_SUCCESS;
}
