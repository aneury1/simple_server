#include "Utils.h"
#include<fstream>


std::string Utils::ReadFile(const char* src)
{
	std::ifstream f("file.txt");
	if (f)
	{
		std::string str((std::istreambuf_iterator<char>(f)),
			std::istreambuf_iterator<char>());
		// Operations on `str`...

		return str;
	}

	return std::string("{\"status\":\"Invalid file\"}");
}