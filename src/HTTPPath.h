#pragma once
#pragma once
#include <vector>
#include <string>
using std::string;
using std::vector;

typedef std::string String;

////\this structure parse the path send by requester or the response object.
struct HTTPPath
{
   String path;
   vector<String>path_parsed;
   int foward_slashed_number;
  
    HTTPPath(String path);
};