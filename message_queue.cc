#include <tr1/memory>
using std::tr1::shared_ptr;

#include "message.hh"
using namespace message_queues;

#include "message_queue.hh"
using namespace message_queues;

// private_list:
private_list::private_list()
{
  first_to_write = shared_ptr< message >( new message() );
  first_to_write->used = true;
  first_to_read = shared_ptr< message >( first_to_write );
}

private_list::~private_list()
{
}

bool private_list::top( message::ptr& m )
{
  if ( first_to_read->used )
  {
    if ( first_to_read->next.get() != 0 )
    {
      // move cursor, set out value to the position:
      m = first_to_read->next ;
      first_to_read = first_to_read->next;
      first_to_read->prev->next.reset();
      first_to_read->prev.reset();
      first_to_read->used = true;

      // once again, move cursor (if anything exists):
      if ( first_to_read->next != 0 )
      {
        first_to_read = first_to_read->next;
        first_to_read->prev->next.reset();
        first_to_read->prev.reset();
      }
      return true;
    }
    return false;
  }
  else
  {
    m = first_to_read;
    first_to_read->used = true;
    if ( first_to_read->next.get() != 0 )
    {
      first_to_read = first_to_read->next;
      first_to_read->prev->next.reset();
      first_to_read->prev.reset();
    }
    return true;
  }
}

void private_list::push( message::ptr& m )
{
  m->prev = first_to_write;
  first_to_write->next = m;
  first_to_write = m;
}

// message_queue:
message_queue::message_queue()
{
}

message_queue::~message_queue()
{
}

void message_queue::write_to_master( message::ptr& m )
{
  master_queue.push( m );
}

bool message_queue::read_for_master( message::ptr& m )
{
  return master_queue.top( m );
}

void message_queue::write_to_slave( message::ptr& m )
{
  slave_queue.push( m );
}

bool message_queue::read_for_slave( message::ptr& m )
{
  return slave_queue.top( m );
}

