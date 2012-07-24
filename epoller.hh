#ifndef __EPOLLER_HH__
#define __EPOLLER_HH__

#include <sys/epoll.h>
#include <vector>
#include <map>
#include <tr1/memory>
#include <stdint.h>

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

      int32_t do_epolls();

      void add( int32_t f );

      void del( int32_t f );

      void init();

      ::epoll_event* get_last_epoll_result();

    private:
      ::epoll_event* raw_events;
      int32_t rawevents_size;
      int32_t fds;
      int32_t own_fd;
  };
}

#endif
