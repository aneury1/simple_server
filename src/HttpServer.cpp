
#include <thread>
#include "ServerStdThread.h"
#include "HttpServer.h"
#include "Utils.h"
#include "Typedefs.h"


namespace {
    SocketServer::SocketFd createSimpleTCPServerListenAny(short port) {
        int server_socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);
        int ret = -1;
        sockaddr_in address;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);
        address.sin_family = AF_INET;

        ret = bind(server_socket_descriptor,
            (sockaddr*)&address,
            sizeof(address)
        );
        if (ret < 0)
        {
            ///\ todo remove this printf and add Logger Formula
            printf("error binding this server");
            
            return -1;
        }

        ret = listen(server_socket_descriptor, 10);

        return server_socket_descriptor;
    }


   


    void getDefaultAcceptorByNow(SocketServer::SocketFd  client, 
                                 ServerStatus& server_status,
                                 ConnectionHandlerContainer connection_handler
        
        
        ) {
       
        
        char buffer[4096*16] = { 0x00 };
       
        int rc = recv(client, buffer, 4096*16, 0);


       
            if (connection_handler.size() <= 0)
                handleHTTPResponse(client, buffer, rc);
            else
                handleHTTPResponseWithMiddleWare(client, buffer, rc, connection_handler);
       
   
    }
}


SocketServer* HttpServer::Start()
{
    this->server_socket_descriptor = createSimpleTCPServerListenAny(this->port);
    
    if (this->server_socket_descriptor != -1)
        this->server_status = ServerStatus::Running;
    else
        this->server_status = ServerStatus::Stopped;


    return this;
}
SocketServer* HttpServer::Listen()
{
    printf("Instance Running on Port => %ld\n", this->port);
    for (auto it : this->connection_handler) 
    {
        HTTP_VERB verb = it.first.verb;
        printf("METHOD: %s => Path[%s]\n", HttpMethodToString(verb).c_str(), it.first.path.c_str());
    }
    while (this->server_status == ServerStatus::Running)
    {
        SocketServer::SocketFd  client = accept(server_socket_descriptor, nullptr, nullptr);
        
        getDefaultAcceptorByNow(server_socket_descriptor,
                                server_status, 
                                connection_handler);
    }
    return this;
}


SocketServer* HttpServer::Stop()
{
    closesocket(this->server_socket_descriptor);
    return this;
}

SocketServer* HttpServer::setConfiguration(ServerConfiguration config)
{
    return this;
}

SocketServer* HttpServer::setConfiguration(std::vector<ServerConfiguration> list_config)
{
    return this;
}

SocketServer* HttpServer::addMiddleWare(ServerMiddleWare* middleware)
{
    
    return this;
}

SocketServer* HttpServer::removeMiddleWare(ServerMiddleWare* middleware)
{
    return this;
}


HttpServer* HttpServer::Get(std::string path, http_request_handler handler) {

    HttpIdentifier identifer(path, HTTP_VERB::HTTP_GET);
    connection_handler[identifer] = handler;
    return this;
}
HttpServer* HttpServer::Post(std::string path, http_request_handler handler) {
    HttpIdentifier identifer(path, HTTP_VERB::HTTP_POST);
    connection_handler[identifer] = handler;
    return this;
}
HttpServer* HttpServer::Put(std::string path, http_request_handler handler) {
    HttpIdentifier identifer(path, HTTP_VERB::HTTP_PUT);
    connection_handler[identifer] = handler;
    return this;
}
HttpServer* HttpServer::Delete(std::string path, http_request_handler handler){
    HttpIdentifier identifer(path, HTTP_VERB::HTTP_DELETE);
    connection_handler[identifer] = handler;
    return this;
}