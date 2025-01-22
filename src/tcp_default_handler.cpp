#include "tcp_default_handler.h"
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
#endif
#include <thread>


tcp_handler_status tcp_default_handler::create_server()
{
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
   return tcp_handler_status::Ok;
}
 
tcp_handler_status tcp_default_handler::accept_and_process_message(){
    while(run){
      sockaddr_in address;
      socklen_t address_size = sizeof(address); 
      
      
      int connection_client = accept(server_fd, (sockaddr*)&address, &address_size);
      char *ip = inet_ntoa(address.sin_addr);
    
      SM_WARN("connection from: result=>%s", ip);
      auto fn_call=std::bind(&tcp_default_handler::handle_incomming_connection, this, connection_client);
      std::thread th(fn_call);
      th.detach();
    }
    return tcp_handler_status::Ok;
}

void tcp_default_handler::handle_incomming_connection(int fd){
    
    if(!on_new_connection_handler_cb){
       SM_ERROR("there is not cb settle for handling incomming connections");
       char buffer[1024];
       int c = recv(fd, buffer, 1024,0);
       SM_WARN("Received payload length: %ld and buffer %\n%s\n", c, buffer);
       send(fd,buffer,c,0);
    }else{
        SM_WARN("calling the address fn=>%x", on_new_connection_handler_cb);
        on_new_connection_handler_cb(fd);
    }
}

void tcp_default_handler::set_incomming_connection_handler(tcp_connection_handler cb){
   this->on_new_connection_handler_cb  =cb;
}