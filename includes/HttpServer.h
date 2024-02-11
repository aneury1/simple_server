#pragma once
#undef WIN32
#include <iostream>
#include <string>
#include <unordered_map>
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#warning "build for nowindows"
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#include "HttpServerMetada.h"
#include "IOUtils.h"



struct HttpServer
{
    SOCKET serverSocket;

    static std::string RootPath;

public:

    void setRootPath(const std::string& path);

    const std::string getRootPath() const;
  
    HttpServer endpoint(const std::string& endpoint,const ParserEndpoint& endp);

    bool validateEnpointRoute(const std::string& path);

    HttpServer(const int port);

    int start();
};
