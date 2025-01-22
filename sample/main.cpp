#include "variant.h"
#include "tcp_handler.h"
#include "tcp_default_handler.h"
#include "tcp_poll_handler.h"
#include "tcp_epoll_handler.h"
#include "http_handler.h"
#include "logger.h"
#include <iostream>
#include <string>

void socket_handler(int fd){

      http_server::get()
      ->handle_connection_request(fd);
    };


int assest_sample_main(int argc, char *argv[])
{
     int port = 9001;
     if(argc>1){
        port = atoi(argv[1]);
     }
       http_server::get()
      ->add_route_handler("/.*",[](http_request req)->http_response{
        http_response response;
        response.set_home_path("/home/neon/Documents/projects/simple_server/sample/assets/");
        if(req.url.find(".html")!=std::string::npos)
           response.set_content_type(http_content_type::text_html);
        else if(req.url.find(".css")!=std::string::npos)
           response.set_content_type(http_content_type::text_css);
        else if(req.url.find(".js")!=std::string::npos)
           response.set_content_type(http_content_type::application_javascript);
        else
        {
          extern std::string  default_500_msg();
          
          response.set_body(default_500_msg());
          return response;
        }
        SM_INFO("URL: %s", req.url);
        response.render_text_file("/home/neon/Documents/projects/simple_server/sample/assets"+req.url);
        return response;
      })->add_route_handler("/",[](http_request req)->http_response{
        http_response response;
        response.render_text_file("/home/neon/Documents/projects/simple_server/sample/assets/index.html");
        return response;
      });

    tcp_epoll_handler handler(port, socket_handler);
    handler.create_server();
    SM_TRACE("Server create at port %ld", port);
    handler.accept_and_process_message();
    return 0;
}