#include "IOUtils.h"
#include <iostream>

std::string readWholeFile(std::string src){
  
    std::fstream stream(src, std::ios::in);
    if(stream.is_open())
    {
       std::stringstream content;
       std::string line;
       while(std::getline(stream, line))
            content << line;
       return content.str();
    }

    return "THIS FILE COULD NOT BE OPENED ." + src;
}