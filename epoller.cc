#include <iostream>
#include "poller.hpp"
#include "mutex_trylock.hpp"
#include <pthread.h>
#include <sys/epoll.h>
#include <vector>

using std::tr1::shared_ptr;
using std::map;
using std::vector;

const int ignored_epoll_value = 1024;

const int initial_epolls = 128;

// epoll_wait(2) manpage:
// timeout = 0: return immediately even if no events are available
// timeout = 10: wait for events for 10 ms
const int epoll_timeout = 10;

scheduler::poller::ptr
scheduler::poller::get()
{
		shared_ptr< poller > i( new poller() );
		i->init();

	return i;
}

scheduler::poller::poller()
  : current_sockets_number( 0 )
  , watched_sockets( new ::epoll_event[ initial_epolls ]() )
  , watched_sockets_size( initial_epolls )
{
}

scheduler::poller::poller( scheduler::poller& )
{}

scheduler::poller::~poller()
{
  delete[] watched_sockets;
}

void
scheduler::poller::init()
{
  using scheduler::trylock;

	//init epoll
	_fd = epoll_create( ignored_epoll_value );
	if ( _fd == -1 )
	{
		throw std::exception();
	}
}

bool
scheduler::poller::add( int fd_ ) throw( std::exception)
{
  return add( fd_, EPOLLIN | EPOLLOUT | EPOLLPRI | EPOLLERR | EPOLLHUP );
}

bool
scheduler::poller::add( int fd_, uint32_t flags ) throw( std::exception )
{
  if ( current_sockets_number == watched_sockets_size )
  {
    throw std::exception();
  }

  std::pair<int, ::epoll_event > fd_pair( fd_, ::epoll_event() );
  current_sockets_number++;

	fd_pair.second.events = flags;
	fd_pair.second.data.fd = fd_;

	if ( ::epoll_ctl( _fd, EPOLL_CTL_ADD, fd_, &(fd_pair.second) ) == 0 )
	{
    _events.insert( fd_pair );
		return true;
	}
	else
	{
		return false;
	}
}

vector< ::epoll_event >*
scheduler::poller::poll()
{
  int events_number;
  //std::cout << "current_sockets_number: " << current_sockets_number << std::endl;
  if ( current_sockets_number > 0 )
  {
    events_number = epoll_wait( _fd, watched_sockets, watched_sockets_size, epoll_timeout );
    if ( events_number == -1 )
    {
      std::cout << "Epoll: error during poll!" << std::endl;
    }
  }
  else
  {
    return 0;
  }
  
	vector< ::epoll_event >* v;
	if ( events_number > 0 )
	{
		v = new vector< ::epoll_event>( events_number );
	}
	else
	{
		v = 0;
	}

  for ( int i = 0; i < events_number; i++ )
  {
    ( *v )[ i ].events = watched_sockets[ i ].events;
    ( *v )[ i ].data.fd = watched_sockets[ i ].data.fd;
  }
  
	return v;
}

void
scheduler::poller::remove( int fd_ )
{
  map< int, ::epoll_event>::iterator removed = _events.find( fd_ );

  ::epoll_ctl( _fd, EPOLL_CTL_DEL, fd_, &( removed->second ) );


  if ( removed != _events.end() )
  {
    _events.erase( removed );
  }

  current_sockets_number--;

}

bool
scheduler::poller::contains( int fd_ )
{
  return ( _events.find( fd_ ) == _events.end() ) ? false : true;
}

int 
scheduler::poller::size()
{
  return current_sockets_number;
}
