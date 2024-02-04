
#include "htmlbuilder.h"
#include "HttpServer.h"
#pragma comment(lib, "ws2_32.lib")

Response *homeDir(Request *request)
{

    for(auto it : request->parameters){
        std::cout << "Parameter: " << it.first <<" : "<< it.second <<"\n";
    }


    auto response = new Response();
    response->body = "Simple Text";
    ///std::cout << response->body;
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
#include <exception>
Response *requestAnyJSFileinRoot(Request *request)
{
    ///std::cout << request->url<<"Render File-->" << request->url.substr(1, request->url.size()-1);
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
    ///std::cout << request->url<<"Render File-->" << request->url.substr(1, request->url.size()-1);
    std::string file = request->url.substr(1, request->url.size() - 1);
    auto buffer = readWholeFile(HttpServer::RootPath+"/"+ file);
    auto response = new Response();
    response->body = buffer;
    response->headers["Content-Type"] = "text/html";
    response->headers["Content-Length"] = std::to_string(buffer.size());
    response->statusCode = 200;
    return response; ///comment
}

Response *requestAnyJSONFileinRoot(Request *request)
{
    //// std::cout << request->url<<"Render File-->" << request->url.substr(1, request->url.size()-1);
    std::string file = request->url.substr(1, request->url.size() - 1);
    auto buffer = readWholeFile(HttpServer::RootPath+"/"+ file);
    auto response = new Response();
    response->body = buffer;
    response->headers["Content-Type"] = "text/javascript; charset=utf-8";
    response->headers["Content-Length"] = std::to_string(buffer.size());
    response->statusCode = 200;
    return response;
}




int main()
{
  
  /*  ComposeHtmlElement meta("p", "THIS IS LAY off");
    meta.addParameter("charset", "utf-sdf8");
    meta.addParameter("provider", "utfs-8");
    meta.addParameter("token", "utf-d8");
    meta.addParameter("id", "utf-8x");
    std::cout << meta.toString();
    */
    return HttpServer(9293)
        .registerNewStaticEndpoint("/", homeDir)
        .registerNewStaticEndpoint("/:id.html", requestAnyHTMLFileinRoot)
        .registerNewStaticEndpoint("/:id.js", requestAnyJSFileinRoot)
        .registerNewStaticEndpoint("/:id.json", requestAnyJSONFileinRoot)
        .registerNewStaticEndpoint("/assets", anyAssets)
        .registerNewStaticEndpoint("/flutter.js", flutterJS)
        .registerNewStaticEndpoint("/assets/:xparams", anyAssetsWithParams)
        .start();
}

#if 0
void test() {
	/*registered_endpoint["/"] = home;
	registered_endpoint["/user/:id/:aneury"] = user;

	Request request;
	request.url = "/user/1/aneury";
	parseRequest(request, nullptr)(&request);*/
}
#endif
