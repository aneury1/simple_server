#ifndef _DEFINES_H_DEFINED
#define _DEFINES_H_DEFINED
#include <vector>
#include <string>
#include <unordered_map>
#include <stdint.h>
#include <functional>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>

constexpr int max_connection_expected = 1000;
constexpr int SO_MAXCON = 10;
typedef std::unordered_map<std::string,std::string> string_map;
typedef std::vector<uint8_t> byte_buffer;
typedef std::function<void(int&)> tcp_connection_handler;
typedef std::function<void(int&, const int)> tcp_connection_event_handler;

enum class tcp_handler_status{
   Ok,
   Error
};


#endif //_DEFINES_H_DEFINED
