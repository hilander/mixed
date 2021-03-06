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

#include <gtest/gtest.h>

struct my_message : public fiber_message
{
    typedef shared_ptr< my_message > ptr;
    int my_int;
};

class sender : public fiber
{
  public:
    typedef shared_ptr< sender > ptr;
    sender()
    {
    }
        virtual ~sender()
        {
        }

    void set_receiver( fiber::ptr r )
    {
      rec = r;
    }
    virtual void go()
    {
      /*
      my_message::ptr mm( new my_message() );
      mm->my_int = 42;
      fiber_message::ptr fm( dynamic_pointer_cast<fiber_message>( mm ) );
      fm->sender = fiber::ptr( this );
      fm->receiver = rec;
      send_message( fm );
       cout << "sender: ok" << endl;
       */
    }

    fiber::ptr get_ptr()
    {
      return shared_from_this();
    }

  private:
    fiber::ptr rec;
};

class receiver : public fiber
{
  public:
    typedef shared_ptr< receiver > ptr;
    receiver()
    {
    }
        virtual ~receiver()
        {
        }

    void set_receiver( fiber::ptr r )
    {
      rec = r;
    }
    virtual void go()
    {
      /*
      fiber_message::ptr fm;
      receive_message( fm ) ;
      shared_ptr< my_message > mm( dynamic_pointer_cast< my_message >( fm ) );
      if ( mm.get() != 0 )
      {
        cout << "receiver: ok: " << mm->my_int << endl;
      }
      else
      {
        cout << "receiver: Received trash" << endl;
      }
      */
    }

    fiber::ptr get_ptr()
    {
      return shared_from_this();
    }

  private:
    fiber::ptr rec;
};

//int main(int,char**)
TEST( libmixed, fiber_messaging )
{
  receiver::ptr rp( new receiver() );
  rp->init();

  master* m = master::create();
  fiber::ptr rpf = dynamic_pointer_cast< fiber >( rp );
  m->spawn( rpf );
  m->run();
  cout << "main: ok." << endl;
}

int main( int argc, char* argv[] )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}

