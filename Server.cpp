#include "Server.h"
#include "HttpRequest.h"
#include <stdio.h>
#include <unistd.h>
#include <unordered_map>
#include <string>
using namespace std;
#include <openssl/ssl.h>
#include <openssl/err.h>
#ifdef __WIN32__
#include <openssl/applink.c>
#endif

#include <vector>
#include "JSON.h"

#ifdef __linux__
#include <sys/socket.h>
#include <arpa/inet.h>
#endif
#ifdef __WIN32__
  
static bool ok= false;
struct WinsockInit{
    WinsockInit(){
       WSAData data;
       WSAStartup(MAKEWORD(2,2),&data );
       ok=true;
    }
}public_initializer;


#else
static bool ok = true;
#endif





#ifdef __EXAMPLE__

GET /?say=Hi&to=Mom HTTP/1.1
Host: localhost:9025
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:64.0) Gecko/20100101 Firefox/64.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Connection: keep-alive
Upgrade-Insecure-Requests: 1



POST / HTTP/1.1
Host: localhost:9025
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:64.0) Gecko/20100101 Firefox/64.0
Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8
Accept-Language: en-US,en;q=0.5
Accept-Encoding: gzip, deflate
Content-Type: application/x-www-form-urlencoded
Content-Length: 13
Connection: keep-alive
Upgrade-Insecure-Requests: 1

say=Hi&to=Mom

PUT /new.html HTTP/1.1
Host: example.com
Content-type: text/html
Content-length: 16

<p>New File</p>

HTTP/1.1 201 Created
Content-Location: /new.html

HTTP/1.1 204 No Content
Content-Location: /existing.html
#endif















int create_socket(int16_t port)
{
    int s;
    struct sockaddr_in addr;

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(INADDR_ANY);

    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) {
	perror("Unable to create socket");
	exit(EXIT_FAILURE);
    }

    if (bind(s, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
	printf("Unable to bind\n");
	exit(EXIT_FAILURE);
    }
    
    int reuse_address = 1;
    int len = sizeof(reuse_address);
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR,(char *)&reuse_address, len);



    if (listen(s, 16) < 0) {
	perror("Unable to listen\n");
	exit(EXIT_FAILURE);
    }
    return s;
}

Socket createSSLSocket(int16_t port)
{
   Socket ret;
   ret.socket_fd = create_socket(port);
   return ret;
}

void init_openssl()
{ 
    SSL_load_error_strings();	
    OpenSSL_add_ssl_algorithms();
}

void cleanup_openssl()
{
    EVP_cleanup();
}

SSL_CTX *create_context()
{
    const SSL_METHOD *method;
    SSL_CTX *ctx;

    method = SSLv23_server_method();

    ctx = SSL_CTX_new(method);
    if (!ctx) {
	perror("Unable to create SSL context");
	ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    return ctx;
}

void configure_context(SSL_CTX *ctx)
{
    SSL_CTX_set_ecdh_auto(ctx, 1);

    /* Set the key and cert */
    if (SSL_CTX_use_certificate_file(ctx, "cert.pem", SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }

    if (SSL_CTX_use_PrivateKey_file(ctx, "key.pem", SSL_FILETYPE_PEM) <= 0 ) {
        ERR_print_errors_fp(stderr);
	exit(EXIT_FAILURE);
    }
}



string remove_leading_space(string b){
    string ret;
    for(int i=0;i<b.length(); i++)
    {
        if(!isspace(b[i]))
            ret+=b[i];
        else
        {
             ret+="<...>";
        }
        
    }
    return ret;
}


string getAllUser()
{
    JSONObject object;
    JsonPair pair1("user_id", std::string("TOK1200034"));
    JsonPair pair2("user_password", "123456");
    JsonPair pair3("user_date_of_registry", "0000-00-00T12:00:00");
    JsonPair pair4("logged", true);
    JsonPair pair5("number", 12);
    JsonPair pair6("float_expression", 12.4f);
    object.addElement(&pair1);
    object.addElement(&pair2);
    object.addElement(&pair3);
    object.addElement(&pair4);
    object.addElement(&pair5);
    object.addElement(&pair6);
    return object.stringify();
}



void handleHTTPResponse(int client, const char *query, int len )
{
    cout <<"Without Connection handler\n"<< query <<"\n\n\n";
    HTTPRequest query_request(query);
    string response_buffer;
    response_buffer += "HTTP/1.1 200 OK\r\n";
    response_buffer += "Content-Type: application/json\r\n";
    string body_response;
   body_response = getAllUser();
   response_buffer += "Content-Length:"+  std::to_string(body_response.length());
   response_buffer +="\r\n\r\n";
   response_buffer += body_response.c_str();
   int rc =  send(client, response_buffer.c_str(), response_buffer.size(), 0);
}

void handleHTTPResponseWithMiddleWare(int client, const char *query, int len, std::map<Identifier , http_request_handler> middleware )
{
//    cout <<"With Connection handler\n"<< query <<"\n\n\n";
//    
//    string response_buffer;
//    response_buffer += "HTTP/1.1 200 OK\r\n";
//    response_buffer += "Content-Type: application/json\r\n";
//    string body_response;
//    body_response = getAllUser();
//    response_buffer += "Content-Length:"+  std::to_string(body_response.length());
//    response_buffer +="\r\n\r\n";
//    response_buffer += body_response.c_str();
//    int rc =  send(client, response_buffer.c_str(), response_buffer.size(), 0);
      HTTPRequest query_request(query);
      Identifier identifier(query_request.path,query_request.verb);
      http_request_handler handler = middleware[identifier];
      if(handler)
      {
          handler(client, &query_request);
      }
      else
      {
          std::cout <<"this handler is null redireting to default handler";
          handleHTTPResponse( client,  query,  len );
      }



}





Server::Server(ServerHandler *handler)
{
   server_handler=nullptr;
}


Server::Server(int16_t port,bool ssl_bindingconst ,const char *instance_name, handle_response_ptr handler   )
{
  server_handler=nullptr;
  this->instance_name = (!instance_name)?"NO-NAME-INSTANCE":instance_name;
  this->ssl_enable    = ssl_bindingconst;
  this->port          = port;
  if(this->ssl_enable)
  {
    init_openssl();
    ctx = create_context();
    configure_context(ctx);
    this->socket_fd.socket_fd = create_socket(port);
  }
  else
  {
      this->socket_fd.socket_fd = create_socket(port);
      ///printf("Socket fd value is =%d\n", this->socket_fd.socket_fd);
  }

}
      
bool Server::support_ssl()
{
    return this->ssl_enable;
}

void Server::default_launcher()
{
   if(ssl_enable)
   {
       /* Handle SSL connections */
    printf("Waiting for client on port %d\n", this->port);
    while(1) {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        SSL *ssl;
        const char reply[] = "test\n";

        int client = accept(socket_fd.socket_fd, (struct sockaddr*)&addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
        }
        else {
            printf("client accepted... %s\n ", this->instance_name);
            char buffer[8*1024]={0};
            int rc =SSL_read(ssl,buffer,8*1024 );
            SSL_write(ssl, buffer, rc);
        }

        SSL_free(ssl);
        shutdown(client,2);
    }

   }
   else
   {
/* Handle NONSSL connections */
    printf("Waiting for client on port %d\n", this->port);
    while(1) 
    {
        struct sockaddr_in addr;
        socklen_t len = sizeof(addr);
        const char reply[] = "test\n";
        int client = accept(socket_fd.socket_fd, (struct sockaddr*)&addr, &len);
        if (client < 0) 
        {
            printf("client rejected for some reason");
        }
        else
        {
            printf("client accepted... %s\n ", this->instance_name);
        }
            char buffer[8*1024]={0};
            int rc = recv(client, buffer, (8*1024), 0);
            if(this->connection_handler.size()<=0)
              handleHTTPResponse(client, buffer, rc);
            else
              handleHTTPResponseWithMiddleWare(client, buffer, rc,connection_handler);
         if(client>0)
           printf("close connection.....\n");
        shutdown(client,2);
    }
   }
}



void Server::launch()
{
    if(nullptr==server_handler)
    {
        printf("Default Launcher is called...\n");
        default_launcher();
    }
    else
    {
        server_handler->launch();
    }
/*
   if(ssl_enable)
   {
       
    printf("Waiting for client on port %d\n", this->port);
    while(1) {
        struct sockaddr_in addr;
        int len = sizeof(addr);
        SSL *ssl;
        const char reply[] = "test\n";

        int client = accept(socket_fd.socket_fd, (struct sockaddr*)&addr, &len);
        if (client < 0) {
            perror("Unable to accept");
            exit(EXIT_FAILURE);
        }

        ssl = SSL_new(ctx);
        SSL_set_fd(ssl, client);

        if (SSL_accept(ssl) <= 0) {
            ERR_print_errors_fp(stderr);
        }
        else {
            printf("client accepted... %s\n ", this->instance_name);
            char buffer[8*1024]={0};
            int rc =SSL_read(ssl,buffer,8*1024 );
            SSL_write(ssl, buffer, rc);
        }

        SSL_free(ssl);
        shutdown(client,2);
    }

   }
   else
   {
 
    printf("Waiting for client on port %d\n", this->port);
    while(1) 
    {
        struct sockaddr_in addr;
        int len = sizeof(addr);
        const char reply[] = "test\n";
        int client = accept(socket_fd.socket_fd, (struct sockaddr*)&addr, &len);
        if (client < 0) 
        {
            printf("client rejected for some reason");
        }
        else
        {
            printf("client accepted... %s\n ", this->instance_name);
        }
            char buffer[8*1024]={0};
            int rc = recv(client, buffer, (8*1024), 0);
            if(this->connection_handler.size()<=0)
              handleHTTPResponse(client, buffer, rc);
            else
              handleHTTPResponseWithMiddleWare(client, buffer, rc,connection_handler);
         if(client>0)
           printf("close connection.....\n");
        shutdown(client,2);
    }
   }
   */
}

void Server::get(string path, http_request_handler handler)
{
    Identifier identifer(path, HTTP_VERB::HTTP_GET);
    connection_handler[identifer]= handler;
}
void Server::post(string path   , http_request_handler handler)
{
    Identifier identifer(path, HTTP_VERB::HTTP_POST);
    connection_handler[identifer]= handler;
}
void Server::put(string path   , http_request_handler handler)
{
    Identifier identifer(path, HTTP_VERB::HTTP_PUT);
    connection_handler[identifer]= handler;
}

int Server::SendResponse(int client, char *buffer, int len)
{
  return send(client, buffer, len, 0);
}

int Server::SendResponse(int client, const std::string &buffer)
{
    return send(client, buffer.c_str(), buffer.length(), 0);
}