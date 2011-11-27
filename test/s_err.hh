#include <string>
#include <errno.h>

inline string s_err( int num )
{
  using namespace std;
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
