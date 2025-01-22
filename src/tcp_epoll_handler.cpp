#include "tcp_epoll_handler.h"
#include "websocket_handler.h"
#include "http_handler.h"
#include "websocket_handler.h"
#include "logger.h"
#include <iostream>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <errno.h>
#include <string>
 

static int count = 0;
// Set a socket to non-blocking mode
void setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}

// Enable SO_KEEPALIVE
void enableKeepAlive(int sockfd) {
    int optval = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));

#ifdef __linux__
    int keepIdle = 60;
    int keepInterval = 10;
    int keepCount = 5;
   // setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPIDLE, &keepIdle, sizeof(keepIdle));
  //  setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPINTVL, &keepInterval, sizeof(keepInterval));
   // setsockopt(sockfd, IPPROTO_TCP, TCP_KEEPCNT, &keepCount, sizeof(keepCount));
#endif
}


tcp_handler_status tcp_epoll_handler::create_server(){
struct sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    char buffer[BUFFER_SIZE] = {0};

    // Create server socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("Socket creation failed");
        return  tcp_handler_status::Error;
    }

    setNonBlocking(server_fd);
    enableKeepAlive(server_fd);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(port);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Bind failed");
        close(server_fd);
                return  tcp_handler_status::Error;

    }

    if (listen(server_fd, SOMAXCONN) < 0) {
        perror("Listen failed");
        close(server_fd);
        return  tcp_handler_status::Error;

    }

    std::cout << "Server listening on port " << port << std::endl;

    // Create epoll instance
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1 failed");
        close(server_fd);
        return  tcp_handler_status::Error;

    }

    struct epoll_event ev, events[MAX_EVENTS];
    ev.events = EPOLLIN;
    ev.data.fd = server_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) < 0) {
        perror("epoll_ctl failed");
        close(server_fd);
        close(epoll_fd);
        return  tcp_handler_status::Error;
    }
    return tcp_handler_status::Ok;
}

tcp_handler_status tcp_epoll_handler::accept_and_process_message(){

 while (true) {
        char buffer[BUFFER_SIZE+1];
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            perror("epoll_wait failed");
            break;
        }

        for (int i = 0; i < nfds; i++) {
 
         


            if (events[i].data.fd == server_fd) {
                // Accept new connection
                socklen_t addr_len;
                sockaddr_in address;
                int client_fd = accept(server_fd, (struct sockaddr*)&address, &addr_len);
                if (client_fd < 0) {
                    if (errno != EWOULDBLOCK && errno != EAGAIN) {
                        perror("accept failed");
                    }
                    continue;
                }

                setNonBlocking(client_fd);
                enableKeepAlive(client_fd);

                ev.events = EPOLLIN | EPOLLET; // Edge-triggered mode
                ev.data.fd = client_fd;

                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
                    perror("epoll_ctl ADD client_fd failed");
                    close(client_fd);
                }
                 std::cout << "New client connected: " << client_fd << std::endl;

            } else {
                // Handle client communication
              
                bool pollin_event =  ev.events & POLLIN;
                bool pollhub_event =  ev.events & POLLHUP;
                bool pollerror_event =  ev.events & POLLERR;
                bool pollout_event =  ev.events & POLLOUT;
       
                SM_TRACE("POLLIN %d", pollin_event);
                SM_TRACE("POLl HUb %d", pollhub_event);
                SM_TRACE("POLL error %d", pollerror_event);
                SM_TRACE("POLL out %d", pollout_event);


                memset(buffer, 0, BUFFER_SIZE);
                ///ssize_t bytes_read = read(events[i].data.fd, buffer, BUFFER_SIZE);
                ssize_t bytes_read = recv(events[i].data.fd, buffer, BUFFER_SIZE, 0);

                if (bytes_read > 0) {

                    std::cout << "Received: " << std::string(buffer, bytes_read) << std::endl;
                    std::string str;
                    const char* HTTP_RESPONSE = 
                    "HTTP/1.1 200 OK\r\n"
                    "Content-Type: text/plain\r\n"
                    "Content-Length: 13\r\n"
                    "Connection: close\r\n"
                    "\r\n"
                    "Hello, World!";
                    std::string body_of_response;
                    body_of_response +="Hola eres el numero==>" + std::to_string(++count);
                     std::string response;
                     response += "HTTP/1.1 200 OK\r\n";
                     response += "Content-Type: text/plain\r\n";
                     response += "Content-Length: "+ std::to_string(body_of_response.size())+"\r\n";     
                     response += "Connection: close\r\n\r\n";
                     response += body_of_response;
                                        
                                        // Send HTTP Response
                    send(events[i].data.fd, response.c_str(), response.size(), 0);
                    //send(events[i].data.fd, HTTP_RESPONSE, strlen(HTTP_RESPONSE), 0);
                    std::cout << "Response sent to client: " << events[i].data.fd << std::endl;
                    close(events[i].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
        
                    //send(events[i].data.fd, buffer, bytes_read, 0);
                } else if (bytes_read == 0) {
                    std::cout << "Client disconnected: " << events[i].data.fd << std::endl;
                    close(events[i].data.fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                } else {
                    if (errno != EWOULDBLOCK && errno != EAGAIN) {
                        perror("read failed");
                        close(events[i].data.fd);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
                    }
                }
            }
        }
    }

    return tcp_handler_status::Ok;
}
 void tcp_epoll_handler::handle_incomming_connection(int fd){
    if(on_new_connection_handler_cb){
       on_new_connection_handler_cb(fd);
    }
}
void tcp_epoll_handler::set_incomming_connection_handler(tcp_connection_handler cb){
    this->on_new_connection_handler_cb = cb;
}