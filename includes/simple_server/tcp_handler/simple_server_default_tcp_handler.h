#ifndef DEFAULT_TCP_HANDLER
#define DEFAULT_TCP_HANDLER
#include "simple_server_tcp_handlers.h"

namespace simple_server{


class default_tcp_handler : public tcp_handler{
   virtual void create_server()override;
   virtual void accept_new_connection()override;
   virtual void on_connection_accepted(std::shared_ptr<socket_connection>)override;
};

}



#endif