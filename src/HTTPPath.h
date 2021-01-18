#pragma once
#pragma once
#include <vector>
#include <string>
using std::string;
using std::vector;


////\this structure parse the path send by requester or the response object.
struct HTTPPath
{
   string path;
   vector<string>path_parsed;
   int foward_slashed_number;
  
    HTTPPath(string path);
};