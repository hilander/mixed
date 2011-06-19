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
      char num[6];
			sprintf( num, "%6d", n );
			string buf( string( "HELLO:" ) + string( num ) );
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
      //while ( ::read( fd, recbuf, 1 ) != 0 )
      //{
      //}
      ::close( fd );
			//cout << "fiber_client(" << (int)num << "): end." << endl;
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
        cout << "poller_client(" << num << "): connect() error: " << endl;
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
  const int overall_clients = 100;

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
  return 0;
}
