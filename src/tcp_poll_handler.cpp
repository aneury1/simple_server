#include "tcp_poll_handler.h"
#include "tcp_default_handler.h"
#include "websocket_handler.h"
#include "logger.h"
#include <regex>
#include <thread>
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#define closesocket close
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <netinet/in.h>
#include <poll.h>
#include <fcntl.h>
#endif
#include <thread>

void set_non_blocking(int sock) {
  int flags = fcntl(sock, F_GETFL, 0);
  fcntl(sock, F_SETFL, flags | O_NONBLOCK);
}


tcp_handler_status tcp_poll_handler::create_server(){
     server_fd = socket(AF_INET, SOCK_STREAM, 0);
   
   sockaddr_in address;
   address.sin_addr.s_addr = INADDR_ANY;
   address.sin_port = htons(port);
   address.sin_family = AF_INET;

   int test = 0;
   test = ::bind(server_fd, (sockaddr *)&address, sizeof(address));
   SM_WARN("bind result=>%ld", test);
   test = listen(server_fd, SO_MAXCON);
   SM_WARN("listen result=>%ld", test);

   int opt =1;
   test = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
   SM_WARN("setsockopt SO_REUSEADDR result=>%ld", test);

   test = setsockopt(server_fd, SOL_SOCKET, SO_REUSEPORT, (char*)&opt, sizeof(opt));
   SM_WARN("setsockopt SO_REUSEPORT result=>%ld", test);
   run = true;

   set_non_blocking(server_fd);
   pollfd event_poll_fd = {server_fd, POLLIN, 0};
   poll_fds.push_back(event_poll_fd);
   return tcp_handler_status::Ok;
}
tcp_handler_status tcp_poll_handler::accept_and_process_message()
{
    while(run){
      int activity = poll(poll_fds.data(), poll_fds.size(), 4000); // Infinite timeout
      if (activity < 0) 
      {
        SM_ERROR("poll error");
        break;
      }
      else
      {
        
        for (size_t i = 0; i < poll_fds.size(); i++)
        {
            ////SM_INFO("New activity EVENTS=%ld", poll_fds[i].revents);
            if(poll_fds[i].revents & POLLHUP){
              SM_ERROR("SOCKET CLOSE");
              remove_socket(poll_fds[i].fd); 
            }
            else if(poll_fds[i].revents & POLLOUT){
              SM_ERROR("SOCKET OUTPUT");
            }
            else if(poll_fds[i].revents & POLLIN)
            {
                SM_ERROR("SOCKET input data to be read");
                if(poll_fds[i].fd == server_fd)
                {
                   int new_connection = accept(server_fd, nullptr, nullptr);
                   SM_WARN("new connection fd=>%ld", new_connection);
                   set_non_blocking(new_connection);
                   pollfd event_poll_fd = {new_connection, POLLIN, 0};
                   poll_fds.push_back(event_poll_fd);
                   handle_incomming_connection(new_connection);
                }
                else
                {
                    char buffer[2048];
                    int client_fd = poll_fds[i].fd;
                    int valread = recv(client_fd, buffer, 2048, MSG_PEEK);
                    if (valread <= 0) {
                      SM_INFO("Client disconnected fd: %ld, serverfd %ld",client_fd, server_fd );
                      if (fcntl(client_fd, F_GETFL) == -1) {
                          perror("Socket is invalid");
                          remove_socket(client_fd); 
                      } else {
                          printf("Socket is valid.\n");
                          closesocket(client_fd);
                          remove_socket(client_fd); 
                      }
                    } else {
                     
    

                      if (errno == EWOULDBLOCK || errno == EAGAIN) {
                          SM_INFO("No data to read yet, retrying...");// << std::endl;
                      } else {
                          perror("recv failed");
                          //close(fds[1].fd);
                          //fds[1].fd = -1;
                      }

                        websocket_manager::get()->notify_message_request(client_fd);
                        ///buffer[valread] = '\0';
                        /// SM_INFO("websocket client_fd: %ld, server_fd: %ld",client_fd, server_fd);
                    }
                }
            }

        }
      }
    }
  return tcp_handler_status::Ok;
}
void tcp_poll_handler::handle_incomming_connection(int fd){
    if(on_new_connection_handler_cb){
       on_new_connection_handler_cb(fd);
    }
}
void tcp_poll_handler::set_incomming_connection_handler(tcp_connection_handler cb){
    this->on_new_connection_handler_cb = cb;
}

void tcp_poll_handler::remove_socket(int fd)
 {
    for (auto it = poll_fds.begin(); it != poll_fds.end(); ++it) {
        if (it->fd == fd) {
            close(fd);
            poll_fds.erase(it);
            SM_WARN("[-] Socket removed: %ld",fd);
            break;
        }
    }
}
