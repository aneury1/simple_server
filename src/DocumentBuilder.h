#pragma once 
#include <string>
#include <sstream>
#include <vector>
#include <unordered_map>

struct DocumentParameter {
	std::unordered_map<std::string, std::string> values;
 

	std::string toString() {
		std::stringstream stream;
		for (auto iter : values) {
			stream << iter.first;
			if (iter.second.size() > 0)
				stream << "= \"" << iter.second << "\"";
		}
		return stream.str();
	}
};