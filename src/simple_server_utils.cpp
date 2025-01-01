#include "simple_server_utils.h"
#include <sstream>
#include <fstream>
#include <string>

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
#include <sys/stat.h>
#include <unistd.h> // close

namespace simple_server {


std::string replace_all(std::string str, const std::string &from,
                       const std::string &to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != std::string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos +=
        to.length(); // Handles case where 'to' is a substring of 'from'
  }
  return str;
}
std::string read_whole_file(std::string src)
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


void print_error_of_descriptor(const socket_connection& fd, int line) {
  switch (fd.current_status) {
    case server_status::CreatingSocket:
      print("Error creating the socket", line);
      break;
    case server_status::BindingSocket:
      print("Error binding the socket", line);
      break;
    case server_status::SettingOptions:
      print("Error setting options the socket", line);
      break;
    case server_status::Listening:
      print("Error listening call the socket", line);
      break;
    case server_status::Accepting:
      print("Error Accepting the client socket", line);
      break;
    case server_status::ErrorOrException:
    default:
      print("Error in the socket", line);
      break;
  }
}

std::vector<String> split_words(String word, char sp) {
  std::vector<String> ret;
  int iter = 0;
  String str;
  while (word[iter] != '\0') {

    if (word[iter] != sp) {
      str += word[iter];
    } else {

      if (!str.empty()) {
        if (word[iter] == sp) {
          ret.push_back(str);
          str = "";
        } else if (word[iter + 1] == sp) {
          str += word[iter];
          ret.push_back(str);
          str = "";
        }
      }
    }
    iter++;
  }
  if (!str.empty()) {
    ret.push_back(str);
  }
  return ret;
};


String to_lower_case_str(const String &str) {
  String ret = str;
  std::transform(ret.begin(), ret.end(), ret.begin(),
                 [](char s) { return std::tolower(s); });
  return ret;
}

String extract_url_with_query_params(String request) {
  int firstSpace = request.find_first_of(" ");
  int findHttpVersion = request.find(" HTTP/1.1");
  String url =
      request.substr(firstSpace + 1, findHttpVersion - (firstSpace + 1));
  return url;
}

String extract_url(String request) {
  int firstSpace = request.find_first_of(" ");
  int findHttpVersion = request.find(" HTTP/1.1");
  String url =
      request.substr(firstSpace + 1, findHttpVersion - (firstSpace + 1));
  if (url.find_first_of('?') != String::npos) {
    return url.substr(0, url.find_first_of("?"));
  }
  return url;
}
 
String extract_body(const String str) {
  int pos = str.find("\r\n\r\n");
  if (pos < str.size())
    ;
  return " ";
  String body = str.substr(pos + 2, str.size() - (pos + 2));
  return body;
}
 
StringMap parse_headers(const String &request) {
  StringMap headers;
  std::istringstream iss(request);
  String line;
 
  std::getline(iss, line);

  while (std::getline(iss, line) && !line.empty()) {
    size_t colonPos = line.find(':');
    if (colonPos != String::npos) {
      String headerName = line.substr(0, colonPos);
      String headerValue = line.substr(colonPos + 2);  
      // Remove any leading or trailing whitespaces from the header value
      headerValue.erase(0, headerValue.find_first_not_of(" \t"));
      headerValue.erase(headerValue.find_last_not_of(" \t") + 1);
      headerValue.erase(0, headerValue.find_first_not_of("\r"));
      headerValue.erase(0, headerValue.find_first_not_of("\n"));
      headerValue.erase(headerValue.find_last_not_of("\r\n") + 1);
      headers[headerName] = headerValue;
    }
  }

  return headers;
}

StringMap parse_url_params(String url) {
  StringMap ret;
  size_t pos = url.find("?");
  if (pos != String::npos) {
    String paramList = url.substr(pos + 1, url.size() - pos);
    auto vars = split_words(paramList, ';');
    for (auto var : vars) {
      auto keyPairs = split_words(var, '=');
      ret[keyPairs[0]] = keyPairs[1];
    }
  }
  return ret;
}
url_data_type get_node_data_type_by_pattern(const String &routeNode) {
  int s = routeNode.size();

  int pos = routeNode.find(":int>");
  if (pos != String::npos)
    return url_data_type::Integer;
  pos = routeNode.find(":str>");
  if (pos != String::npos)
    return url_data_type::Str;
  pos = routeNode.find(":boolean>");
  if (pos != String::npos)
    return url_data_type::Boolean;

  return url_data_type::InvalidUrlDatatype;
}
const String url_data_typeAsStr(const url_data_type &type) {
  switch (type) {
  case url_data_type::Integer:
    return "Integer";
  case url_data_type::Str:
    return "Str";
  case url_data_type::Boolean:
    return "Boolean";
  case url_data_type::InvalidUrlDatatype:
  default:
    return "Invalid";
  }
}

bool isBoolean(String sn) {
  if (sn.find("true") != String::npos ||
      sn.find("false") != String::npos)
    return true;
  return false;
}

bool isInteger(String sn) {
  for (auto it : sn)
    if (!isdigit(it))
      return false;
  return true;
}

url_data_type getDataTypeByValue(String value) {
  if (isInteger(value))
    return url_data_type::Integer;
  if (isBoolean(value))
    return url_data_type::Boolean;
  return url_data_type::Str;
}
bool validate_pattern_by_url(std::vector<String> uri,
                         std::vector<String> pattern) {

  bool found = false;
  auto uIter = uri.begin();
  auto pIter = pattern.begin();
  auto uIterEnd = uri.end();
  while (uIter != uIterEnd) {
    if (*uIter != *pIter) {
      String value = *uIter;
      String pValue = *pIter;
      if (pValue[0] != '<') {
        return false;
      }
      auto ptype = get_node_data_type_by_pattern(pValue);
      auto vType = getDataTypeByValue(value);
      if (ptype != vType)
        return false;
    } else {
    }
    pIter++;
    uIter++;
  }
  return true;
};
 
std::map<std::string, std::string> get_local_ip_address_list() {
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

std::string execute_cat_cmd(std::string path, std::string desc) {
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

std::string get_port_by_tulnp_cmd() {
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

std::string get_working_directory(){
    char cwd[255];
   if (getcwd(cwd, sizeof(cwd)) != NULL) {
       printf("Current working dir: %s\n", cwd);
       return std::string(cwd);
   } else {
       return "get_working_directory: error";
   }
}
bool check_if_path_exist(const std::string& path) {
    struct stat buffer;
    return (stat(path.c_str(), &buffer) == 0);
}
std::string read_assets_from(std::string path, std::string assets_name)
{
   return read_whole_file(path+"/"+assets_name);
}

}  // namespace simple_server