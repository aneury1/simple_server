#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#warning "build for nowindows"
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#endif


#include <thread>
#include <vector>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <sstream>

#include "HttpServerMetada.h"
#include "IOUtils.h"

struct HttpServer
{
	SOCKET serverSocket;
public:

   
   HttpServer registerNewStaticEndpoint(std::string endpoint, ParserEndpoint);

	HttpServer(int port);
	int start();

};

