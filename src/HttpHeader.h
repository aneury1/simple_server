#pragma once
#include <string>


struct HttpHeader {
    std::string key;
   
    std::string value;
    
    HttpHeader(std::string bulk);
    
    HttpHeader(std::string key, std::string value);
    
    std::string toString();
};
