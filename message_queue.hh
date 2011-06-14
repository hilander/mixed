#ifndef __MESSAGE_QUEUE_HH__
#define __MESSAGE_QUEUE_HH__

#include <tr1/memory>

namespace message_queues
{
  struct message : std::tr1::enable_shared_from_this< message >
  {
    typedef std::tr1::shared_ptr< message > ptr;
    message();
    
    bool used;
    message* prev;
    message* next;
  };

  class private_list
  {
    public:
      private_list();
      virtual ~private_list();

      bool top( message::ptr m );
      void push( message::ptr m );

    private:
      message::ptr first_to_write;
      message::ptr first_to_read;
  };

  class message_queue : public std::tr1::enable_shared_from_this< message_queue >
  {
    public:
    typedef std::tr1::shared_ptr< message_queue > ptr;

    public:
      message_queue();
      virtual ~message_queue();

      void write_to_master( message& m );
      bool read_for_master( message& m );
      void write_to_slave( message& m );
      bool read_for_slave( message& m );

    private:
      private_list master_queue;
      private_list slave_queue;
  };
}

#endif
