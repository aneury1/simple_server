/*
Sample Endpoint v0.1
std::atomic_int counter(0);
Response *home(Request *request){
    std::string text;
    text +="<b style=\"color:red\">";
    text += std::to_string(counter);
    text +="</b>";
    counter+=1;
    auto response = new Response();
    response->body = text;
    response->headers["Content-Type"] = "text/html";
    response->headers["Content-Length"] = std::to_string(response->body.size());
    response->statusCode = 200; 
    return response;
}
-
-Response *homeDir(Request *request)
-{
-    for(auto it : request->parameters){
-        std::cout << "Parameter: " << it.first <<" : "<< it.second <<"\n";
-    }
-    auto response = new Response();
-    response->body = "Simple Text";
-    response->headers["Content-Type"] = "text/html";
-    response->headers["Content-Length"] = std::to_string(response->body.size());
-    response->statusCode = 200; 
-    return response;
-}
-
-Response *flutterJS(Request *request)
-{
-    auto buffer = readWholeFile("/home/aneury1/project/testbed/build/web/flutter.js");
-    auto response = new Response();
-    response->body = buffer;
-    response->headers["Content-Type"] = "text/javascript; charset=utf-8";
-    response->headers["Content-Length"] = std::to_string(buffer.size());
-    response->statusCode = 200;
-    return response;
-}
-
-Response *anyAssets(Request *request)
-{
-    auto response = new Response();
-    response->body = "this should be anyAssets";
-    response->headers["Content-Type"] = "text/html";
-    response->headers["Content-Length"] = "0";
-    response->statusCode = 400;
-    return response;
-}
-
-Response *anyAssetsWithParams(Request *request)
-{
-    auto response = new Response();
-    response->body = "this should be anyAssets With Params";
-    response->headers["Content-Type"] = "text/html";
-    return response;
-}
-
-Response *requestAnyJSFileinRoot(Request *request)
-{
-    std::string file = request->url.substr(1, request->url.size() - 1);
-    auto buffer = readWholeFile(HttpServer::RootPath+"/"+ file);
-    auto response = new Response();
-    response->body = buffer;
-    response->headers["Content-Type"] = "text/javascript; charset=utf-8";
-    response->headers["Content-Length"] = std::to_string(buffer.size());
-    response->statusCode = 200;
-    return response; ///comment
-}
-
-Response *requestAnyHTMLFileinRoot(Request *request)
-{
-    std::string file = request->url.substr(1, request->url.size() - 1);
-    auto buffer = readWholeFile(HttpServer::RootPath+"/"+ file);
-    auto response = new Response();
-    response->body = buffer;
-    response->headers["Content-Type"] = "text/html";
-    response->headers["Content-Length"] = std::to_string(buffer.size());
-    response->statusCode = 200;
-    return response;
-}
-
-Response *requestAnyJSONFileinRoot(Request *request)
-{
-    std::string file = request->url.substr(1, request->url.size() - 1);
-    auto buffer = readWholeFile(HttpServer::RootPath+"/"+ file);
-    auto response = new Response();
-    response->body = buffer;
-    response->headers["Content-Type"] = "text/javascript; charset=utf-8";
-    response->headers["Content-Length"] = std::to_string(buffer.size());
-    response->statusCode = 200;
-    return response;
-}
-
-Response *apiWareTest(Request *request)
-{
-    std::string file = request->url.substr(1, request->url.size() - 1);
-    auto buffer = std::string("PRUEBA");
-    
-    for(auto re : request->parameters){
-        buffer+="\n";
-        buffer+="Key: "+ re.first;
-        buffer+= "Value: "+ re.second;
-        buffer+="\n";
-    }
-    
-    auto response = new Response();
-    response->body = buffer;
-    response->headers["Content-Type"] = "text/javascript; charset=utf-8";
-    response->headers["Content-Length"] = std::to_string(buffer.size());
-    response->statusCode = 200;
-
-    return response;
-}
-
-int main(int argc, char *argv[])
-{
-    return HttpServer(9293)
-        .registerNewStaticEndpoint("/", homeDir)
-        .registerNewStaticEndpoint("/cliente/:id/:nombre/orden", apiWareTest)
-        .registerNewStaticEndpoint("/:id.html", requestAnyHTMLFileinRoot)
-        .registerNewStaticEndpoint("/:id.js", requestAnyJSFileinRoot)
-        .registerNewStaticEndpoint("/:id.json", requestAnyJSONFileinRoot)
-        .registerNewStaticEndpoint("/assets", anyAssets)
-        .registerNewStaticEndpoint("/flutter.js", flutterJS)
-        .registerNewStaticEndpoint("/assets/:xparams", anyAssetsWithParams)
-        .start();
-}

*/
// Must include the gtest header to use the testing library
#include <gtest/gtest.h>

namespace {
  // We will test this dummy function but you can test
  // any function from any library that you write too.
  int GetMeaningOfLife() {  return 42; }
}

// All tests must live within TEST* blocks
// Inside of the TEST block is a standard C++ scope
// TestTopic defines a topic of our test, e.g. NameOfFunctionTest
// TrivialEquality represents the name of this particular test
// It should be descriptive and readable to the user
// TEST is a macro, i.e., preprocessor replaces it with some code
TEST(TestTopic, TrivialEquality) {
  // We can test for equality, inequality etc.
  // If the equality does not hold, the test fails.
  // EXPECT_* are macros, i.e., also replaced by the preprocessor.
  EXPECT_EQ(GetMeaningOfLife(), 42);
}

TEST(TestTopic, MoreEqualityTests) {
  // ASSERT_* is similar to EXPECT_* but stops the execution
  // of the test if fails.
  // EXPECT_* continues execution on failure too.
  ASSERT_EQ(GetMeaningOfLife(), 42) << "Oh no, a mistake!";
  EXPECT_FLOAT_EQ(23.23F, 23.23F);
}
