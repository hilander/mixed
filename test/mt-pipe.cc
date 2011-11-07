#include <iostream>
using namespace std;

#include <tr1/memory>
using namespace std::tr1;

#include <gtest/gtest.h>

void* sender_fun( void* p )
{
  int fd = *((int*)p);
  for (int i = 0; i < 1000; i++ )
  {
    char buf[8] = ".......";
    ::write( fd, buf, 8 );
  }
  return 0;
}

void* receiver_fun( void* p )
{
  int fd = *((int*)p);
  for (int i = 0; i < 1000; i++ )
  {
    char buf[8];
    ::read( fd, buf, 8 );
  }
  return 0;
}

TEST( UnixPipes, test )
{
  int pfds[2];
  ASSERT_TRUE( ::pipe( pfds ) == 0 );
  // spawn two threads:
  ::pthread_t sender, receiver;
  ::pthread_create( &sender, 0, &sender_fun, &pfds[0] );
  ::pthread_create( &receiver, 0, &receiver_fun, &pfds[1] );
  pthread_join( sender, 0 );
  pthread_join( receiver, 0 );
  ::close( pfds[0] );
  ::close( pfds[1] );
}

int main( int argc, char* argv[] )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
