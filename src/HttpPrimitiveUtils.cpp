#include "HttpPrimitiveUtils.h"
#include "IOUtils.h"

#define TEST_STATIC_FILE "index.html"


std::unordered_map<std::string, std::string> parseUrlParams(std::string url)
{
    std::unordered_map<std::string, std::string> ret;
    // if (url.find("?") != std::string::npos) {
   
    // }
    size_t pos = url.find("?");
    if(pos!=std::string::npos){
        std::string paramList = url.substr(pos,url.size()-pos);
    }
    return ret;
}
 

 ParserEndpoint parseDinamicRequest(Request &request, ParserEndpoint notFoundHandler){
 ParserEndpoint ret = nullptr;
    std::string url = request.url;
    bool found = false;
    std::unordered_map<std::string, std::string>
        parameters; // Create an empty map to store the parameters

    request.urlParams = parseUrlParams(request.url);

    for (const auto &endpoint : registered_endpoint) {
        std::string pattern = endpoint.first;

        std::cout << endpoint.first <<"\n";

        std::regex regexPattern(std::regex_replace(pattern, std::regex(":\\w+"), "(\\w+)"));
        std::smatch matches;
        if (std::regex_match(url, matches, regexPattern)) {
            ret = endpoint.second;
            found = true;

            std::regex paramRegex(":\\w+");
            std::sregex_iterator paramIterator(pattern.begin(), pattern.end(), paramRegex);

            for (std::size_t i = 1; i < matches.size(); ++i) {
                std::string paramName = paramIterator->str().substr(1); // Remove the leading ":"
                std::string paramValue = matches[i].str();
 
                parameters[paramName] = paramValue; // Store the parameter name-value pair in the map

                ++paramIterator;
            }

            break;
        }
    }
    if (!found) {
        if (!notFoundHandler) {
            std::cout << request.url <<"  WOULD BE NOT FOUND?????\n";
            notFoundHandler = [](Request *) -> Response * {
                auto response = new Response();
                response->body = "EMPTY REQUEST [\"  \"]";
                response->headers["Content-Type"] = "text/plain";
                return response;
            };
        }

        ret = notFoundHandler;
    }
    // Store the parameters in the Request object
    request.parameters = parameters;
    return ret;
}
 
std::string toLowerCaseString(const std::string& str){
    std::string ret = str;
    std::transform(ret.begin(), ret.end(), ret.begin(),[](char s){
          return std::tolower(s);        
    });
    return ret;
}

RequestVerb parseRequestVerb(std::string buffer){
    auto testStr = toLowerCaseString(buffer);
   /// std::cout << testStr <<"!!!!";
    if(testStr.find("get")!=std::string::npos)
        return RequestVerb::Get;
    else if(testStr.find("post")!=std::string::npos)
        return RequestVerb::Post;
    return RequestVerb::Get;
}

std::string extractBody(const std::string str){
    int pos =str.find("\r\n\r\n");
    if(pos < str.size());
       return " ";
    std::string body = str.substr(pos+2, str.size()-(pos+2));
    return body;
}

ParserEndpoint parseRequest(Request &request, ParserEndpoint notFoundHandler)
{
    return parseDinamicRequest(request, notFoundHandler);
}

void extractQueryParameters(std::string urlLine,std::unordered_map<std::string, std::string>& ret){
    size_t queryPos = urlLine.find("?");
    std::string line = urlLine.substr(queryPos+1, urlLine.size()-(queryPos+11));
 
    int iter = 0;
    int status=0;
    std::string k;
    std::string v;
    while(line[iter]!='\0'){
 
       if(line[iter]=='='){
          iter++;
          status = 1;
          continue;
       }else if(line[iter]=='&'){
          status = 0;
          iter++;
          ret[k] = v;
           
          k = "";
          v = "";
          continue;
       }else{
          if(status==0){
            k+= line[iter];
          }else{
            v+= line[iter];
          }
       }
       iter++;
    }
    ret[k] = v;
   
}


std::unordered_map<std::string, std::string> parseRequest(const std::string &request)
{
    std::unordered_map<std::string, std::string> requestData;

    std::istringstream iss(request);
    std::string requestLine;

    // Parse the request line
    std::getline(iss, requestLine);
 
    // Find the position of the first space character
    size_t verbEndPos = requestLine.find(' ');

    size_t queryPos = requestLine.find("?");

    if(queryPos!=std::string::npos){
       
        extractQueryParameters(requestLine,requestData);
         
    }

    if (verbEndPos != std::string::npos) {

        std::string verb = requestLine.substr(0, verbEndPos);

        size_t urlEndPos = requestLine.find(' ', verbEndPos + 1);

        if (urlEndPos != std::string::npos) {
            // Extract the URL
            std::string url = requestLine.substr(verbEndPos + 1, urlEndPos - verbEndPos - 1);
            // Save the verb and URL into the map
            requestData["verb"] = verb;
            requestData["url"] = url;
        }
    }
    return requestData;
}

std::unordered_map<std::string, std::string> parseHeaders(const std::string &request)
{
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
void handleClient(ClientInfo *client)
{
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

 

#ifdef __JUST_TEST
        stream += "Http Verb and URL \n";
        stream += head["verb"] + " \n";
        stream += head["url"] + " \n\n";
        stream += "Http Headers";
        for (const auto &header : headers) {
            stream += header.first + ": " + header.second + "\n";
        }
#else
        //    if(res)
        //	  stream +=
        //    else
        //	  stream += readWholeFile(TEST_STATIC_FILE);
#endif

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
