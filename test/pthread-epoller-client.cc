#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>

#include <errno.h>
#include <cstring>
#include <cstdio>

#include <vector>
#include <iostream>
#include <string>
#include <tr1/memory>
using namespace std;
using namespace std::tr1;

string s_err( int num )
{
	switch (num)
	{
		case EACCES:
			return string ( "EACCES" );
			break;

		case EPERM:
			return string ( "EPERM" );
			break;

		case EADDRINUSE:
			return string ( "EADDRINUSE" );
			break;

		case EAFNOSUPPORT:
			return string ( "EAFNOSUPPORT" );
			break;

		case EAGAIN :
			return string ( "EAGAIN" );
			break;

		case EALREADY:
			return string ( "EALREADY" );
			break;

		case EBADF :
			return string ( "EBADF" );
			break;

		case ECONNREFUSED:
			return string ( "ECONNREFUSED" );
			break;

		case EFAULT:
			return string ( "EFAULT" );
			break;

		case EINPROGRESS:
			return string ( "EINPROGRESS" );
			break;

		case EINTR:
			return string ( "EINTR" );
			break;

		case EISCONN:
			return string ( "EISCONN" );
			break;

		case ENETUNREACH:
			return string ( "ENETUNREACH" );
			break;

		case ENOTSOCK:
			return string ( "ENOTSOCK" );
			break;

		case ETIMEDOUT:
			return string ( "EACCES" );
			break;

		default:
			return string ( "unknown" );
			break;
	}
}

class f_client
{
  public:
    f_client( string& a, int p, int n )
      : addr( a )
			, port( p )
      , num ( n )
      , fd ( 0 )
    {}

    virtual void go()
    {
			fd = init_socket();

			if ( fd < 0 )
			{
        ::close( fd );
				return;
			}
			
			int n = 1000;
      char transfer_number[6];
			sprintf( transfer_number, "%6d", n );
			string buf( string( "HELLO:" ) + string( transfer_number ) );
      ssize_t written_bytes = buf.size();
			char* b = (char*)buf.c_str();

			if ( ::write( fd, b, written_bytes ) == written_bytes )
			{
				//cout << "fiber_client: I wrote " << (int)written_bytes << " bytes." << endl;
			}

			char sndbuf[1];
			char recbuf[1];
			for ( int i = 0; i < n; i++ )
			{
				::read( fd, recbuf, 1 );
				sndbuf[0] = recbuf[0];
				::write( fd, sndbuf, 1 );
			}
			cout << num << ".."; cout.flush();
    }

	private:

		int init_socket()
		{
      sockaddr_in sar;
      sar.sin_family = AF_INET;
      inet_pton( AF_INET, addr.c_str(), &sar.sin_addr );
      sar.sin_port = htons( port );

      int sa = socket( AF_INET, SOCK_STREAM, 0 );

      int sw = 0;
      do
      {
        sw = ::connect( sa, (const sockaddr*)&sar, sizeof(sar) );
      }
      while ( sw != 0 && ( errno == EINPROGRESS || errno == EACCES || errno == EALREADY ) )
        ;

      if ( sw != 0 )
      {
        cout << "poller_client(" << num << "): connect() error: " << s_err( errno ) << endl;
        return -1;
      }
      else
      {
        return sa;
      }

		}

  private:
    string addr;
		int port;
    int num;
    int fd;
};

static void* starter( f_client* c )
{
  c->go();
  return 0;
}

int main(int,char**)
{
  const int overall_clients = 10;

  vector< shared_ptr< ::pthread_t > > threads( overall_clients );
  vector< shared_ptr< f_client > > clients( overall_clients );

  string s( "127.0.0.1" );
  for (int i = 0; i < overall_clients ; i++ )
  {
    threads[i].reset( new ::pthread_t() );
    clients[i].reset( new f_client( s, 8100, i ) );
    ::pthread_create( threads[ i ].get()
                    , 0
                    , reinterpret_cast< void* (*)(void*) >( &starter )
                    , clients[i].get() );
  }
  for ( int j = 0; j < overall_clients; j++ )
  {
    ::pthread_join( *threads[j], 0 );
  }
	cout << "client: end." << endl;
  return 0;
}
