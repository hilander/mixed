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
      typedef std::tr1::shared_ptr< epoller > ptr;

			static ptr create();

			virtual ~epoller();

      int do_epolls();

			void add( int f );

			void del( int f );

			void init();

			::epoll_event* get_last_epoll_result();

    private:
			::epoll_event* raw_events;
			int rawevents_size;
      int fds;
			int own_fd;
  };
}

#endif
