#include "simple_server.h"
#include "jzmn.h"
#include <memory>
int main(int argc,char *argv[])
{
  std::shared_ptr<simple_server::http_server> httpserver(new simple_server::http_server(9000,"HOLA"));
  httpserver->create_socket();

  httpserver->add_route_handler("/",[](simple_server::Request *)->simple_server::Response *{
      simple_server::Response* response = new simple_server::Response();
      response->body="HOLA 1";
      response->headers["Content-type"]="text/plain";
      response->headers["Content-Length"]="4";
      response->status_code = simple_server::http_response_code::OK;
      return response;
  });

  httpserver->add_route_handler("^\/.*\.html$",[](simple_server::Request *req)->simple_server::Response *{
      simple_server::Response* response = new simple_server::Response();
      response->body= simple_server::read_whole_file(get_current_dir_name()+std::string("/")+"index.html");
       ///"HOLA CSS " + req->url + " Working dir "+ simple_server::get_working_directory();
      response->headers["Content-type"]= simple_server::http_content_type_to_string(simple_server::content_type::text_html);
      response->headers["Content-Length"]= std::to_string(response->body.size());
      response->status_code = simple_server::http_response_code::OK;
      return response;
  })
  ->add_route_handler("^\/.*\.css$",[](simple_server::Request *req)->simple_server::Response *{
      std::cout <<"Requesting a CSS File?";
      simple_server::Response* response = new simple_server::Response();
      response->body= simple_server::read_whole_file(get_current_dir_name()+std::string("/")+"style.css");
     
     
       ///"HOLA CSS " + req->url + " Working dir "+ simple_server::get_working_directory();
      response->headers["Content-type"]= simple_server::http_content_type_to_string(simple_server::content_type::text_css);
      response->headers["Content-Length"]= std::to_string(response->body.size());
      response->status_code = simple_server::http_response_code::OK;
      return response;
  })
   ->add_route_handler("^\/.*\.xpp$",[](simple_server::Request *req)->simple_server::Response *{
      simple_server::Response* response = new simple_server::Response();
      response->body="HOLA XPP" + req->url;
      response->headers["Content-type"]="text/plain";
      response->headers["Content-Length"]= std::to_string(response->body.size());
      response->status_code = simple_server::http_response_code::OK;
      return response;
  });
    httpserver->add_route_handler("/<int>/hola",[](simple_server::Request *req)->simple_server::Response *{
      simple_server::Response* response = new simple_server::Response();
      response->body="HOLA XPP" + req->url;
      response->headers["Content-type"]="text/plain";
      response->headers["Content-Length"]= std::to_string(response->body.size());
      response->status_code = simple_server::http_response_code::OK;
      return response;
  })
  ->start();
  return 0;
}