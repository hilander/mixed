#include <iostream>
using namespace std;

#include <tr1/memory>
using namespace std::tr1;

#include "fiber.hh"
using namespace fibers;

#include "master.hh"
using namespace masters;

#include <iostream>
#include "stopwatch-tool.hh"
#include "getopt.hh"

class yielder : public fiber
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
        yield();
      }
    }
    virtual ~yielder()
    {}

  private:
    int yields;
};

class starter : public fiber
{
  public:
    typedef shared_ptr< starter > ptr;
    starter( int how_many, int yields)
    : yielder_count( how_many )
    , yields_per_yielder( yields )
    {}

    virtual void go()
    {
      for ( int i = 0; i < yielder_count; i++ )
      {
        yielder::ptr yp( new yielder( yields_per_yielder ) );
        yp->init();
        fiber::ptr fp = dynamic_pointer_cast< fiber >( yp );
        spawn( fp );
      }
    }
    virtual ~starter()
    {}

  private:
    int yielder_count;
    int yields_per_yielder;
};

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
  master* mp = master::create( false );
  fiber::ptr sp( new starter( yields_size, yields_per ) );
  sp->init();
  mp->spawn( sp );
  mp->run();
  sw.stop();
  cout << sw.get_time() << endl;
  return EXIT_SUCCESS;
}
