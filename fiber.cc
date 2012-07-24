#include <arpa/inet.h>

#include <tr1/memory>
using namespace std::tr1;

#include <vector>
using namespace std;

#include "fiber.hh"
using namespace fibers;

#include "message.hh"
using namespace message_queues;

#include "worker.hh"
using namespace workers;

#include <unistd.h>

fiber::fiber()
: state( READY )
{
}

fiber::~fiber()
{
}

void fiber::start()
{
  go();
  state = FINISHED;
  yield();
}

ssize_t fiber::do_read( int32_t f, ssize_t s )
{
  last_read = -1;
  rw_size = s;
  owner->block_on_io( f, shared_from_this(), BLOCKED_FOR_READ );
  yield();
  return last_read;
}

ssize_t fiber::do_write( int32_t f, ssize_t s )
{
  rw_size = s;
  last_read = -1;
  owner->block_on_io( f, shared_from_this(), BLOCKED_FOR_WRITE );
  yield();
  return last_write;
}

int32_t fiber::do_accept( int32_t f )
{
  owner->block_on_io( f, shared_from_this(), BLOCKED_FOR_ACCEPT );
  yield();
  return last_accepted_fd;
}

void fiber::do_connect( int32_t f, ::sockaddr_in& s )
{
  owner->insert_fd( f );

  int32_t sw = ::connect( f, (const sockaddr*)&s, sizeof(s) );

  if ( sw == 0 )
  {
    ::linger l;
    l.l_linger = 0;
    l.l_onoff = 1;
    ::setsockopt( f, SOL_SOCKET, SO_LINGER, &l, sizeof(::linger) );

    return;
  }
      
  owner->do_connect( f, shared_from_this(), BLOCKED_FOR_CONNECT );
  yield();
}

void fiber::do_close( int32_t f )
{
  owner->remove_fd( f );
  ::close( f );
}

void fiber::send_message( fiber_message::ptr& m )
{
  owner->send_message( m );
}

void fiber::receive_message( fiber_message::ptr& p )
{
  owner->block_on_message( shared_from_this() );
  yield();
  p = message_buffer.front();
  message_buffer.pop_front();
}

fiber_message::ptr fiber::receive_message_nonblock()
{
  fiber_message::ptr p = message_buffer.front();
  message_buffer.pop_front();
  return p;
}

void fiber::spawn( ptr& f )
{
  owner->spawn( f );
  yield();
}

fiber::current_state fiber::get_state()
{
  return state;
}

void fiber::set_state( fiber::current_state s )
{
  state = s;
}

ssize_t fiber::get_rw_size()
{
  return rw_size;
}

void fiber::set_last_accepted_fd( int32_t f )
{
  last_accepted_fd = f;
}

void fiber::set_connect_status( int32_t status )
{
  connect_status = status;
}

void fiber::set_last_read( ssize_t s )
{
  last_read = s;
}

void fiber::set_last_write( ssize_t s )
{
  last_read = s;
}

void fiber::set_owner( std::tr1::shared_ptr< workers::worker > o )
{
  owner = o;
}

void fiber::put_into_message_buffer( shared_ptr< fiber_message > m )
{
  message_buffer.push_back( m );
}

void fiber::put_into_rw_buffer( char* b, ssize_t s )
{
  copy( &b[0], &b[s], rw_buffer.begin() ); 
}
