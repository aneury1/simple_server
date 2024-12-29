#pragma once

#include <iostream>


template<typename T, typename... Args>
void print(const T& v,Args...args){
   std::cout << v;
   ///print(v,args...);
}

struct printerObject{

};