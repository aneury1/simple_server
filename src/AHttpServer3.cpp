#include <string>
#include <sstream>
#include <cstdint>
#include "HttpServer.h"
#include "HttpDefaults.h"
#include "ASqlWrapper.h"



template<typename Type, typename Original>
std::vector<Type> MapTo(std::vector<Original>ctx, Type (*fn)(Original)) {
	std::vector<Type> ret;
	for (auto i : ctx)
		ret.emplace_back(fn(i));
	return ret;
}


template<typename Original>
std::vector<Original> FilterTo(std::vector<Original>ctx, bool (*fn)(Original)) {
	std::vector<Original> ret;
	for (auto i : ctx)
		if(fn(i))ret.emplace_back(i);
	return ret;
}








std::string convert_int_to_string_post_fix(int val) {
	return "Hola " + std::to_string(val * 100);
}

bool is_even(int i) {
	return (i % 2) == 0;
}


int main() {
	 
	std::vector<int> ls{ 1,3,4,5 };
	std::vector<int> toS = FilterTo(ls,is_even);

	for (auto t : toS)
		std::cout << t << "\n";

	for (;;);
	return 0;


	  auto theServer = new HttpServer{9091};
	  theServer
		  ->Get("/favicon.ico", http_send_static_html_file_favicon)
		  ->Get("/file", http_send_static_file)
		  ->Get("/mp3", http_send_static_mp3_file)
		  ->Get("/horse.mp3", http_send_static_mp3_file)
		  ->Get("/html", http_send_static_html_file)
		  ->Get("/printer", http_send_default_printer)
		  ->Get("/guids", http_send_default_default_session_guids)
		  ->Start()
		  ->Listen();
 

	return 0;
}
