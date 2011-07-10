#include <sys/epoll.h>
#include <vector>
#include <map>
#include <tr1/memory>
using namespace std;
using namespace std::tr1;

#include "epoller.hh"
using namespace epollers;

shared_ptr< epoller > epoller::create()
{
	shared_ptr< epoller > px( new epoller() );
	px->init();
	return px;
}

epoller::epoller()
: raw_events( 0 )
, rawevents_size( 0 )
, fds( 0 )
, own_fd( 0 )
{
}

epoller::~epoller()
{
	if ( raw_events != 0 )
	{
		delete[] raw_events;
	}
	::close( own_fd );
}

void epoller::init()
{
	own_fd = ::epoll_create( 1024 );
	raw_events = new ::epoll_event[64];
	fds = 64;
}

int epoller::do_epolls()
{
	if ( fds > rawevents_size )
	{
		if ( raw_events != 0 )
		{
			delete[] raw_events;
		}
		raw_events = new ::epoll_event[fds];
		rawevents_size = fds;
	}

	if ( fds > 0 )
	{
		return ::epoll_wait( own_fd, raw_events, fds, 0 );
	}
	return 0;
}

void epoller::add( int f )
{
	::epoll_event ev;
	ev.events = EPOLLIN | EPOLLOUT | EPOLLPRI | EPOLLERR | EPOLLHUP;
	ev.data.fd = f;

	::epoll_ctl( own_fd, EPOLL_CTL_ADD, f, &ev );

	fds++;
}

void epoller::del( int f )
{
	::epoll_event ev;
	::epoll_ctl( own_fd, EPOLL_CTL_DEL, f, &ev );
	fds--;
}

::epoll_event* epoller::get_last_epoll_result()
{
	return this->raw_events;
}
