#include <iostream>
#include <algorithm>
using namespace std;

#include <tr1/memory>
using namespace std::tr1;

#include <message_queue.hh>
using namespace message_queues;

int main(int,char**)
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
  if ( mq.read_for_master( m ) )
  {
    cout << "push+top. Got: " << m->m_type << endl;
  }
  if ( mq.read_for_master( m ) )
  {
    cout << "push+top. error" << endl;
  }
  else
  {
    cout << "push+top+top. ok" << endl;
  }
  mq.write_to_master( messages[0] );
  mq.read_for_master( m );
  mq.write_to_master( messages[1] );
  if ( mq.read_for_master( m ) )
  {
    cout << "push+top. Got: " << m->m_type << endl;
  }
  return 0;
}
