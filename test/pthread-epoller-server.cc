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
#include <signal.h>

#include <list>
using namespace std;

#include <tr1/memory>
using namespace std::tr1;

const int default_port = 8100;

void s_err( int num, string& s );

class f_listener
{
  public:

    typedef shared_ptr< f_listener > ptr;
    f_listener( int fd_ )
    : fd ( fd_ )
    {}

    virtual void go()
    {
      const int init_message_size = 12;

      ssize_t read_bytes =  init_message_size ;

      ::linger l;
      l.l_linger = 0;
      l.l_onoff = 1;
      ::setsockopt( fd, SOL_SOCKET, SO_LINGER, &l, sizeof(::linger) );

      char tbuf[12];
      if ( ::read( fd, tbuf, read_bytes ) != read_bytes )
      {
        cout << "Server listener: end( prematurely )." << endl;
        ::close( fd );
        return;
      }

      string message_header( tbuf );

      std::stringstream sstr;
      try
      {
      sstr << message_header.substr( 6, init_message_size );
      }
      catch ( ... )
      {
        cout << "Server listener: end( prematurely )." << endl;
        ::close( fd );
        return;
      }
      int bytes = 0;
      sstr >> bytes;

      char sndbuf[1];
      char recbuf[1];
      sndbuf[1] = 42;

      for ( int i = 0; i < bytes; i++ )
      {
        int tmpcnt = 0;
        do { tmpcnt = ::read( fd, sndbuf, 1 ); if ( tmpcnt == 0 ) break; } while ( tmpcnt != 1 ) ;
        if ( tmpcnt == 0 ) { cout << "server: wtf(read)\n" ; break; }

        tmpcnt = 0;
        do { tmpcnt = ::write( fd, recbuf, 1 ); if ( tmpcnt == 0 ) break; } while ( tmpcnt != 1 ) ;
        if ( tmpcnt == 0 ) { cout << "server: wtf(write)\n" ; break; }

        if ( sndbuf[0] != recbuf[0] )
        {
          std::cout << "Server listener: Client response incorrect." << std::endl;
          break;
        }
      }
      //cout << "Server listener: end." << endl;
      ::close( fd );
    }

  private:
    int fd;
};

void* listener_starter( void* vp )
{
  f_listener* lp = reinterpret_cast< f_listener* >( vp );
  lp->go();
  return 0;
}

class f_client
{
  public:
    typedef shared_ptr< f_client > ptr;
    f_client( char* address_c_str )
      : _addr( address_c_str )
    {}

    virtual void go()
    {

      int max_opened = 1000;
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
          std::cout << "poller_client: accept() error: " << error_name << std::endl;
          //return;
        }
        else
        {
          create_listener( sw );
          opened_sockets++;
          cout << opened_sockets << " "; cout.flush();
        }
      //std::cout << "."; cout.flush();
      }

      ::close( sa );
      //std::cout << "Server: exiting. " << std::endl;
    }

  private:

    void create_listener( int listen_descriptor )
    {
      f_listener::ptr l( new f_listener( listen_descriptor ) );
      shared_ptr< ::pthread_t > pp( new ::pthread_t );
      ::pthread_create( pp.get()
                      , 0
                      , &listener_starter
                      , l.get() );
      pps.push_back( pp );
      listeners.push_back( l );
      //cout << "server: client created" << endl;
    }

    int init_socket()
    {
      ::protoent *pe = getprotobyname( "tcp" );

      sockaddr_in sar;
      sar.sin_family = AF_INET;
      sar.sin_addr.s_addr = INADDR_ANY;
      sar.sin_port = htons( default_port );

      int sa = socket( AF_INET, SOCK_STREAM, pe->p_proto );

      if ( bind( sa, (sockaddr*)&sar, sizeof(sockaddr_in) ) != 0 )
      {
        string error_name;
        s_err( errno, error_name );
        std::cout << "fiber_server: bind() error: " << error_name << std::endl;
        return -1;
      }

      if ( listen( sa, 1004 ) != 0 )
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
      sockaddr_in sadr;
      socklen_t sadrlen = sizeof( sockaddr_in );
      return ::accept( sa, (::sockaddr*)&sadr, &sadrlen );
    }

  private:
    char* _addr;
    std::list< shared_ptr< ::pthread_t > > pps;
    std::list< f_listener::ptr > listeners;
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

static void myaction( int action )
{
  cout << "got " << action << endl;
  ::exit(-1);
}

int main(int argc ,char* argv[])
{
  signal( SIGPIPE, SIG_IGN );
  signal( SIGINT, &myaction );
  char loopback[] = "127.0.0.1";

  f_client::ptr fcl( new f_client( ( argc == 2 ) ? argv[1] : loopback ) );
  fcl->go();
  cout << "Main process: exiting." << endl;

  return 0;
}

