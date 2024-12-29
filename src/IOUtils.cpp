#include "IOUtils.h"
#include <arpa/inet.h> // inet_ntoa
#include <array>
#include <cstring>   // memset
#include <ifaddrs.h> // getifaddrs
#include <iostream>
#include <memory>
#include <netinet/in.h> // sockaddr_in
#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h> // close

std::string ReplaceAll(std::string str, const std::string &from,
                       const std::string &to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos +=
        to.length(); // Handles case where 'to' is a substring of 'from'
  }
  return str;
}

std::string readWholeFile(std::string src) {
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

std::map<std::string, std::string> getAddress() {
  struct ifaddrs *ifAddrStruct = nullptr;
  struct ifaddrs *ifa = nullptr;
  void *tmpAddrPtr = nullptr;
  std::map<std::string, std::string> ret;
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
      tmpAddrPtr = &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr;

      char addressBuffer[INET_ADDRSTRLEN];
      inet_ntop(AF_INET, tmpAddrPtr, addressBuffer, INET_ADDRSTRLEN);
      if (!ret[ifa->ifa_name].length() != 0) {
        ret[ifa->ifa_name] = std::string(addressBuffer);
      } else {
        ret[ifa->ifa_name + std::to_string(++cnt)] = std::string(addressBuffer);
      }
      std::cout << "Interface: " << ifa->ifa_name
                << "\tIPv4 Address: " << addressBuffer << std::endl;
    }

    // Check if it is an IPv6 address (optional)
    else if (ifa->ifa_addr->sa_family == AF_INET6) {
      tmpAddrPtr = &((struct sockaddr_in6 *)ifa->ifa_addr)->sin6_addr;

      char addressBuffer[INET6_ADDRSTRLEN];
      inet_ntop(AF_INET6, tmpAddrPtr, addressBuffer, INET6_ADDRSTRLEN);

      if (!ret[ifa->ifa_name].length() != 0) {
        ret[ifa->ifa_name] = std::string(addressBuffer);
      } else {
        ret[ifa->ifa_name + std::to_string(++cnt)] = std::string(addressBuffer);
      }

      std::cout << "Interface: " << ifa->ifa_name
                << "\tIPv6 Address: " << addressBuffer << std::endl;
    }
  }

  // Free the memory allocated by getifaddrs
  if (ifAddrStruct != nullptr) {
    freeifaddrs(ifAddrStruct);
  }

  return ret;
}
std::string catCMD(std::string path, std::string desc) {
  const char *command = std::string("cat " + path).c_str();
  std::array<char, 1024> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);
  if (!pipe) {
    return "Failed to run the sequence LS";
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }

  if (result.size() <= 0)
    result += "Information seems to be not available...\r\n";

  return desc + result;
}

std::string getPortLS() {
  const char *command = "ss -tulnp";
  std::array<char, 512> buffer;
  std::string result;
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(command, "r"), pclose);
  if (!pipe) {
    return "Failed to run the sequence LS";
  }
  while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
    result += buffer.data();
  }
  return "Open Ports and Associated Processes:\n" + result;
}

std::string catCMD(std::string cmd) { std::stringstream stream; }