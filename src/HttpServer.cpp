#include "HttpServer.h"
#include "HttpServerMetada.h"
#include "HttpPrimitiveUtils.h"
namespace {
	const int	MAX_CONNECTIONS = 1000;
}

HttpServer HttpServer::registerNewStaticEndpoint(std::string endpoint, ParserEndpoint parsed){
	registered_endpoint[endpoint] = parsed;
	return (*this);
}

HttpServer::HttpServer(int port)
{

#ifdef WIN32
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Failed to initialize WinSock" << std::endl;
		 
	}
#endif

	// Create a socket for incoming connections
	 serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Failed to create server socket" << std::endl;
		 
	}

	// Set socket options to reuse address and enable listening
	int opt = 1;
	setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char*)&opt, sizeof(opt));

	// Set up server address and port
	sockaddr_in serverAddr{};
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(port);

	// Bind the socket to the server address
	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		std::cerr << "Failed to bind server socket" << std::endl;
		 
	}

	// Start listening for incoming connections
	if (listen(serverSocket, MAX_CONNECTIONS) == SOCKET_ERROR) {
		std::cerr << "Failed to listen on server socket" << std::endl;
		return ;
	}

	std::cout << "Server v0.1 started. Listening on port" << port << std::endl;
}

int HttpServer::start()
{
	while (true) {
		// Accept incoming connection
		sockaddr_in clientAddr{};
		int clientAddrLen = sizeof(clientAddr);
		SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr,(socklen_t *)&clientAddrLen);
		if (clientSocket == INVALID_SOCKET) {
			std::cerr << "Failed to accept client connection" << std::endl;
			continue;
		}

		// Create a new client info structure
		ClientInfo* client = new ClientInfo;
		client->sockfd = clientSocket;
		client->clientAddr = clientAddr;

		// Handle the client connection in a separate thread
		std::thread clientThread(handleClient, client);
		clientThread.detach();
	}

	// Close the server socket
	closesocket(serverSocket);
#ifdef WIN32
	// Cleanup WinSock
	WSACleanup();
	#endif
}
