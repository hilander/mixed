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
{
}

epoller::~epoller()
{
	::close( own_fd );
}

void epoller::init()
{
	own_fd = ::epoll_create( 1024 );
}

const vector< shared_ptr< ::epoll_event > > epoller::do_epolls( int& how_many )
{
	how_many = 0;
	int fdss = fds.size();
	if ( fdss > 0 )
	{
		how_many = ::epoll_wait( own_fd, reinterpret_cast< epoll_event* >( &raw_events ), fdss, 0 );
	}
	return events;
}

void epoller::add( int f )
{
	shared_ptr< ::epoll_event > event(new ::epoll_event );
	event->events = EPOLLIN | EPOLLOUT | EPOLLPRI | EPOLLERR | EPOLLHUP;
	event->data.fd = f;

	::epoll_ctl( own_fd, EPOLL_CTL_ADD, f, event.get() );

	events.push_back( event );
	raw_events.push_back( *event );
	fds.insert( pair< int, weak_ptr< ::epoll_event > >( f, event ) );
}

void epoller::del( int f )
{
	::epoll_ctl( own_fd, EPOLL_CTL_DEL, f, events.back().get() );
}
