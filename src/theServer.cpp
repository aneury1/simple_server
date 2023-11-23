#if 0
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

#define SOCKET_ERROR -1
#define INVALID_SOCKET -1
#define closesocket close
typedef int SOCKET;
#endif

#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <string>
#include <thread>
#include <vector>

#pragma comment(lib, "ws2_32.lib")


struct APIInitializer{

    APIInitializer(){
#ifdef WIN32
#ifdef __WINDOWS__
		WSADATA wsaData;
		if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
			std::cerr << "Failed to initialize WinSock" << std::endl;
			return  ;
		}
#endif
#endif

	}
	~APIInitializer(){
#ifdef WIN32
#ifdef __WINDOWS__
		// Close the server socket
		closesocket(serverSocket);

		// Cleanup WinSock
		WSACleanup();
#endif
#endif
	}

};


const int MAX_CONNECTIONS = 10000;

// Define a structure to store client information
struct ClientInfo {
	SOCKET sockfd;
	sockaddr_in clientAddr;
};

struct Request {
	std::string url;
	std::string verb;
	std::unordered_map<std::string, std::string> http_headers;
	std::unordered_map<std::string, std::string> url_parameters;
	ClientInfo* client_socket_file_descriptor;
};

struct Response {
	std::string response_line;
	std::unordered_map<std::string, std::string> http_headers;
	std::string body;

	std::string toString() {
		std::string ret;
		ret += response_line + "\r\n";
		for (auto it : http_headers)
			ret += it.first + ":" + it.second + "\r\n";
		ret += "\r\n";
		return ret;
	}

};

typedef Response* (*ParserEndpoint)(Request* request);

struct ParserEndpointInformation {
	ParserEndpoint handler;
	std::string http_verb;
	std::map<std::string, std::string> header_criteria;
	ParserEndpointInformation() {
		handler = nullptr;
		http_verb = "GET";
	}
};


std::map<std::string, ParserEndpoint> registered_endpoint;
std::map<std::string, ParserEndpointInformation> registered_endpoint_with_criterion;


Response* notFoundDefaultHandler(Request* request) {
	Response* response = nullptr;
	std::cout << "Route not found";
	return nullptr;
}


ParserEndpoint parseRequestRoute(Request& request, ParserEndpoint notFoundHandler) {
	ParserEndpoint ret = nullptr;
	std::string url = request.url;
	bool found = false;
	std::unordered_map<std::string, std::string> parameters;  // Create an empty map to store the parameters

	for (const auto& endpoint : registered_endpoint) {
		std::string pattern = endpoint.first;
		std::regex regexPattern(std::regex_replace(pattern, std::regex(":\\w+"), "(\\w+)"));
		std::smatch matches;
		if (std::regex_match(url, matches, regexPattern)) {
			ret = endpoint.second;
			found = true;
			std::cout << "Route found";
			std::regex paramRegex(":\\w+");
			std::sregex_iterator paramIterator(pattern.begin(), pattern.end(), paramRegex);

			for (std::size_t i = 1; i < matches.size(); ++i) {
				std::string paramName = paramIterator->str().substr(1); // Remove the leading ":"
				std::string paramValue = matches[i].str();
				std::cout << "name " << paramName << " Value: " << paramValue << "\n\n";
				request.url_parameters[paramName] = paramValue;
				parameters[paramName] = paramValue; // Store the parameter name-value pair in the map

				++paramIterator;
			}

			break;
		}
	}
	if (!found) {
		std::cout << "This route is not found";
		if (!notFoundHandler) {
			notFoundHandler = notFoundDefaultHandler;
		}

		ret = notFoundHandler;

	}
	return ret;
}

std::unordered_map<std::string, std::string> parseRequestData(const std::string& request) {
	std::unordered_map<std::string, std::string> requestData;

	std::istringstream iss(request);
	std::string requestLine;
	std::getline(iss, requestLine);
	size_t verbEndPos = requestLine.find(' ');

	if (verbEndPos != std::string::npos) {
		std::string verb = requestLine.substr(0, verbEndPos);
		size_t urlEndPos = requestLine.find(' ', verbEndPos + 1);

		if (urlEndPos != std::string::npos) {
			std::string url = requestLine.substr(verbEndPos + 1, urlEndPos - verbEndPos - 1);
			requestData["verb"] = verb;
			requestData["url"] = url;
		}
	}

	return requestData;
}

std::unordered_map<std::string, std::string> parseHeaders(const std::string& request) {
	std::unordered_map<std::string, std::string> headers;

	std::istringstream iss(request);
	std::string line;

	// Skip the first line (request line)
	std::getline(iss, line);

	// Parse headers
	while (std::getline(iss, line) && !line.empty()) {
		size_t colonPos = line.find(':');
		if (colonPos != std::string::npos) {
			std::string headerName = line.substr(0, colonPos);
			std::string headerValue = line.substr(colonPos + 2); // Skip the colon and space

			// Remove any leading or trailing whitespaces from the header value
			headerValue.erase(0, headerValue.find_first_not_of(" \t"));
			headerValue.erase(headerValue.find_last_not_of(" \t") + 1);

			headers[headerName] = headerValue;
		}
	}

	return headers;
}
// Function to handle an individual client connection
void handleClient(ClientInfo* client) {
	// Receive and process the request
	char buffer[8194] = { 0x00 };
	int bytesRead = recv(client->sockfd, buffer, sizeof(buffer), 0);
	if (bytesRead > 0) {
		std::string request(buffer, bytesRead);


		auto headers = parseHeaders(request);
		auto head = parseRequestData(request);

		Request requestInfo;
		requestInfo.url = head["url"];
		requestInfo.verb = head["verb"];
		requestInfo.http_headers = headers;
		requestInfo.client_socket_file_descriptor = client;
		auto routeHandler = parseRequestRoute(requestInfo, notFoundDefaultHandler);


		if (routeHandler) {
			routeHandler(&requestInfo);
			return;
		}

		std::string stream;
		stream += "Http Verb and URL \n";
		stream += head["verb"] + " \n";
		stream += head["url"] + " \n\n";
		stream += "Http Headers";
		for (const auto& header : headers) {
			stream += header.first + ": " + header.second + "\n";
		}

		// Process the request and generate a response
		std::string response = "HTTP/1.1 200 OK\r\nContent-Length: ";
		response += std::to_string(stream.size());
		response += "\r\n\r\n";
		response += stream;
		//12\r\n\r\nHello World!";
		send(client->sockfd, response.c_str(), response.length(), 0);
	}

	// Close the client connection
	closesocket(client->sockfd);
	delete client;
}




Response* home(Request* request) {


	std::string stream;
	stream += "---HOME---PARAMS=--\n";
	stream += "Http Verb and URL \n";
	stream += request->verb + " \n";
	stream += request->url + " \n\n";
	stream += "Http Headers";
	for (const auto& header : request->http_headers) {
		stream += header.first + ": " + header.second + "\n";
	}
	stream += "\n---URL--parameter--->>\n";
	for (const auto& param : request->url_parameters) {
		stream += param.first + ":" + param.second;
	}


	// Process the request and generate a response
	std::string response = "HTTP/1.1 200 OK\r\nContent-Length: ";
	response += std::to_string(stream.size());
	response += "\r\n\r\n";
	response += stream;
	//12\r\n\r\nHello World!";
	send(request->client_socket_file_descriptor->sockfd, response.c_str(), response.length(), 0);
	closesocket(request->client_socket_file_descriptor->sockfd);
	delete request->client_socket_file_descriptor;
	return nullptr;
}
Response* user(Request* request) {

	std::string stream;
	stream += "---USER---PARAMS=--\n";
	stream += "Http Verb and URL \n";
	stream += request->verb + " \n";
	stream += request->url + " \n\n";
	stream += "Http Headers";
	for (const auto& header : request->http_headers) {
		stream += header.first + ": " + header.second + "\n";
	}
	stream += "---URL--parameter--->>\n";
	for (const auto& param : request->url_parameters) {
		stream += param.first + ":" + param.second + "\n";
	}


	// Process the request and generate a response
	std::string response = "HTTP/1.1 200 OK\r\nContent-Length: ";
	response += std::to_string(stream.size());
	response += "\r\n\r\n";
	response += stream;
	//12\r\n\r\nHello World!";
	send(request->client_socket_file_descriptor->sockfd, response.c_str(), response.length(), 0);

	closesocket(request->client_socket_file_descriptor->sockfd);
	delete request->client_socket_file_descriptor;
	return nullptr;
}


class HttpServer {
	
	bool run = false;
	int port = 9191;
	SOCKET serverSocket;

public:

	HttpServer() {

	}

	void RunServer() {
		std::cout << "Server started. Listening on port: " << port << std::endl;
		// Main event loop to handle incoming connections
		while (true) {
			// Accept incoming connection
			sockaddr_in clientAddr{};
			int clientAddrLen = sizeof(clientAddr);
			SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr,(socklen_t *) &clientAddrLen);
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
	}


	~HttpServer() {

	}

	int init() {
		serverSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (serverSocket == INVALID_SOCKET) {
			std::cerr << "Failed to create server socket" << std::endl;
			return 1;
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
			return 1;
		}

		// Start listening for incoming connections
		if (listen(serverSocket, MAX_CONNECTIONS) == SOCKET_ERROR) {
			std::cerr << "Failed to listen on server socket" << std::endl;
			return 1;
		}

	}
};


int main_server() 
{
	HttpServer server;
	server.init();
	server.RunServer();

#if 0
	int port = 9191;
	// Initialize WinSock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		std::cerr << "Failed to initialize WinSock" << std::endl;
		return 1;
	}
	registered_endpoint["/user/:id/:name"] = user;
	registered_endpoint["/home/:id"] = home;
	// Create a socket for incoming connections
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		std::cerr << "Failed to create server socket" << std::endl;
		return 1;
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
		return 1;
	}

	// Start listening for incoming connections
	if (listen(serverSocket, MAX_CONNECTIONS) == SOCKET_ERROR) {
		std::cerr << "Failed to listen on server socket" << std::endl;
		return 1;
	}

	std::cout << "Server started. Listening on port: " << port << std::endl;

	// Main event loop to handle incoming connections
	while (true) {
		// Accept incoming connection
		sockaddr_in clientAddr{};
		int clientAddrLen = sizeof(clientAddr);
		SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
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

	// Cleanup WinSock
	WSACleanup();
#endif
	return 0;
}
#endif
