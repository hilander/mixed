#include "message.hh"
using namespace message_queues;

message::message()
: used( false )
{
}

message::~message()
{
}

fiber_message::fiber_message()
{
	m_type = message_type::FiberMessage;
}

service_message::service_message()
{
	m_type = message_type::ServiceMessage;
}
