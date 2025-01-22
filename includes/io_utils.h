#ifndef IO_UTILS_DEFINED
#define IO_UTILS_DEFINED
#include "defines.h"
#include "logger.h"
#include <sstream>
#include <string>
#include <vector>
#include <fstream>
#ifdef WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#define closesocket close
#include <arpa/inet.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#endif

inline int received_data(int connection, char *data, int max_length){
  int rcv = recv(connection, data, max_length, 0);
  return rcv;
}

inline std::string received_str(int connection){
  constexpr int default_max_buffer= 2048;
  char buffer[default_max_buffer] = {0x00};
  int l = received_data(connection, buffer, default_max_buffer);
   SM_WARN("RECEIVED FROM SERVER:2-> %ld", l);
  if(l>0){
    std::string ret;
    ret = buffer;
    ///SM_WARN("RETURNING STR: \n\n%s\n\n", buffer);
    return ret;
  }
  return std::string();
}

inline std::vector<std::string> list_dir(std::string path){
  std::vector<std::string> ret;
    struct dirent *entry;
    struct stat statbuf;
    DIR *dir = opendir(path.c_str());
    //printf("\n\n%s\n\n", path.c_str());
    if (dir == NULL) {
        perror("Unable to open directory");
        ret.emplace_back(path);
        return ret;
    }

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
            continue;
        char fullPath[1024];
        snprintf(fullPath, sizeof(fullPath), "%s/%s", path, entry->d_name);
        if (stat(fullPath, &statbuf) == 0 && S_ISDIR(statbuf.st_mode)) {
           /// printf("📁 %s\n", entry->d_name);
         ret.emplace_back(std::string(fullPath));
        }else{
          // printf("file\n");
           ret.emplace_back(std::string(fullPath));
        }
    }
  closedir(dir);
  return ret;
}

std::string read_text_file(std::string src)
{
  std::fstream stream(src, std::ios::in | std::ios::binary);
  if (stream.is_open()) {
    std::stringstream content;
    std::string line;
    while (std::getline(stream, line))
      content << line << "\n";
    return content.str();
  }else{
    return std::string();
  }
 //return "THIS FILE COULD NOT BE OPENED ." + src;
}


static inline std::vector<uint8_t> str_to_byte_array(std::string str){
    std::vector<uint8_t> ret;
    for(auto it : str) ret.emplace_back(it);
    return ret;
}

#endif