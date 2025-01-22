#ifndef DEFAULT_TCP_HANDLER
#define DEFAULT_TCP_HANDLER
#include "defines.h"
#include "tcp_handler.h"

struct tcp_default_handler : public tcp_handler{
     
    private:
        int server_fd;
        short port;
        bool run=false;
        tcp_connection_handler on_new_connection_handler_cb;
    public:
    tcp_default_handler(short port=9000, tcp_connection_handler cb=nullptr){ 
       this->port = port;
       on_new_connection_handler_cb = cb;
    }
    virtual ~tcp_default_handler(){}
    virtual tcp_handler_status create_server() override;
    virtual tcp_handler_status accept_and_process_message() override;
    void handle_incomming_connection(int fd);
    void set_incomming_connection_handler(tcp_connection_handler cb);
};


#endif ///DEFAULT_TCP_HANDLER