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

  private_list pl;
  shared_ptr< message > messages[ msize ];
  
  for ( int i = 0; i < msize; i++ )
  {
    messages[ i ].reset( new message() );
    messages[i]->value = i+1;
  }

  pl.push( messages[0] );
  
  shared_ptr< message >  m;
  if ( pl.top( m ) )
  {
    cout << "push+top. Got: " << m->value << endl;
  }
  if ( pl.top( m ) )
  {
    cout << "push+top. error" << endl;
  }
  else
  {
    cout << "push+top+top. ok" << endl;
  }
  pl.push( messages[0] );
  pl.top( m );
  pl.push( messages[1] );
  if ( pl.top( m ) )
  {
    cout << "push+top. Got: " << m->value << endl;
  }
  return 0;
}
