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

string s_err( int num );

class f_client : public fiber
{
  public:
    f_client( char* address_c_str, int port )
      : _addr( address_c_str )
      , _port( port )
    {}

    virtual void go()
    {
      int sa;

      for ( int tries = 0; tries < 10; tries++ )
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
      
      int n = 10;
      char num[6];
      sprintf( num, "%6d", n );
      string buf( string( "HELLO:" ) + string( num ) );
      rw_buffer->resize( buf.size() );
      copy( buf.begin(), buf.end(), rw_buffer->begin() );
      do_write( sa, buf.size() );

      //cout << "client: send/receive" << endl;
      for ( int i = 0; i < n; i++ )
      {
        do_read( sa, 1 );
        do_write( sa, 1 );
      //cout << "."; cout.flush();
      }
      ::close( sa );
      //cout << "client end" << endl;
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

      int sw = ::connect( sa, (const sockaddr*)&sar, sizeof(sar) );

      if ( sw == 0 )
      {
        ::linger l;
        l.l_linger = 0;
        l.l_onoff = 1;
        ::setsockopt( sa, SOL_SOCKET, SO_LINGER, &l, sizeof(::linger) );

        return sa;
      }
      
      do_connect( sa );

      if ( connect_status != 0 )
      {
        ::close( sa );
        return -1;
      }

      ::linger l;
      l.l_linger = 0;
      l.l_onoff = 1;
      ::setsockopt( sa, SOL_SOCKET, SO_LINGER, &l, sizeof(::linger) );
      return sa;
    }

  private:
    char* _addr;
    int _port;
};

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
  int port;
  sstr >> port;

  int fiber_count = 1000;
  fiber::ptr fcl[fiber_count];
  for (int i = 0; i < fiber_count; i++ )
  {
    fcl[i].reset( new f_client( argv[1], port ) );
    fcl[i]->init();
    mp->spawn( fcl[i] );
  }
  mp->run();
  cout << "Main process: exiting." << endl;

  return 0;
}
