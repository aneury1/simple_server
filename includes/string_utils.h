#ifndef STRING_UTILS_DEFINED
#define STRING_UTILS_DEFINED
#include <algorithm>
#include <vector>
#include <string>
using std::string;
#include "defines.h"


static inline string replace_all(string str, const string &from,
                       const string &to) {
  size_t start_pos = 0;
  while ((start_pos = str.find(from, start_pos)) != string::npos) {
    str.replace(start_pos, from.length(), to);
    start_pos +=to.length();
  }
  return str;
}


static inline string to_lower_case(string str){
  string ret = str;
  std::transform(ret.begin(), ret.end(), ret.begin(),
                 [](char s) { return std::tolower(s); });
  return ret;
}

static inline std::vector<string> split_words(string word, char sp) {
  std::vector<string> ret;
  int iter = 0;
  string str;
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

#endif ///STRING_UTILS_DEFINED