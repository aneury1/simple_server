#pragma once
#include <string>
#include "HttpPath.h"
#include <iostream>
#if 0

enum class RouterType {
	Static,
	Dynamic,
	File
};

struct Route {
	std::string route;
	std::string get() {
		return route;
    }
	virtual RouterType getType()const {
		return RouterType::Static;
	}
};
struct DynamicRoute : public Route{

	DynamicRoute(const char* routeSpec) {
	
		HTTPPath path(routeSpec);

		auto em = path.path_parsed;
		for (auto it : em) {
			std::cout << it << "\n";
		}
	}	

	virtual RouterType getType()const override{
			return RouterType::Dynamic;
	}

};
struct FileRoute : public Route{};

#endif