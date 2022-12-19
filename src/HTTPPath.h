#pragma once
#include <vector>
#include <string>

#include "Router.h"

using std::string;
using std::vector;

struct HTTPPath
{
    string path;
    vector<string>path_parsed;
    int foward_slashed_number;

    HTTPPath(string path);

   /// Route convertToRoute();

};



/// <summary>
///  types[ <int:id> <str:id> <obj:id::obj> ] 
/// </summary>

struct HttpPath {
    std::string current;
    std::vector<HttpPath> children;
    std::string convert_to_type();
};






