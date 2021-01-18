#pragma once
 
#ifdef __WIN32__
#include <winsock2.h>
#include <windows.h>
struct WinsockInitializer{

};
#else
typedef int SOCKET;
#endif
#include <time.h>
#include <stdlib.h>
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
#include <stdio.h>

#ifdef __linux__
#include <unistd.h>
#endif
///#define OPENSSL_INCLUDE
#ifdef OPENSSL_INCLUDE
#include <openssl/ssl.h>
#include <openssl/err.h>
#endif

#ifdef __linux__
#include <sys/socket.h>
#include <arpa/inet.h>
#endif


#include "ServerHandler.h"
#include "HttpRequest.h"

#ifdef ___GUID_GENERATOR__

char *genGUID()
{
  char buffer[36]={0}; 
  srand(time(NULL));
  sprintf(buffer, "%x%x-%x-%x-%x-%x%x%x",
			rand(), rand(),		// Generates a 64-bit Hex number
			rand(),				// Generates a 32-bit Hex number
			((rand() & 0x0fff) | 0x4000),	// Generates a 32-bit Hex number of the form 4xxx (4 indicates the UUID version)
			rand() % 0x3fff + 0x8000,		// Generates a 32-bit Hex number in the range [0x8000, 0xbfff]
			rand(), rand(), rand());		// Generates a 96-bit Hex number
   return buffer;
}
#endif


struct Socket
{
    SOCKET      socket_fd;
    sockaddr_in address;
};

struct Identifier : public std::pair<std::string, HTTP_VERB>
{
      std::string path;
      HTTP_VERB   verb;
      Identifier()=delete;
      Identifier(string path, HTTP_VERB verb):pair(path, verb){}
};

typedef void (*handle_response_ptr)(int , char *, int );

typedef void (*http_request_handler)(int client_socket, HTTPRequest *request);


class Server
{
   Socket socket_fd;

   bool running_stack;
  
   bool ssl_enable;

#ifdef OPENSSL_INCLUDE
   SSL_CTX *ctx;
#endif
  int16_t port;

  handle_response_ptr response_handler;

  void default_launcher();

  protected:

  ServerHandler* server_handler;
 
  ////typedef vector<http_request_handler> middleware_base;
  
  std::map<Identifier , http_request_handler> connection_handler;

  public:


 
   Server(ServerHandler *handler);
   Server(int16_t port,bool ssl_binding = false,const char *instance_name=nullptr, handle_response_ptr handler =nullptr);
   
   const char *instance_name;
   
   bool support_ssl();

   void launch();

   void get(string path, http_request_handler);
   void post(string path   , http_request_handler);
   void put(string path   , http_request_handler);

   static int SendResponse(int client, char *buffer, int len);
   static int SendResponse(int client, const std::string &buffer);
};