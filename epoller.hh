#ifndef __EPOLLER_HH__
#define __EPOLLER_HH__

#include <sys/epoll.h>
#include <vector>
#include <map>
#include <tr1/memory>

namespace epollers
{
  class fail
  {
    public:
      fail( int e ) : error( e ) {}

      int error;
  };

  class epoller
  {
    public:
      std::vector< std::tr1::shared_ptr< ::epoll_event > > do_epolls();

    private:
      std::vector< std::tr1::shared_ptr< ::epoll_event > > events;
      std::map< int, std::tr1::weak_ptr< ::epoll_event > > fds;
  };
}

#endif
