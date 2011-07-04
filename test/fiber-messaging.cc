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

class sender : public fiber
{
	public:
		typedef shared_ptr< sender > ptr;
		sender()
		{
		}

		void set_receiver( fiber::ptr r )
		{
			rec = r;
		}
		virtual void go()
		{
			fiber_message::ptr fm( new fiber_message() );
			fm->sender = fiber::ptr( this );
			fm->receiver = rec;
			send_message( fm );
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

		void set_receiver( fiber::ptr r )
		{
			rec = r;
		}
		virtual void go()
		{
			fiber_message::ptr fm;
			fm  = receive_message();
			 cout << "receiver: ok" << endl;
		}

	private:
		fiber::ptr rec;
};

int main(int,char**)
{
	sender::ptr s( new sender() );
	receiver::ptr r( new receiver() );

	s->set_receiver( r );
	r->set_receiver( s );

	s->init();
	r->init();

	master::ptr m = master::create();
	m->spawn( s );
	m->spawn( r );
	m->run();
	cout << "main: ok." << endl;
	return 0;
}
