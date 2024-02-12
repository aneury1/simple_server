#include "HttpPrimitiveUtils.h"
#include "IOUtils.h"

#define TEST_STATIC_FILE "index.html"

/**
 * this function will convert string to lower case.
*/
std::string toLowerCaseString(const std::string& str){
    std::string ret = str;
    std::transform(ret.begin(), ret.end(), ret.begin(),[](char s){
          return std::tolower(s);        
    });
    return ret;
}

/// @brief Parse the buffer to Http Verb enum
/// @param buffer 
/// @return RequestVerb
RequestVerb parseRequestVerb(std::string buffer){
    auto testStr = toLowerCaseString(buffer);
   /// std::cout << testStr <<"!!!!";
    if(testStr.find("get")!=std::string::npos)
        return RequestVerb::Get;
    else if(testStr.find("post")!=std::string::npos)
        return RequestVerb::Post;
    return RequestVerb::Get;
}

std::string RequestVerbToString(RequestVerb httpVerb){
    switch(httpVerb){
        case RequestVerb::Get:
            return "GET";
        case RequestVerb::Post:
            return "POST";
        default:
            return "UNDEFINED";
    }
}


/// @brief this function help to parse url params like /host?id=1;name=aneury 
///        this would convert to map: map[id]=1, map[name]=aneury
/// @return unordered_map<string,string> 
std::unordered_map<std::string, std::string> parseUrlParams(std::string url)
{
    std::unordered_map<std::string, std::string> ret;
    size_t pos = url.find("?");
    if(pos!=std::string::npos){
        std::string paramList = url.substr(pos+1,url.size()-pos);
        auto vars = splitWords(paramList, ';');
        for(auto var : vars){
            auto keyPairs = splitWords(var,'=');
            ret[keyPairs[0]] = keyPairs[1];
        }
    }
    return ret;
}

/// @brief extract Get, Post Delete Request
/// @param request 
/// @return 
std::string extractHttpVerb(const std::string request){
   std::string ret = request.substr(0, request.find_first_of(" "));
   return ret;
}

/// @brief Extract current Url
/// @param request 
/// @return 
std::string extractUrlWithQueryParams(std::string request){
   int firstSpace = request.find_first_of(" ");
   int findHttpVersion = request.find(" HTTP/1.1");
   ///std::cout << firstSpace << " -> " << findHttpVersion <<"\n\n\n "<<request;
   std::string url = request.substr(firstSpace+1, findHttpVersion-(firstSpace+1));
   //// std::cout <<" URL "<< url;
   return url;
}

/// @brief Extract current Url
/// @param request 
/// @return 
std::string extractUrl(std::string request){
   int firstSpace = request.find_first_of(" ");
   int findHttpVersion = request.find(" HTTP/1.1");
   std::string url = request.substr(firstSpace+1, findHttpVersion-(firstSpace+1));
   if(url.find_first_of('?')!=std::string::npos){
      return url.substr(0, url.find_first_of("?"));
   }
   return url;
}


/// @brief  Extract Content Body...
/// @param str 
/// @return 
std::string extractBody(const std::string str){
    int pos =str.find("\r\n\r\n");
    if(pos < str.size());
       return " ";
    std::string body = str.substr(pos+2, str.size()-(pos+2));
    return body;
}
/// @brief Parse Http Header..
/// @param request 
/// @return 
std::unordered_map<std::string, std::string> parseHeaders(const std::string &request)
{
    std::unordered_map<std::string, std::string> headers;

    std::istringstream iss(request);
    std::string line;
    // Skip the first line (request line)
    std::getline(iss, line);

    while (std::getline(iss, line) && !line.empty()) {
        size_t colonPos = line.find(':');
        if (colonPos != std::string::npos) {
            std::string headerName = line.substr(0, colonPos);
            std::string headerValue = line.substr(colonPos + 2); // Skip the colon and space

            // Remove any leading or trailing whitespaces from the header value
            headerValue.erase(0, headerValue.find_first_not_of(" \t"));
            headerValue.erase(headerValue.find_last_not_of(" \t") + 1);
            headerValue.erase(0, headerValue.find_first_not_of("\r"));
            headerValue.erase(0, headerValue.find_first_not_of("\n"));
            headerValue.erase(headerValue.find_last_not_of("\r\n") + 1);
            headers[headerName] = headerValue;
        }
    }

    return headers;
}


ParserEndpoint parseRequest(Request &request, ParserEndpoint notFoundHandler)
{
   return nullptr;
}

// Function to handle an individual client connection
void handleClient(ClientInfo *client)
{


    char buffer[8194] = {0x00};
    int bytesRead = recv(client->sockfd, buffer, sizeof(buffer), 0);

    if (bytesRead > 0) {
        std::string request(buffer, bytesRead);
    
      std::string httpVerb            = extractHttpVerb(request);
      std::string urlDynamicPath      = extractUrlWithQueryParams(request);
      std::string url                 = extractUrl(request);
      auto headers                    = parseHeaders(request);
      std::unordered_map<std::string, std::string> params = parseUrlParams(urlDynamicPath);
     
     
      auto it = getEndpointFromMap(url,registered_endpoint);

      std::string response;

      if(it.first == "not_valid"){
            std::stringstream body;

            body << "HTTP VERB: "<< httpVerb <<"\n"
                << "URL: " << url <<"\n"
                << "Dynamic Url: "<< urlDynamicPath <<"\n"
                << "HTTP Headers: ";
            for(auto header : headers){
                body << header.first <<": "<<header.second <<"\n";
            }
            if(params.size()>0){
                body <<"Query Parameters: \n";
                for(auto param : params ){
                    body << param.first <<" : "<< param.second<<"\n";
                }
            }
                response = "HTTP/1.1 200 OK\r\n";
                response += ("Content-Length: ") + std::to_string(body.str().size()) + "\r\n";
                response += "Content-Type: text/plain\r\n";
                response += "\r\n\r\n";
                response += body.str();
      }else{

         Request request;
         request.body = "";
         request.headers = headers;
         request.url = url;
         request.urlParams = params;
         auto res = it.second(&request);
               
         response = "HTTP/1.1 200 OK\r\n";
         response += ("Content-Length: ") + std::to_string((*res).body.size()) + "\r\n";
         response += "Content-Type: text/plain\r\n";
         response += "\r\n\r\n";
         response += (*res).body.c_str();
         
      }

      int querysend = send(client->sockfd, response.c_str(), response.length(), 0);
    }


#if 0  
    // Receive and process the request
    char buffer[8194] = {0x00};
    int bytesRead = recv(client->sockfd, buffer, sizeof(buffer), 0);

    if (bytesRead > 0) {
        std::string request(buffer, bytesRead);

        auto headers = parseHeaders(request);
        auto head = parseRequest(request);
        Request thisRequest;
        thisRequest.url = head["url"];
        thisRequest.parameters = headers;
        thisRequest.requestVerb = parseRequestVerb(buffer);
        thisRequest.body = extractBody(buffer);

        auto res = parseRequest(thisRequest, nullptr);

        std::string stream;


        auto workedResponse = res(&thisRequest);

        // Process the request and generate a response
        std::string response = "HTTP/1.1 200 OK\r\n";
        response += ("Content-Length: ") + workedResponse->headers["Content-Length"] + "\r\n";
        response += ("Content-Type: " + workedResponse->headers["Content-Type"]);
        response += "\r\n\r\n";
        response += workedResponse->body;
        //12\r\n\r\nHello World!";

        // Send the response back to the client
        int querysend = send(client->sockfd, response.c_str(), response.length(), 0);
    }

    // Close the client connection
    closesocket(client->sockfd);
    delete client;
#endif
}


#include <sstream>
#include <map>
#include <string>

 


std::string fetchJsBuilder(
    std::string url, 
    std::string body, 
    std::map<std::string,std::string>fetchOptions,
    std::map<std::string,std::string>httpHeaders
    ){


   std::stringstream fetchOptsStream;
  
   fetchOptsStream << "async function postData(url = \"\", data = {}) {\r\n";
   fetchOptsStream << "const response = await fetch(\"" <<url <<"\",{\r\n";
   for(auto it : fetchOptions){
      fetchOptsStream << it.first <<": \""<<it.second<<"\",\r\n\t";
   } 


   if(httpHeaders.size()>0){
      fetchOptsStream <<"headers: {";
         for(auto it : httpHeaders)
             fetchOptsStream <<"\""<<it.first<<"\":"<<"\""<<it.first<<"\",\r\n\t";
         fetchOptsStream <<"masterc:1\r\n}";
   }
   if(body.size()>0){
       fetchOptsStream << "\r\n,body: "<<"JSON.stringify("<<"{}"<<")});";
   }else{
       fetchOptsStream << ",});\r\n";
   }
  fetchOptsStream << "\r\nreturn response.json(); \r\n}\r\n";
  return fetchOptsStream.str();
#if 0
 const response = await fetch(url, {
    method: "POST", // *GET, POST, PUT, DELETE, etc.
    mode: "cors", // no-cors, *cors, same-origin
    cache: "no-cache", // *default, no-cache, reload, force-cache, only-if-cached
    credentials: "same-origin", // include, *same-origin, omit
    headers: {
      "Content-Type": "application/json",
      // 'Content-Type': 'application/x-www-form-urlencoded',
    },
    redirect: "follow", // manual, *follow, error
    referrerPolicy: "no-referrer", // no-referrer, *no-referrer-when-downgrade, origin, origin-when-cross-origin, same-origin, strict-origin, strict-origin-when-cross-origin, unsafe-url
    body: JSON.stringify(data), // body data type must match "Content-Type" header
  });
  return response.json(); // parses JSON response into native JavaScript objects
#endif
}


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

            if(str.size()>0){
              if(word[iter]==sp){
              ret.push_back(str);
              str="";
              }else if(word[iter+1]==sp){
                str+= word[iter];
                ret.push_back(str);
                str="";
              }
            }
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

std::string generateStrRequestPaylod(const std::string& path,const RequestVerb& verb) {
#ifdef VERSION_2_0
   // std::string secWebSocketKey = generateRandomString(16);
   // std::string secWebSocketAccept = generateSHA1Hash(secWebSocketKey + "258EAFA5-E914-47BE-9B52-6C18FB827A3A"); // Magic string
#endif
    const std::string hostname ="localhost";
    const std::string port = "9090"; 
    std::string request = RequestVerbToString(verb) +" " + path + " HTTP/1.1\r\n";
    request += "Upgrade: websocket\r\n";
    request += "Connection: Upgrade\r\n";
    request += "Host: " + hostname + ":" + port + "\r\n";
    request += "supper-long-lasting-information-parameter: 258EAFA5-E914-47BE-9B52-6C18FB827A3A\r\n";
#ifdef VERSION_2_0
    request += "Sec-WebSocket-Key: " + secWebSocketKey + "\r\n";
    request += "Sec-WebSocket-Accept: " + secWebSocketAccept + "\r\n";
#endif
    // Add other optional headers if needed

    request += "\r\n";
    return request;
}