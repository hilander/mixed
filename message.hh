#ifndef __MESSAGE_HH__
#define __MESSAGE_HH__

#include <tr1/memory>

#include "fiber.hh"

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
    typedef std::tr1::shared_ptr< fiber_message > ptr;

		fiber_message();

		fibers::fiber::ptr sender;

		fibers::fiber::ptr receiver;
	};

	struct service_message : public message
	{
    typedef std::tr1::shared_ptr< service_message > ptr;


		enum available_services
		{
			SPAWN,
			SPAWN_REPLY,
			BROADCAST_MESSAGE,
			FINISH_WORK
		};

		service_message( available_services s );

		available_services service;
	};

	template < service_message::available_services N >
	struct serv_message : public service_message
	{
	};

	template <> struct serv_message< service_message::BROADCAST_MESSAGE > : public service_message
	{
    typedef std::tr1::shared_ptr< serv_message< service_message::BROADCAST_MESSAGE > > ptr;
		std::tr1::shared_ptr< message > fiber_data;
		serv_message< service_message::BROADCAST_MESSAGE >( service_message::available_services s );
	};

	template <> struct serv_message< service_message::SPAWN > : public service_message
	{
		typedef std::tr1::shared_ptr< serv_message< service_message::SPAWN > > ptr;
		std::tr1::shared_ptr< fibers::fiber > fiber_to_spawn;
		serv_message< service_message::SPAWN >( service_message::available_services s );
	};

	template <> struct serv_message< service_message::SPAWN_REPLY > : public service_message
	{
		typedef std::tr1::shared_ptr< serv_message< service_message::SPAWN_REPLY > > ptr;
		serv_message< service_message::SPAWN_REPLY >( service_message::available_services s );
	};
}

#endif
