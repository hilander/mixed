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

const int32_t default_port = 8100;

string s_err( int32_t num );

class f_client : public fiber
{
  public:
    f_client( char* address_c_str, int32_t port )
      : _addr( address_c_str )
      , _port( port )
    {}

    virtual void go()
    {
      int32_t sa;

      for ( int32_t tries = 0; tries < 10; tries++ )
      {
        sa = init_socket();

        if ( sa < 0 )
        {
          //return;
        }
        else
        {
          break;
          //cout << "client: socket ready" << endl;
        }
      }
      
      int32_t n = 1;
      char num[6];
      sprintf( num, "%6d", n );
      string buf( string( "HELLO:" ) + string( num ) );
      rw_buffer.resize( buf.size() );
      copy( buf.begin(), buf.end(), rw_buffer.begin() );
      do_write( sa, buf.size() );

      //cout << "client: send/receive" << endl;
      for ( int32_t i = 0; i < n; i++ )
      {
        do_read( sa, 1 );
        do_write( sa, 1 );
      //cout << "."; cout.flush();
      }
      do_close( sa );
      //cout << "client end" << endl;
    }

  private:

    int32_t init_socket()
    {
      sockaddr_in sar;
      sar.sin_family = AF_INET;
      inet_pton( AF_INET, _addr, &sar.sin_addr );
      sar.sin_port = htons( _port );

      int32_t sa = socket( AF_INET, SOCK_STREAM, 0 );
      int32_t orig_flags = fcntl( sa, F_GETFL );
      fcntl( sa, F_SETFL, orig_flags | O_NONBLOCK );

      do
      {
        do_connect( sa, sar );

        /*
           if ( connect_status != 0 )
           {
           do_close( sa );
           return -1;
           }
           */
      } while ( connect_status != 0 ) ;

      ::linger l;
      l.l_linger = 0;
      l.l_onoff = 1;
      ::setsockopt( sa, SOL_SOCKET, SO_LINGER, &l, sizeof(::linger) );
      return sa;
    }

  private:
    char* _addr;
    int32_t _port;
};

string s_err( int32_t num )
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

    case EEXIST:
      return string ( "EEXIST" );
      break;

    default:
      return string( "unknown" );
      break;
  }
}

int main(int argc ,char* argv[])
{
  using std::stringstream;

  if ( argc != 3 )
  {
    cout << "Error! Improper number of bytes!" << endl;
    return 1;
  }

  signal( SIGPIPE, SIG_IGN );
  master* mp = master::create();
  
  stringstream sstr;
  sstr << argv[2] ;
  int32_t port;
  sstr >> port;

  int32_t fiber_count = 10;
  fiber::ptr fcl[fiber_count];
  for (int32_t i = 0; i < fiber_count; i++ )
  {
    fcl[i].reset( new f_client( argv[1], port ) );
    fcl[i]->init();
    mp->spawn( fcl[i] );
  }
  mp->run();
  cout << "Main process: exiting." << endl;

  return 0;
}
