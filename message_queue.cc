#include <tr1/memory>
using std::tr1::shared_ptr;

#include "message_queue.hh"

using namespace message_queues;

// message:
message::message()
: used( false )
{
  prev = 0;
  next = 0;
}

// private_list:
private_list::private_list()
{
  first_to_write = shared_ptr< message >( new message() );
  first_to_write->used = true;
  first_to_read = first_to_write;
}

private_list::~private_list()
{
}

bool private_list::top( message::ptr m )
{
  if ( first_to_read->used )
  {
    if ( first_to_read->next != 0 )
    {
      // move cursor, set out value to the position:
      m.reset( first_to_read->next );
      shared_ptr< message > sp( first_to_read->next );
      first_to_read.swap( sp );
      first_to_read->prev->next = 0;
      first_to_read->prev = 0;
      first_to_read->used = true;
      // once again, move cursor (if anything exists):
      if ( first_to_read->next != 0 )
      {
        shared_ptr< message > ssp( first_to_read->next );
        first_to_read.swap( ssp );
        first_to_read->prev->next = 0;
        first_to_read->prev = 0;
      }
      return true;
    }
    return false;
  }
  else
  {
    m = first_to_read;
    first_to_read->used = true;
    if ( first_to_read->next != 0 )
    {
      shared_ptr< message > sp( first_to_read->next );
      first_to_read.swap( sp );
      first_to_read->prev->next = 0;
      first_to_read->prev = 0;
    }
    return true;
  }
}

void private_list::push( message::ptr m )
{
  m->prev = first_to_write.get();
  first_to_write->next = m.get();
  first_to_write.swap( m );
}
