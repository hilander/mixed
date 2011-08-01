#include <sys/socket.h>
#include <netdb.h>
#include <signal.h>

#include <string>
#include <sstream>
#include <iostream>
#include <tr1/memory>
#include <list>
#include <algorithm>
using namespace std;
using namespace std::tr1;

#include <epoller.hh>
using namespace epollers;

class f_listener
{
  public:
    f_listener( int f )
    : fd( f )
    {
    }

    void go()
    {
      char buf[12];
      ssize_t read_bytes = 12;

      ::read( fd, buf, read_bytes );
      string s = string(buf).substr(6,12);
      std::stringstream sstr;
      sstr << string(buf).substr(6,12);
      int bytes = 0;
      sstr >> bytes;

      char sndbuf[1];
      char recbuf[1];
      sndbuf[0] = 42;
      for ( int i = 0; i < bytes; i++ )
      {
        ::write( fd, sndbuf, 1 );
        ::read( fd, recbuf, 1 );
        if ( sndbuf[0] != recbuf[0] )
        {
          std::cout << "Server listener: Client response incorrect." << std::endl;
          break;
        }
      }

      while ( ::read( fd, recbuf, 1 ) != 0 )
      {
      }
      //std::cout << "Server listener: end." << std::endl;
      ::close( fd );
    }

  private:
    int fd;
};

static void* unified_starter( f_listener* s )
{
  s->go();
  return 0;
}

class wait_for_end
{
  public:
  void operator() ( shared_ptr< ::pthread_t > p )
  {
    ::pthread_join( *p, 0 );
  }
};

class server_socket
{
  public:
    typedef shared_ptr< server_socket > ptr;

    static ptr create( int p )
    {
      shared_ptr< server_socket > ss( new server_socket( p ) );
      ss->init();
      return ss;
    }

    void go( int max_opened )
    {
      if ( fd < 0 )
      {
        return;
      }

      int opened_sockets;
      for ( opened_sockets = 0; opened_sockets < max_opened;  )
      {
        int sw = wait_for_connection( fd );

        if ( sw <= 0 )
        {
          cout << "fiber_server: listen() error" << endl;
          ::close( fd );
          return;
        }
        else
        {
          create_listener( sw );
          opened_sockets++;
        }
      }

      wait_for_end we;
      for_each( threads.begin(), threads.end(), we );

      cout << "fiber_server::go(): ok" << endl;
      ::close( fd );;
      std::cout << "Server: exiting. " << std::endl;
    }

  private:
    server_socket( int p )
      : fd( 0 )
      , port( p )
    {
    }

    int wait_for_connection( int sa )
    {
      ::sockaddr_in sd;
      ::socklen_t sl = sizeof( ::sockaddr_in );
      return ::accept( sa, ( ::sockaddr* )&sd, &sl );
    }

    void create_listener( int listen_descriptor )
    {
      shared_ptr< f_listener > l( new f_listener( listen_descriptor ) );
      shared_ptr< ::pthread_t > tp( new ::pthread_t() );
      pthread_create( tp.get()
                    , 0
                    , reinterpret_cast< void* (*)(void*) >( &unified_starter )
                    , static_cast< void* >( l.get() ) );
      threads.push_back( tp );
      listeners.push_back( l );
    }

    void init()
    {
      // create socket:
      ::protoent* pe( ::getprotobyname( "tcp" ) );
      fd = ::socket( AF_INET, SOCK_STREAM, pe->p_proto );

      // bind:
      ::sockaddr_in sar;
      sar.sin_family = AF_INET;
      sar.sin_addr.s_addr = INADDR_ANY;
      sar.sin_port = htons( port );
      if ( ::bind( fd, reinterpret_cast< sockaddr* >( &sar ), sizeof( ::sockaddr_in ) ) != 0 )
      {
        cout << "fiber_server: bind() error" << endl;
        ::close( fd );
        return;
      }

      // listen:
      if ( listen( fd, 10 ) != 0 )
      {
        cout << "fiber_server: listen() error" << endl;
        ::close( fd );
        return;
      }
    }

    int fd;
    int port;
    list< shared_ptr< ::pthread_t > > threads;
    list< shared_ptr< f_listener > > listeners;
};

int main(int,char**)
{
  ::signal( SIGPIPE, SIG_IGN );
  server_socket::ptr p = server_socket::create( 8100 );
  p->go( 100 );
  return 0;
}
