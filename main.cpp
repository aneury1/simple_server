#include "Server.h"
#include <thread>
#include "JSON.h"
#include "user_model.h"
#include "SqliteConnectionHandler.h"
#include "HttpRequest.h"
#include "HTTPPath.h"
#include <iostream>


///@POST 
void login(int client_socket, HTTPRequest *response)
{
     std::cout <<"POST OPERATION\n";
     if(response->body.length()>0)
     {
 
     }   
     return ;
}








int main(int argc, char *argv[])
{
    Server theServer1(9025, false  , "nossl-server 1");
    Server theServer2(9026, false   , "ssl-server");
    Server theServer3(9027, false  , "nossl-server 2");
    Server theServer4(9028, false  , "nossl-server 3");

    theServer1.post("/login", login);


    auto tls_server =[&](){
         theServer1.launch();
   };
    auto tls_server2=[&](){
       theServer2.launch();
   };

    auto tls_server3 =[&](){
        theServer3.launch();
   };
    auto tls_server4=[&](){
       theServer4.launch();
    };
    std::thread t1(tls_server), t2(tls_server2), t3(tls_server3), t4(tls_server4);


    t1.join();
    t2.join();
    t3.join();
    t4.join();
 
   return 0;
}