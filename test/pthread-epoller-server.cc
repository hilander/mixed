#include <sys/socket.h>
#include <netdb.h>

#include <string>
#include <iostream>
#include <tr1/memory>
using namespace std;
using namespace std::tr1;

#include <epoller.hh>
using namespace epollers;

class server_socket
{
	public:
		typedef shared_ptr< server_socket > ptr;

		static ptr create( int p, string& a )
		{
			shared_ptr< server_socket > ss( new server_socket( p, a ) );
			ss->init();
			return ss;
		}

	private:
		server_socket( int p, string& a )
			: port( p )
			, address( a )
		{
		}

		void init()
		{
			// create socket:
			shared_ptr< ::protoent > pe( ::getprotobyname( "tcp" ) );
      int sa = ::socket( AF_INET, SOCK_STREAM, pe->p_proto );

      ::sockaddr_in sar;
      sar.sin_family = AF_INET;
      sar.sin_addr.s_addr = INADDR_ANY;
      sar.sin_port = htons( port );
			if ( bind( sa, reinterpret_cast< sockaddr* > &sar, sizeof( ::sockaddr_in ) ) != 0 )
			{
				cout << "fiber_server: bind() error" << endl;
			}
		}

		int port;
		string address;
};

int main(int,char**)
{
	string address = string( "127.0.0.1" );
	server_socket::ptr p = server_socket::create( 8100, address );
	return 0;
}
