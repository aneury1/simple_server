
#include "htmlbuilder.h"
#include "HttpServer.h"

#ifdef WIN32
#pragma comment(lib, "ws2_32.lib")
#endif 

Response *homeDir(Request *request)
{
    for(auto it : request->parameters){
        std::cout << "Parameter: " << it.first <<" : "<< it.second <<"\n";
    }
    auto response = new Response();
    response->body = "Simple Text";
    response->headers["Content-Type"] = "text/html";
    response->headers["Content-Length"] = std::to_string(response->body.size());
    response->statusCode = 200; 
    return response;
}

Response *flutterJS(Request *request)
{
    auto buffer = readWholeFile("/home/aneury1/project/testbed/build/web/flutter.js");
    auto response = new Response();
    response->body = buffer;
    response->headers["Content-Type"] = "text/javascript; charset=utf-8";
    response->headers["Content-Length"] = std::to_string(buffer.size());
    response->statusCode = 200;
    return response;
}

Response *anyAssets(Request *request)
{
    auto response = new Response();
    response->body = "this should be anyAssets";
    response->headers["Content-Type"] = "text/html";
    response->headers["Content-Length"] = "0";
    response->statusCode = 400;
    return response;
}

Response *anyAssetsWithParams(Request *request)
{
    auto response = new Response();
    response->body = "this should be anyAssets With Params";
    response->headers["Content-Type"] = "text/html";
    return response;
}

Response *requestAnyJSFileinRoot(Request *request)
{
    std::string file = request->url.substr(1, request->url.size() - 1);
    auto buffer = readWholeFile(HttpServer::RootPath+"/"+ file);
    auto response = new Response();
    response->body = buffer;
    response->headers["Content-Type"] = "text/javascript; charset=utf-8";
    response->headers["Content-Length"] = std::to_string(buffer.size());
    response->statusCode = 200;
    return response; ///comment
}

Response *requestAnyHTMLFileinRoot(Request *request)
{
    std::string file = request->url.substr(1, request->url.size() - 1);
    auto buffer = readWholeFile(HttpServer::RootPath+"/"+ file);
    auto response = new Response();
    response->body = buffer;
    response->headers["Content-Type"] = "text/html";
    response->headers["Content-Length"] = std::to_string(buffer.size());
    response->statusCode = 200;
    return response;
}

Response *requestAnyJSONFileinRoot(Request *request)
{
    std::string file = request->url.substr(1, request->url.size() - 1);
    auto buffer = readWholeFile(HttpServer::RootPath+"/"+ file);
    auto response = new Response();
    response->body = buffer;
    response->headers["Content-Type"] = "text/javascript; charset=utf-8";
    response->headers["Content-Length"] = std::to_string(buffer.size());
    response->statusCode = 200;
    return response;
}

Response *apiWareTest(Request *request)
{
    std::string file = request->url.substr(1, request->url.size() - 1);
    auto buffer = std::string("PRUEBA");
    
    for(auto re : request->parameters){
        buffer+="\n";
        buffer+="Key: "+ re.first;
        buffer+= "Value: "+ re.second;
        buffer+="\n";
    }
    
    auto response = new Response();
    response->body = buffer;
    response->headers["Content-Type"] = "text/javascript; charset=utf-8";
    response->headers["Content-Length"] = std::to_string(buffer.size());
    response->statusCode = 200;

    return response;
}

int main(int argc, char *argv[])
{
    return HttpServer(9293)
        .registerNewStaticEndpoint("/", homeDir)
        .registerNewStaticEndpoint("/cliente/:id/:nombre/orden", apiWareTest)
        .registerNewStaticEndpoint("/:id.html", requestAnyHTMLFileinRoot)
        .registerNewStaticEndpoint("/:id.js", requestAnyJSFileinRoot)
        .registerNewStaticEndpoint("/:id.json", requestAnyJSONFileinRoot)
        .registerNewStaticEndpoint("/assets", anyAssets)
        .registerNewStaticEndpoint("/flutter.js", flutterJS)
        .registerNewStaticEndpoint("/assets/:xparams", anyAssetsWithParams)
        .start();
}