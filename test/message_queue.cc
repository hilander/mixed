#include <iostream>
#include <algorithm>
using namespace std;

#include <tr1/memory>
using namespace std::tr1;

#include <message_queue.hh>
using namespace message_queues;

#include <gtest/gtest.h>

TEST( MessageQueue, SendReceive )
{
  const int msize = 10;

  message_queue mq;
  shared_ptr< message > messages[ msize ];
  
  for ( int i = 0; i < msize; i++ )
  {
    messages[ i ].reset( new message() );
    messages[i]->m_type = i+1;
  }

  mq.write_to_master( messages[0] );
  
  shared_ptr< message >  m;
  ASSERT_TRUE( mq.read_for_master( m ) );
  ASSERT_EQ( m->m_type, 1 );
  ASSERT_FALSE( mq.read_for_master( m ) );

  mq.write_to_master( messages[0] );
  mq.read_for_master( m );
  mq.write_to_master( messages[1] );
  ASSERT_TRUE( mq.read_for_master( m ) );
  ASSERT_EQ( m->m_type, 2 );
}

int main( int argc, char* argv[] )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
