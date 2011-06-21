#ifndef __MESSAGE_HH__
#define __MESSAGE_HH__

#include <tr1/memory>

namespace message_queues
{
	struct message_type
	{
		static const int FiberMessage = 10;
 		static const int ServiceMessage = 20;
	};

  struct message : std::tr1::enable_shared_from_this< message >
  {
    typedef std::tr1::shared_ptr< message > ptr;
    message();
		virtual ~message();
    
		int m_type;

    bool used;
    ptr prev;
    ptr next;
  };

	struct fiber_message : public message
	{
		fiber_message();
	};

	struct service_message : public message
	{
		service_message();
	};
}
#endif
