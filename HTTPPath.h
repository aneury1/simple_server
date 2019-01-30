#pragma once
#pragma once
#include <vector>
#include <string>
using std::string;
using std::vector;
struct HTTPPath
{
   string path;
   vector<string>path_parsed;
   int foward_slashed_number;
  
    HTTPPath(string path);
};