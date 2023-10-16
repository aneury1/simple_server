#pragma once

#include <iostream>
#include <string>
#include <unordered_map>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <thread>
#include <vector>
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <regex>
#include <sstream>

#include "HttpServerMetada.h"


struct HttpServer
{
	SOCKET serverSocket;
public:

	HttpServer(int port);
	int start();

};

