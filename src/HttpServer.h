#pragma once
#include <map>
#include "SocketServerBase.h"
#include "HttpRequest.h"
#include "HttpResponse.h"
#include "HttpIdentifier.h"
#include "Typedefs.h"


 



class HttpServer : public SocketServer
{

public:

	HttpServer(short port) : SocketServer{ port } {}
		
	virtual ~HttpServer()
	{}

	virtual SocketServer* Start();

	virtual SocketServer* Listen();

	virtual SocketServer* Stop();

	virtual SocketServer* setConfiguration(ServerConfiguration config);

	virtual SocketServer* setConfiguration(std::vector<ServerConfiguration> list_config);

	virtual SocketServer* addMiddleWare(ServerMiddleWare* middleware);

	virtual SocketServer* removeMiddleWare(ServerMiddleWare* middleware);


	HttpServer* Get(std::string path, http_request_handler);

	HttpServer* Post(std::string path, http_request_handler);
	
	HttpServer* Put(std::string path, http_request_handler);
	
	HttpServer* Delete(std::string path, http_request_handler);


protected:

	ConnectionHandlerContainer connection_handler;

};

