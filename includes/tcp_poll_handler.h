#ifndef TCP_POLL_HANDLER_DEFINED
#define TCP_POLL_HANDLER_DEFINED

#include <poll.h>

#include "defines.h"
#include "tcp_handler.h"




struct tcp_poll_handler : public tcp_handler{
     
    private:
        int server_fd;
        std::vector<pollfd> poll_fds;
        short port;
        bool run=false;
        tcp_connection_handler on_new_connection_handler_cb;
        tcp_connection_event_handler on_notify_event_handler_cb;
        void remove_socket(int fd);
    public:
    tcp_poll_handler(short port=9000, tcp_connection_handler cb=nullptr){ 
       this->port = port;
       on_new_connection_handler_cb = cb;
    }
    virtual ~tcp_poll_handler(){}
    virtual tcp_handler_status create_server() override;
    virtual tcp_handler_status accept_and_process_message() override;
    void handle_incomming_connection(int fd);
    void set_incomming_connection_handler(tcp_connection_handler cb);
};


#endif ///DEFAULT_POLL_HANDLER
