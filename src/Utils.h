#pragma once 
#include <iostream>
#include <string>
#include <fstream>
#include "HttpRequest.h"
#include "HttpResponse.h"

std::string readFile(const char *str)
{
  std::string ret;
  std::ifstream fs(str);
   
  if (fs.is_open() == false)
	  return "NOT FOUND";
  // The function getline returns false if there are no more lines.
	for (std::string str; std::getline(fs, str);) {
	 // Process the line that has been read.
	 ret+= str;
	}
   return ret;
}

std::string getContentTypeString(WellKnowContentType type)
{
	switch (type)
	{
		case WellKnowContentType::Json:
			return "application/json";
		case WellKnowContentType::Xml:
			return "application/xml";
		case WellKnowContentType::PlainText:
			return "text/plain";
		case WellKnowContentType::Html:
			return "text/html";
		case WellKnowContentType::Raw:
		default:
			return "text/plain";
	}
}


static inline
std::string createHttpResponse(int statusCode, std::string message, WellKnowContentType contentType, std::string payload)
{
    string response_buffer;
	response_buffer += "HTTP/1.1";
	response_buffer += std::to_string(statusCode);
	response_buffer += message;
	response_buffer += "\r\n";
	response_buffer += "Content-Type: "; 
	response_buffer += getContentTypeString(contentType);
	response_buffer += "\r\n";
    response_buffer += "Content-Length:" + std::to_string(payload.length());
    response_buffer += "\r\n\r\n";
    response_buffer += payload.c_str();
	return response_buffer;
}


static inline
std::string createHttpResponse(const HttpResponse& response)
{
	string response_buffer;
	response_buffer += "HTTP/1.1";
	response_buffer += std::to_string(getResponseStatusCode(response.status));
	response_buffer += getResponseStringFromStatusCode(response.status);
	response_buffer += "\r\n";
	response_buffer += "Content-Type: ";
	response_buffer += getContentTypeString(response.contentType);
	response_buffer += "\r\n";
	response_buffer += "Content-Length:" + std::to_string(response.payload.length());
	response_buffer += "\r\n\r\n";
	response_buffer += response.payload.c_str();
	return response_buffer;
}


