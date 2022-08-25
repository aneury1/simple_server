#pragma once
#include <thread>
#include <map>
#include "HttpIdentifier.h"
#include "HttpServer.h"

typedef void (*ThreadFunction)(int client, const char* query, int len);

class ServerStdThread
{

	std::thread thread;

public:


	ServerStdThread(ThreadFunction function, int client, const char* query, int len) : thread(function, client,query, len) {

	}

	ServerStdThread(ThreadFunction function, int client, const char* query, int len, std::map<HttpIdentifier, http_request_handler> connection_handler)
		: thread(function, client, query, len, connection_handler) {

	}

	~ServerStdThread();

};

