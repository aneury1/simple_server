#include <thread>
#include <iostream>

#include "Server.h"
#include "JSON.h"
#include "HttpRequest.h"
#include "HTTPPath.h"


int main(int argc, char *argv[])
{
    Server theServer1(9025, false  , "nossl-server 1");
    auto tls_server =[&](){
         theServer1.launch();
   };
   std::thread t1(tls_server);
   t1.join();
   return 0;
}