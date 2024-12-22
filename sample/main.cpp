#include <memory>
#include <string>
#include <sstream>
#include <iostream>
#include "HttpServer.h"
#include "HttpPrimitiveUtils.h"
#include "IOUtils.h"





extern std::string getVersion();
int main(int argc, char *argv[]){
   
   auto server = std::make_shared<HttpServer>(9091);
   
   server->endpoint("/index", [](Request *req)->Response*{
      auto ret = new Response(); 
    
      std::stringstream stream;
      auto addresses =  getAddress();
      for(auto it : addresses){
        stream << "Interface: " << it.first << "\t Address: " << it.second<< std::endl;
      }
      stream <<"\n"<<getPortLS();
      stream << "\n"<<catCMD("/proc/meminfo","Mem Info:\n");
      stream << "\n"<<catCMD("/proc/cgroups","c-groups:\n");
      stream <<"\n"<<catCMD("/proc/modules", "Kernel Modules");
      stream <<"\n"<<catCMD("/etc/fstab", "fstab");
      stream <<"\n"<<catCMD("/etc/networks", "network");
      stream <<"\n"<<catCMD("/etc/network", "network");
      ret->body = stream.str();
      ret->headers["Content-Type"]="text/plain";
      ret->headers["Content-Length"]=std::to_string(ret->body.length());
      return ret;
   });
    
   return server->start();
}
