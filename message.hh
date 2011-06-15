#ifndef __MESSAGE_HH__
#define __MESSAGE_HH__

#include <tr1/memory>

namespace message_queues
{
  struct message : std::tr1::enable_shared_from_this< message >
  {
    typedef std::tr1::shared_ptr< message > ptr;
    message();
		virtual ~message();
    
		int value;

    bool used;
    ptr prev;
    ptr next;
  };
}
#endif
