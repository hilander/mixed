#include <tr1/memory>
using namespace std::tr1;

#include <iostream>
#include <sstream>
#include <algorithm>
using namespace std;

//#include <signal.h>
#include <netdb.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <pthread.h>
#include <memory>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

#include <message.hh>
using namespace message_queues;

#include <fiber.hh>
using namespace fibers;

#include <master.hh>
using namespace masters;

const int default_port = 8100;

void s_err( int num, string& s );

class f_listener : public fibers::fiber
{
	public:

		f_listener( int fd_, fiber::ptr parent_ )
		: fd ( fd_ )
		, parent( parent_ )
		{}

		virtual void go()
		{
			const int init_message_size = 12;

			rw_buffer->resize( init_message_size );
				cout << "f_listener: fiber_buffer Size = " << rw_buffer->size() << endl;

      char buf[ init_message_size ];
      ssize_t read_bytes =  init_message_size ;

			cout << "f_listener: begin" << endl;
			if ( do_read( fd, read_bytes ) != read_bytes )
			{
				throw exception();
			}
			else
			{
				/*
				for ( int i=0; i<fiber_buffer->size(); i++ )
				{
				}
				*/
				//fiber_buffer = get_buffer();
				cout << "f_listener: Message read. Size = " << rw_buffer->size() << endl;
				//copy( fiber_buffer->begin(), fiber_buffer->end(), buf );
			}
			string s = string( rw_buffer->begin(), rw_buffer->end() ).substr( 6, init_message_size );
			std::stringstream sstr;
			sstr << string( rw_buffer->begin(), rw_buffer->end() ).substr( 6, init_message_size );
			int bytes = 0;
			sstr >> bytes;

			char sndbuf[1];
			char recbuf[1];
			sndbuf[0] = 42;

			for ( int i = 0; i < bytes; i++ )
			{
				socket_write( 1 );
				sndbuf[0] = rw_buffer->at( 0 );
				socket_read( 1 );
				recbuf[0] = rw_buffer->at( 0 );
				if ( sndbuf[0] != recbuf[0] )
				{
					std::cout << "Server listener: Client response incorrect." << std::endl;
					break;
				}
			}
			cout << "Server listener: end." << endl;
			::close( fd );
		}

	private:
		void socket_read( ssize_t bytes )
		{
			if ( do_read( fd, bytes ) != bytes )
			{
				//cout << "f_listener::socket_read(): exception" << endl;
			}
		}

		void socket_write( ssize_t bytes )
		{
			if ( do_write( fd, bytes ) != bytes )
			{
				//cout << "f_listener::socket_write(): exception" << endl;
			}
		}

	private:
		int fd;
		fiber::fiber::ptr parent;
};

class f_client : public fiber
{
  public:
    f_client( char* address_c_str )
      : _addr( address_c_str )
    {}

    virtual void go()
    {

			int max_opened = 2;
			int sa = init_socket();
			if ( sa < 0 )
			{
				return;
			}

			int opened_sockets;
			for ( opened_sockets = 0; opened_sockets < max_opened;  )
			{
				int sw = wait_for_connection( sa );

				if ( sw <= 0 )
				{
					string error_name;
					s_err( errno, error_name );
					std::cout << "poller_client: connect() error: " << error_name << std::endl;
					//return;
				}
				else
				{
					create_listener( sw );
					opened_sockets++;
				}
			}

			::close( sa );
			std::cout << "Server: exiting. " << std::endl;
    }

	private:

		void create_listener( int listen_descriptor )
		{
			fiber::ptr l( new f_listener( listen_descriptor, shared_from_this() ) );
			l->init();
			spawn( l );
			listeners.push_back( l );
		}

		int init_socket()
		{
      ::protoent *pe = getprotobyname( "tcp" );

      sockaddr_in sar;
      sar.sin_family = AF_INET;
      sar.sin_addr.s_addr = INADDR_ANY;
      sar.sin_port = htons( default_port );

      int sa = socket( AF_INET, SOCK_STREAM, pe->p_proto );
      int orig_flags = fcntl( sa, F_GETFL );
      fcntl( sa, F_SETFL, orig_flags | O_NONBLOCK );

      if ( bind( sa, (sockaddr*)&sar, sizeof(sockaddr_in) ) != 0 )
      {
        string error_name;
        s_err( errno, error_name );
        std::cout << "fiber_server: bind() error: " << error_name << std::endl;
        return -1;
      }

      if ( listen( sa, 10 ) != 0 )
      {
        string error_name;
        s_err( errno, error_name );
        std::cout << "fiber_server: bind() error: " << error_name << std::endl;
        return -1;
      }
			return sa;
		}

		int wait_for_connection( int sa )
		{
			return do_accept( sa );
		}

  private:
    char* _addr;
		std::list< fiber::fiber::ptr > listeners;
};

void s_err( int num, string& s )
{
  s.clear();

	switch (num)
	{
		case EACCES:
			s = string ( "EACCES" );
			break;

		case EPERM:
			s = string ( "EPERM" );
			break;

		case EADDRINUSE:
			s = string ( "EADDRINUSE" );
			break;

		case EAFNOSUPPORT:
			s = string ( "EAFNOSUPPORT" );
			break;

		case EAGAIN :
			s = string ( "EAGAIN" );
			break;

		case EALREADY:
			s = string ( "EALREADY" );
			break;

		case EBADF :
			s = string ( "EBADF" );
			break;

		case ECONNREFUSED:
			s = string ( "ECONNREFUSED" );
			break;

		case EFAULT:
			s = string ( "EFAULT" );
			break;

		case EINPROGRESS:
			s = string ( "EINPROGRESS" );
			break;

		case EINTR:
			s = string ( "EINTR" );
			break;
		case EISCONN:
			s = string ( "EISCONN" );
			break;

		case ENETUNREACH:
			s = string ( "ENETUNREACH" );
			break;

		case ENOTSOCK:
			s = string ( "ENOTSOCK" );
			break;

		case ETIMEDOUT:
			s = string ( "EACCES" );
			break;
	}
}

int main(int argc ,char* argv[])
{
  signal( SIGPIPE, SIG_IGN );
  char loopback[] = "127.0.0.1";
	master* mp = master::create();

	f_client::ptr fcl( new f_client( ( argc == 2 ) ? argv[1] : loopback ) );
	fiber::ptr cl = dynamic_pointer_cast< fiber >( fcl );
	fcl->init();
  mp->spawn( cl );
  mp->run();

  return 0;
}
