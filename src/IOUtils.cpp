#include "IOUtils.h"
#include <iostream>

std::string ReplaceAll(std::string str, const std::string &from, const std::string &to)
{
    size_t start_pos = 0;
    while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

std::string readWholeFile(std::string src)
{
    std::fstream stream(src, std::ios::in | std::ios::binary);
    if (stream.is_open()) {
        /*std::string res;
       char *ptr = res.data();
       stream.seekg(std::ios::end);
       int len = stream.tellg();
       stream.seekg(std::ios::beg);
       res.reserve(len);
       stream.read(ptr, len);
       std::cout << res <<"CONTETN";*/

        std::stringstream content;
        std::string line;
        while (std::getline(stream, line))
            content << line << "\n";

        //// auto res = ReplaceAll(content.str(), "\n", "\r\n");

        return content.str();
    }

    return "THIS FILE COULD NOT BE OPENED ." + src;
}
