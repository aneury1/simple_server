#pragma once
#include <string>
#include <vector>
#define __Window_Platform__
#ifdef __Window_Platform__
#include <windows.h>
#include <winsock.h>
#pragma comment(lib, "Ws2_32.lib")
#pragma warning(disable : 4996)

struct WinsocketInit {
	WinsocketInit() {
		WSAData d;
		WSAStartup(MAKEWORD(2, 2), &d);
	}
};

static WinsocketInit _window_socket_winsocket_init_jargon_v_0_1;

#endif



enum class ServerStatus {
	Running,
	Stopped
};


struct ServerConfiguration {
	std::string key;
	std::string value;
};

struct ServerMiddleWare {};


class SocketServer {



public:

	typedef int SocketFd;


	SocketServer(short port) : port{ port }, server_socket_descriptor(-1) {

	}
	virtual ~SocketServer()
	{
	}

	virtual SocketServer* Start() = 0;

	virtual SocketServer* Stop() = 0;

	virtual SocketServer* setConfiguration(ServerConfiguration config) = 0;

	virtual SocketServer* setConfiguration(std::vector<ServerConfiguration> list_config) = 0;

	virtual SocketServer* addMiddleWare(ServerMiddleWare* middleware) = 0;

	virtual SocketServer* removeMiddleWare(ServerMiddleWare* middleware) = 0;

	virtual SocketServer* Listen() = 0;

	std::string getName()const {
		return instance_name;
	}

protected:

	std::string instance_name;

	SocketFd server_socket_descriptor;

	ServerStatus server_status;

	short port;


};
