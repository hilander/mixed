#include <iostream>
#include <algorithm>
#include <pthread.h>
using namespace std;

#include <tr1/memory>
using namespace std::tr1;

#include <message_queue.hh>
using namespace message_queues;

#include <gtest/gtest.h>

typedef void* thread_t;

struct starter_pack
{
  int count;
  shared_ptr< message_queue > mq;
};

thread_t message_reader( void* d )
{
  starter_pack* pack = reinterpret_cast< starter_pack* >( d );
  int m_counter = pack->count;
  shared_ptr< message > m;

  for ( int i = 0; i < m_counter; i++ )
  {
    m.reset( new message() );
    m->m_type = i+1;
    pack->mq->write_to_master( m );
  }
  return 0;
}

thread_t message_writer( void* d )
{
  starter_pack* pack = reinterpret_cast< starter_pack* >( d );
  int m_counter = pack->count;
  shared_ptr< message > m;

  for ( int i = 0; i < m_counter; i++ )
  {
    while ( !pack->mq->read_for_master( m ) )
    {
    }
    if ( m->m_type != i+1 )
    {
      cout << "Reader: bad value read. in " << i+1 << " attempt" << endl;
      break;
    }
  }
  return 0;
}

TEST( MessageQueue, MultiThreaded )
{
  const int msize = 10;
  shared_ptr< message_queue > mq( new message_queue() );
  starter_pack sp;

  sp.count = msize;
  sp.mq.swap( mq );
  
  pthread_t reader_thread, writer_thread;
  pthread_create( &writer_thread, 0, &message_writer, &sp );
  pthread_create( &reader_thread, 0, &message_reader, &sp );
  pthread_join( writer_thread, 0 );
  pthread_join( reader_thread, 0 );
}

int main( int argc, char* argv[] )
{
  ::testing::InitGoogleTest( &argc, argv );
  return RUN_ALL_TESTS();
}
