#include <iostream>
#include <algorithm>
using namespace std;

#include <tr1/memory>
using namespace std::tr1;

#include <message_queue.hh>
using namespace message_queues;

#include <gtest/gtest.h>

TEST( PrivateList, PushAndTop )
{
  const int msize = 10;

  private_list pl;
  shared_ptr< message > messages[ msize ];
  
  for ( int i = 0; i < msize; i++ )
  {
    messages[ i ].reset( new message() );
    messages[i]->m_type = i+1;
  }

  pl.push( messages[0] );
  
  shared_ptr< message >  m;
  EXPECT_TRUE( pl.top( m ) );
  EXPECT_FALSE( pl.top( m ) );

  pl.push( messages[0] );
  pl.top( m );
  pl.push( messages[1] );
  EXPECT_TRUE( pl.top( m ) );
}

int main( int argc, char* argv[] )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
