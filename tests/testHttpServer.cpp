// Must include the gtest header to use the testing library
#include <gtest/gtest.h>
#include "HttpServer.h"
#include "HttpPrimitiveUtils.h"
#include <map>
/*
TEST(PathHandler, SetAndGetCorrectPath){
   HttpServer server(9000);
   server.setRootPath("/www/home");
   EXPECT_EQ(server.getRootPath(), "/www/home");
}

TEST(PathHandler, SetAndGetInvalidPath){
   HttpServer server(9000);
   server.setRootPath("/www/home");
   EXPECT_NE(server.getRootPath(), "/www/home1");
}
*/
TEST(ParseCorrectRoutes, SplitSize2){
 

  const std::string route_to_parse = "/post/<int:post_id>";
  
  auto splitWords = [](std::string word, char sp){
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
   std::string routeToTest("/post/<int:post_id>");
   auto vec = splitWords(routeToTest, '/'); 
   EXPECT_EQ(vec.size(), 2);
}


TEST(ValidateRoutePattern, ValidateRoute){
  const std::string route_to_parse = "/post/<post_id:int>";
  const std::string uri_to_parse = "/post/1";
  std::cout <<"\n\n\n";  
  auto vecPattern = splitWords(route_to_parse, '/');
  auto vecUriToTest = splitWords(uri_to_parse, '/');
  EXPECT_EQ(vecPattern.size(), vecUriToTest.size());
  EXPECT_EQ(validatePatterByUrl(vecUriToTest, vecPattern), true);
}


TEST(ValidateRoutePattern, ValidateValidRoute){

  const std::map<std::string, ParserEndpoint> routeList={
    {"/post/<post_id:int>/ha", DefaultEndpoint},
    {"/post/<post_id:int>/hola", DefaultEndpoint},
    {"/post/<post_id:int>/", DefaultEndpointW200},
    {"/post/<post_id:int>/h", DefaultEndpoint},
    {"/post/<post_id:int>/hla", DefaultEndpoint},
  };
  const std::string uriRequested = "/post/1";

   auto it = getEndpointFromMap(uriRequested,routeList);
   
   EXPECT_EQ(it.first, "/post/<post_id:int>/");

   auto response = it.second(nullptr);

   EXPECT_EQ(response->statusCode, 200);
}


TEST(ValidateRoutePattern, ValidateQueryParameter){
  std::unordered_map<std::string, std::string> params = parseUrlParams("localhost/url/home?id=1;key=2;gusano=fad1c989-dfcd-4b06-bac2-82c98e8530b8");

  EXPECT_EQ(params.size(), 3);
  EXPECT_EQ(params["id"], "1");
  EXPECT_EQ(params["key"], "2");
  EXPECT_EQ(params["gusano"], "fad1c989-dfcd-4b06-bac2-82c98e8530b8");
}


TEST(ValidateRoutePattern, ValidateRoutePatternWithRequestGet){
  
  const std::string path = "/url/home?id=1;key=2;gusano=fad1c989-dfcd-4b06-bac2-82c98e8530b8";
  std::string request = generateStrRequestPaylod(path, RequestVerb::Get);

  std::string httpVerb = extractHttpVerb(request);
  std::string url      = extractUrlWithQueryParams(request);
  auto headers         = parseHeaders(request);
  //auto body            = extractBody(request);


   EXPECT_EQ(httpVerb, "GET");
   EXPECT_EQ(url     , path);
   EXPECT_EQ(headers["Upgrade"]     , "websocket");
   EXPECT_EQ(headers["supper-long-lasting-information-parameter"]     , "258EAFA5-E914-47BE-9B52-6C18FB827A3A");

}


TEST(ValidateRoutePattern, ValidateRoutePatternWithRequestPost){
  
  const std::string path = "/homepath/unveil/aneury1/url/home?id=1;key=2;gusano=fad1c989-dfcd-4b06-bac2-82c98e8530b8";
  std::string request = generateStrRequestPaylod(path, RequestVerb::Post);

  std::string httpVerb = extractHttpVerb(request);
  std::string url      = extractUrlWithQueryParams(request);
  auto headers         = parseHeaders(request);
  //auto body            = extractBody(request);


   EXPECT_EQ(httpVerb, "POST");
   EXPECT_EQ(url     , path);
}


TEST(ValidateRoutePattern, ValidateRoutePatternWithDynamicGetRequest){
  
  const std::string expectedBaseUrl="/url/<id:int>/home";
  const std::string path = "/url/1/home?id=1;key=2;gusano=fad1c989-dfcd-4b06-bac2-82c98e8530b8";
  const std::map<std::string, ParserEndpoint> routeList={
    {"/post/<post_id:int>/ha", DefaultEndpoint},
    {"/post/<post_id:int>/hola", DefaultEndpoint},
    {"/url/<id:int>/home", DefaultEndpointW200},
    {"/post/<post_id:int>/h", DefaultEndpoint},
    {"/post/<post_id:int>/hla", DefaultEndpoint},
  };

  std::string request = generateStrRequestPaylod(path, RequestVerb::Get);

  std::string httpVerb            = extractHttpVerb(request);
  std::string urlDynamicPath      = extractUrlWithQueryParams(request);
  std::string url                 = extractUrl(request);
  auto headers                    = parseHeaders(request);
  std::unordered_map<std::string, std::string> params = parseUrlParams(urlDynamicPath);

  EXPECT_EQ(httpVerb           , "GET");
  EXPECT_EQ(urlDynamicPath     , path);
  EXPECT_EQ(url                , "/url/1/home");


  EXPECT_EQ(params.size(), 3);
  EXPECT_EQ(params["id"], "1");
  EXPECT_EQ(params["key"], "2");
  EXPECT_EQ(params["gusano"], "fad1c989-dfcd-4b06-bac2-82c98e8530b8");


  EXPECT_EQ(headers["Upgrade"]     , "websocket");
  EXPECT_EQ(headers["supper-long-lasting-information-parameter"]     , "258EAFA5-E914-47BE-9B52-6C18FB827A3A");
 
  auto it = getEndpointFromMap(url,routeList);

  EXPECT_EQ(it.first, expectedBaseUrl);

  auto response = it.second(nullptr);

  EXPECT_EQ(response->statusCode, 200);

}