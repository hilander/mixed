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

class empty_fiber : public fiber
{
  public:
    typedef shared_ptr< empty_fiber > ptr;
    empty_fiber()
    {}

    virtual void go()
    {
    }
    virtual ~empty_fiber()
    {}
};

class starter : public fiber
{
  public:
    typedef shared_ptr< starter > ptr;
    starter( int how_many)
    : et_count( how_many )
    {}

    virtual void go()
    {
      for ( int i = 0; i < et_count; i++ )
      {
        fiber::ptr et( new empty_fiber() );
        et->init();
        spawn( et );
      }
    }
    virtual ~starter()
    {}

  private:
    int et_count;
};

int main( int argc, char* argv[] )
{
  getopts g;
  g.set( 's' );
  g.parse( argc, argv );
  int et_size = g.get< int >( 's' );
  stopwatch sw( stopwatch::USEC );
  sw.reset();
  master* mp = master::create( false );
  fiber::ptr sp( new starter( et_size ) );
  sp->init();
  mp->spawn( sp );
  mp->run();
  sw.stop();
  cout << sw.get_time() << endl;
  return EXIT_SUCCESS;
}
