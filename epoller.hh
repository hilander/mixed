#ifndef LIBMIXED_SOCKET_HPP
#define LIBMIXED_SOCKET_HPP

#include <tr1/memory>
#include <map>
#include <vector>
#include <sys/epoll.h>

namespace scheduler
{

class poller
{

	public: // typedefs
		typedef std::tr1::shared_ptr< poller > ptr;

	public:

		static ptr get();

		/** \brief Triggeruj epoll-a: sprawdź, które sockety coś zapisały / odczytały
		 * \return true, gdy co najmniej jeden z socketów zmienił stan; false wpw.
		 */
		std::vector< ::epoll_event >* poll();

		bool add( int fd_, uint32_t flags ) throw( std::exception );

		bool add( int fd_ ) throw( std::exception );

		void remove( int fd_ );

    bool contains ( int fd_ );

    int size();

	public:

		void init();

    ~poller();

  private:

    poller();

    poller( poller& );

	private:
		int _fd;
    size_t current_sockets_number;
		::epoll_event* watched_sockets;
    std::map< int, ::epoll_event > _events;
    size_t watched_sockets_size;
};

}

#endif
