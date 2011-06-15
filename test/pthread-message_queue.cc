#include <iostream>
#include <list>
#include <algorithm>
#include <pthread.h>
using namespace std;

#include <tr1/memory>
using namespace std::tr1;

#include <message_queue.hh>
using namespace message_queues;

typedef void* thread_t;

struct starter_pack
{
  shared_ptr< ::pthread_mutex_t > mt;
  int count;
  list< shared_ptr< message > > ml;
};

thread_t message_writer( void* d )
{
  starter_pack* pack = reinterpret_cast< starter_pack* >( d );
  int m_counter = pack->count;
  shared_ptr< message > m;

  for ( int i = 0; i < m_counter; i++ )
  {
    m.reset( new message() );
    m->value = i+1;
    ::pthread_mutex_lock( pack->mt.get() );
    pack->ml.push_front( m );
    ::pthread_mutex_unlock( pack->mt.get() );
  }
  return 0;
}

thread_t message_reader( void* d )
{
  starter_pack* pack = reinterpret_cast< starter_pack* >( d );
  int m_counter = pack->count;
  shared_ptr< message > m;

  for ( int i = 0; i < m_counter; i++ )
  {
    list< shared_ptr< message > >::reverse_iterator last_elem;
    do
    {
      ::pthread_mutex_lock( pack->mt.get() );
      last_elem = pack->ml.rbegin();
      ::pthread_mutex_unlock( pack->mt.get() );
    }
    while ( last_elem == pack->ml.rend() )
      ;
    m = *last_elem;
    if ( m->value != i+1 )
    {
      cout << "Reader: bad value read. in " << i+1 << " attempt" << endl;
      break;
    }
    /*
    else
    {
      cout << "Reader: read: " << i+1 << "." << " attempt" << endl;
    }
    */
    ::pthread_mutex_lock( pack->mt.get() );
    pack->ml.pop_back();
    ::pthread_mutex_unlock( pack->mt.get() );
  }
  return 0;
}

int main(int,char**)
{
  const int msize = 10000;
  list< shared_ptr< message > > ml;
  shared_ptr< ::pthread_mutex_t > mt( new ::pthread_mutex_t() );
  ::pthread_mutex_init( mt.get(), 0 );
  starter_pack sp;

  sp.mt.swap( mt );
  sp.count = msize;
  sp.ml =  ml;
  
  pthread_t reader_thread, writer_thread;
  pthread_create( &writer_thread, 0, &message_writer, &sp );
  pthread_create( &reader_thread, 0, &message_reader, &sp );
  pthread_join( writer_thread, 0 );
  pthread_join( reader_thread, 0 );
  return 0;
}
