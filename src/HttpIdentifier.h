#pragma once
#include <vector>
#include <string>
#include <map>
#include "Constant.h"
struct HttpIdentifier : public std::pair<std::string, HTTP_VERB>
{
    std::string path;
    HTTP_VERB   verb;
    HttpIdentifier() = delete;
    HttpIdentifier(std::string path, HTTP_VERB verb) :pair(path, verb), path{ path }, verb { verb }  {}
};

