#include "HttpServer.h"
#include "HttpPrimitiveUtils.h"
#include "HttpServerMetada.h"

Response *DefaultEndpoint(Request *request){
   Response *ret = new Response();
   ret->body ="NO RESPONSE MSG";
   ret->headers["Content-type"] = "text/plain";
   ret->statusCode = 400;
   return ret;
}
Response *DefaultEndpointW200(Request *request){
   Response *ret = new Response();
   ret->body ="NO RESPONSE MSG";
   ret->headers["Content-type"] = "text/plain";
   ret->statusCode = 200;
   return ret;
}


UrlDatatype getNodeDataTypeByPattern(const std::string& routeNode){
    int s = routeNode.size();
     
        int pos = routeNode.find(":int>");        
        if(pos!=std::string::npos)
            return UrlDatatype::Integer;
        pos = routeNode.find(":str>");        
        if(pos!=std::string::npos)
            return UrlDatatype::Str;
        pos = routeNode.find(":boolean>");        
        if(pos!=std::string::npos)
            return UrlDatatype::Boolean;
   
    return UrlDatatype::InvalidUrlDatatype;
}
const std::string UrlDatatypeAsStr(const UrlDatatype& type){
   switch(type){
     case UrlDatatype::Integer: return "Integer";
     case UrlDatatype::Str: return "Str";
     case UrlDatatype::Boolean: return "Boolean";
     case UrlDatatype::InvalidUrlDatatype:
     default: return "Invalid";
   }
}

bool isBoolean(std::string sn){
    if(sn.find("true")!=std::string::npos ||
       sn.find("false")!=std::string::npos
      )
      return true;
    return false;
}

bool isInteger(std::string sn){
     for(auto it : sn)
        if(!isdigit(it))
          return false;
     return true;
}

UrlDatatype getDataTypeByValue(std::string value){
    if(isInteger(value))
       return UrlDatatype::Integer;
    if(isBoolean(value))
       return UrlDatatype::Boolean;
    return UrlDatatype::Str;
}
std::vector<std::string> splitWords(std::string word, char sp){
     std::vector<std::string> ret;
     int iter=0;
     std::string str;
     while(word[iter]!='\0'){
        
        if(word[iter]!=sp){
          str+= word[iter];
        }
        else{
            if(str.size()>0)
              ret.push_back(str);
            str="";
        }
        iter++;
     }
     if(str.size()>0)
       ret.push_back(str);
     return ret;
  };

bool validatePatterByUrl(std::vector<std::string> uri, std::vector<std::string> pattern){

      bool found = false;
      auto uIter = uri.begin();
      auto pIter = pattern.begin();
      auto uIterEnd = uri.end();
      while(uIter != uIterEnd){
         if(*uIter!=*pIter){
            std::string value  = *uIter;
            std::string pValue = *pIter;
            if(pValue[0]!='<')
            {
              return false;
            }
            auto ptype = getNodeDataTypeByPattern(pValue);
            auto vType = getDataTypeByValue(value);
            if(ptype!=vType)
               return false;
         }else{
            std::cout << *uIter <<"  EST ES IGUAL NOTHING TO DO\n";
         }
         pIter++;
         uIter++;
      }
      return true;
  };


std::pair<std::string, ParserEndpoint> getEndpointFromMap(const std::string uri,const std::map<std::string, ParserEndpoint>& enpointList){

    std::pair<std::string, ParserEndpoint> res;
    res.first ="not_valid";
    auto splittedUri = splitWords(uri, '/');
    for(auto it : enpointList){
         auto eSplitted = splitWords(it.first, '/');
         if(eSplitted.size()!=splittedUri.size())
            continue;
        bool status = validatePatterByUrl(splittedUri, eSplitted);
        if(status==true)
           return it;
    }
    return res;
}





namespace {

const int MAX_CONNECTIONS = 1000;

}

std::string HttpServer::RootPath = "/etc/www";

void HttpServer::setRootPath(const std::string& path){
    RootPath = path;
}

const std::string HttpServer::getRootPath() const{
    return  RootPath;
}

HttpServer HttpServer::endpoint(const std::string& endpoint,const ParserEndpoint& parsed)
{
    registered_endpoint[endpoint] = parsed;
    return (*this);
}

 bool HttpServer::validateEnpointRoute(const std::string& path){
   
    return false;
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
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (const char *) &opt, sizeof(opt));

    // Set up server address and port
    sockaddr_in serverAddr{};
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    // Bind the socket to the server address
    if (bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        std::cerr << "Failed to bind server socket: " << std::endl;
    }

    // Start listening for incoming connections
    if (listen(serverSocket, MAX_CONNECTIONS) == SOCKET_ERROR) {
        std::cerr << "Failed to listen on server socket" << std::endl;
        return;
    }

    std::cout << "Server v0.1 started\nListening on port : 0.0.0.0:" << port <<" "<< std::endl;
}

int HttpServer::start()
{
    while (true) {
        // Accept incoming connection
        sockaddr_in clientAddr{};
        int clientAddrLen = sizeof(clientAddr);
        SOCKET clientSocket = accept(serverSocket,
                                     (struct sockaddr *) &clientAddr,
                                     (socklen_t *) &clientAddrLen);
        if (clientSocket == INVALID_SOCKET) {
            std::cerr << "Failed to accept client connection" << std::endl;
            continue;
        }

        // Create a new client info structure
        ClientInfo *client = new ClientInfo;
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
