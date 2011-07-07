#include <iostream>
using namespace std;

#include <tr1/memory>
using namespace std::tr1;

#include <fiber.hh>
using namespace fibers;

#include <message.hh>
using namespace message_queues;

#include <master.hh>
using namespace masters;

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
			my_message::ptr mm( new my_message() );
			mm->my_int = 42;
			fiber_message::ptr fm( dynamic_pointer_cast<fiber_message>( mm ) );
			fm->sender = fiber::ptr( this );
			fm->receiver = rec;
			send_message( fm );
			 cout << "sender: ok" << endl;
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
		}

		fiber::ptr get_ptr()
		{
			return shared_from_this();
		}

	private:
		fiber::ptr rec;
};

int main(int,char**)
{
	sender::ptr sp( new sender() );
	receiver::ptr rp( new receiver() );

	sp->set_receiver( rp->get_ptr() );
	rp->set_receiver( sp->get_ptr() );

	sp->init();
	rp->init();

	master* m = master::create();
	m->spawn( rp->get_ptr() );
	m->spawn( sp->get_ptr() );
	m->run();
	cout << "main: ok." << endl;
	return 0;
}
