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


	~ServerStdThread();

};

