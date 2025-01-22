#ifndef TCP_EPOLL_HANDLER
#define TCP_EPOLL_HANDLER
#include <poll.h>
#include <sys/epoll.h>
#include "defines.h"
#include "tcp_handler.h"
#define MAX_EVENTS 10000
#define BUFFER_SIZE 1024*4
struct tcp_epoll_handler : public tcp_handler{
     
    private:
        struct epoll_event ev, events[MAX_EVENTS];
        int server_fd;
        int epoll_fd;
        std::vector<pollfd> poll_fds;
        short port;
        bool run=false;
        tcp_connection_handler on_new_connection_handler_cb;
        tcp_connection_event_handler on_notify_event_handler_cb;
        void remove_socket(int fd);
    public:
    tcp_epoll_handler(short port=9000, tcp_connection_handler cb=nullptr){ 
       this->port = port;
       on_new_connection_handler_cb = cb;
    }
    virtual ~tcp_epoll_handler(){}
    virtual tcp_handler_status create_server() override;
    virtual tcp_handler_status accept_and_process_message() override;
    void handle_incomming_connection(int fd);
    void set_incomming_connection_handler(tcp_connection_handler cb);
};



#endif