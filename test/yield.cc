#include <tr1/memory>
using namespace std::tr1;

#include "fiber.hh"
using namespace fibers;

#include "master.hh"
using namespace masters;

class yielder : public fiber
{
  public:
    virtual void go()
    {
      yield();
    }
    virtual ~yielder()
    {}
};

class starter : public fiber
{
  public:
    virtual void go()
    {
      for ( int i = 0; i < 1000; i++ )
      {
        fiber::ptr fp( new yielder() );
        fp->init();
        spawn( fp );
      }
    }
    virtual ~starter()
    {}
};

int main(int,char**)
{
  master::ptr mp( master::create() );
  fiber::ptr sp( new starter() );
  sp->init();
  mp->spawn( sp );
  mp->run();
  return 0;
}
