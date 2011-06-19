#include <tr1/memory>
using namespace std::tr1;

#include <vector>
using namespace std;

#include "message.hh"
using namespace message_queues;

#include "worker.hh"
using namespace workers;

worker::worker()
: master_allowed( false )
{
}

worker::~worker()
{
}

void worker::run()
{
  while ( !finished() )
  {
    iteration();
  }
}

void worker::iteration()
{
  sched.run();
  do_epolls();
  process_messages();
}

void worker::do_epolls()
{
  int how_many = 0;
  vector< shared_ptr< ::epoll_event > > affected_fds = io_facility.do_epolls( how_many );
  if ( how_many > 0 )
  {
    // do some reads, writes, etc.
  }
}

void worker::process_messages()
{
  vector< shared_ptr< message > > m;
}

bool worker::finished()
{
  return true;
}

