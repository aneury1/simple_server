#pragma once
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>



std::string readWholeFile(std::string src);

std::map<std::string, std::string> getAddress();

std::string catCMD(std::string path, std::string desc);
std::string getPortLS();


