#include <iostream>
#include <sstream>
#include <cstdio>
#include <algorithm>
using namespace std;

#include <tr1/memory>
using namespace std::tr1;

#include <signal.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <errno.h>
#include <pthread.h>
#include <memory>
#include <vector>
#include <unistd.h>
#include <fcntl.h>

#include <fiber.hh>
using namespace fibers;

#include <master.hh>
using namespace masters;

const int default_port = 8100;

void s_err( int num, string& s );

class f_client : public fiber
{
  public:
    f_client( char* address_c_str, int port )
      : _addr( address_c_str )
			, _port( port )
    {}

    virtual void go()
    {
			int sa = init_socket();

			if ( sa < 0 )
			{
				return;
			}
			
			int n = 1000;
      char num[6];
			sprintf( num, "%6d", n );
			string buf( string( "HELLO:" ) + string( num ) );
			rw_buffer->resize( buf.size() );
			copy( buf.begin(), buf.end(), rw_buffer->begin() );
			do_write( sa, buf.size() );

			for ( int i = 0; i < n; i++ )
			{
				do_read( sa, 1 );
				do_write( sa, 1 );
			}
      ::close( sa );
    }

	private:

		int init_socket()
		{
      sockaddr_in sar;
      sar.sin_family = AF_INET;
      inet_pton( AF_INET, _addr, &sar.sin_addr );
      sar.sin_port = htons( _port );

      int sa = socket( AF_INET, SOCK_STREAM, 0 );
      int orig_flags = fcntl( sa, F_GETFL );
      fcntl( sa, F_SETFL, orig_flags | O_NONBLOCK );

      int sw = 0;
      
      do
      {
        sw = ::connect( sa, (const sockaddr*)&sar, sizeof(sar) );
				if ( sw != 0 )
				{
					yield();
				}
      }
      while ( sw != 0 && ( errno == EINPROGRESS || errno == EALREADY ) )
        ;

      if ( sw != 0 )
      {
        string error_name;
        s_err( errno, error_name );
        std::cout << "poller_client: connect() error: " << error_name << std::endl;
        return -1;
      }
			return sa;
		}

  private:
    char* _addr;
		int _port;
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
	using std::stringstream;

	if ( argc != 3 )
	{
		std::cout << "Error! Improper number of bytes!" << std::endl;
		return 1;
	}

  signal( SIGPIPE, SIG_IGN );
	master* mp = master::create();
	
	stringstream sstr;
	sstr << argv[2] ;
	int port;
	sstr >> port;

	fiber::ptr fcl( new f_client( argv[1], port ) );
	fcl->init();
  mp->spawn( fcl );
  mp->run();

  return 0;
}
