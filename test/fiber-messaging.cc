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
            my_message mm;
            mm.my_int = 42;
			fiber_message::ptr fm( &mm );
			fm->sender = fiber::ptr( this );
			fm->receiver = rec;
			send_message( fm );
			 cout << "sender: ok" << endl;
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
            my_message* mm = static_cast< my_message* >( fm.get() );
			cout << "receiver: ok: " << mm->my_int << endl;
		}

	private:
		fiber::ptr rec;
};

int main(int,char**)
{
	sender* s = new sender();
    sender::ptr sp( s );
	receiver* r = new receiver();
    receiver::ptr rp( r );

	s->set_receiver( rp );
	r->set_receiver( sp );

	s->init();
	r->init();

	master* m = master::create();
	m->spawn( rp );
	m->spawn( sp );
	m->run();
	cout << "main: ok." << endl;
	return 0;
}
