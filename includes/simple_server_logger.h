#ifndef SIMPLE_SERVER_LOGGER_DEFINED
#define SIMPLE_SERVER_LOGGER_DEFINED
#pragma once

#include <iostream>

namespace simple_server {

template <class T>
void print(const T& t) {
  std::cout << t << "\r\n";
}
template <class T, class I>
void print(const T& t, const I& i) {
  std::cout << t << " " << i << "\r\n";
}

}  // namespace simple_server

#endif