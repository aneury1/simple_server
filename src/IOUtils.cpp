#include "IOUtils.h"
#include <ifaddrs.h>      // getifaddrs
#include <netinet/in.h>   // sockaddr_in
#include <arpa/inet.h>    // inet_ntoa
#include <sys/types.h>
#include <sys/socket.h>
#include <cstring>        // memset
#include <unistd.h>       // close
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


std::map<std::string, std::string> getAddress(){
  struct ifaddrs* ifAddrStruct = nullptr;
    struct ifaddrs* ifa = nullptr;
    void* tmpAddrPtr = nullptr;
    std::map<std::string,std::string> ret;
    // Get the list of network interfaces
    if (getifaddrs(&ifAddrStruct) == -1) {
        std::cerr << "Error: getifaddrs failed." << std::endl;
        return ret;
    }
    int cnt = 0;
    std::cout << "Network Interfaces and IP addresses:" << std::endl;

    // Loop through the linked list of interfaces
    for (ifa = ifAddrStruct; ifa != nullptr; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr == nullptr) {
            continue; // Skip if no address
        }

        // Check if it is an IPv4 address
        if (ifa->ifa_addr->sa_family == AF_INET) {
            tmpAddrPtr = &((struct sockaddr_in*)ifa->ifa_addr)->sin_addr;

            char addressBuffer[INET_ADDRSTRLEN];
            inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
             if(!ret[ifa->ifa_name].length()!=0)
            {
                ret[ifa->ifa_name] = std::string(addressBuffer);
            }   
            else{
                ret[ifa->ifa_name+std::to_string(++cnt)] = std::string(addressBuffer);
            }
            std::cout << "Interface: " << ifa->ifa_name << "\tIPv4 Address: " << addressBuffer << std::endl;
        }

        // Check if it is an IPv6 address (optional)
        else if (ifa->ifa_addr->sa_family == AF_INET6) {
            tmpAddrPtr = &((struct sockaddr_in6*)ifa->ifa_addr)->sin6_addr;

            char addressBuffer[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);
            
            if(!ret[ifa->ifa_name].length()!=0)
            {
             ret[ifa->ifa_name] = std::string(addressBuffer);
            }   
            else{
               ret[ifa->ifa_name+std::to_string(++cnt)] = std::string(addressBuffer);
            }
            
            
            std::cout << "Interface: " << ifa->ifa_name << "\tIPv6 Address: " << addressBuffer << std::endl;
        }
    }

    // Free the memory allocated by getifaddrs
    if (ifAddrStruct != nullptr) {
        freeifaddrs(ifAddrStruct);
    }

    return ret;
}