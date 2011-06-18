#ifndef __EPOLLER_HH__
#define __EPOLLER_HH__

#include <sys/epoll.h>
#include <vector>
#include <map>
#include <tr1/memory>

namespace epollers
{
  class epoller
  {
		private:
			epoller();

    public:
			static std::tr1::shared_ptr< epoller > create();

			virtual ~epoller();

      const std::vector< std::tr1::shared_ptr< ::epoll_event > > do_epolls( int& how_many );

			void add( int f );

			void del( int f );

			void init();
    private:
      std::vector< std::tr1::shared_ptr< ::epoll_event > > events;
      std::vector< ::epoll_event > raw_events;
      std::map< int, std::tr1::weak_ptr< ::epoll_event > > fds;
			int own_fd;
  };
}

#endif
