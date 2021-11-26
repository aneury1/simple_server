#include <thread>
#include <iostream>

#include "Server.h"
#include "JSON.h"
#include "HttpRequest.h"
#include "HTTPPath.h"
#include "Utils.h"

void home(int client, HTTPRequest* response){

    HTTPRequest* query_request = response;

    std::string body_response = createHttpResponse(200, "OK", WellKnowContentType::Json, readFile("request_endpoint.txt"));

    int rc = send(client, body_response.c_str(), body_response.size(), 0);
}

void home2(int client, HTTPRequest* response) {

    HTTPRequest* query_request = response;

    std::string response_buffer;

    response_buffer = createHttpResponse(200, "OK", WellKnowContentType::Json, readFile("invalid_endpoint.txt"));

    int rc = send(client, response_buffer.c_str(), response_buffer.size(), 0);
}




int main(int argc, char *argv[])
{
    Server theServer1(9025, false  , "nossl-server 1");
  
    theServer1.get("/", home);
    theServer1.get("/test", home2);
  
    auto tls_server =[&](){
         theServer1.launch();
   };

  

   std::thread t1(tls_server);
   t1.join();
   return 0;
}