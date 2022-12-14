#include <string>
#include <sstream>
#include <cstdint>

bool char_is_in_this_group(char ch, std::string s) {
	for (auto it : s)
		if (it == ch)return true;
	return false;
}

bool char_is_printable(char ch) {
	return ch < 32 || ch >= 127;
}


class AJsonParser {

    uint32_t current_offset_position;
	uint32_t token_next_position;
	int32_t  parent_token;



public:
	AJsonParser(std::string raw) {

	}

	enum class AJsonDatatype {
		Undefined,
		Null,
		Object,
		Array,
		String,
		Primitive
	};

	struct AJsonToken {
		AJsonDatatype type;
		int start_pos;
		int end_pos;
		int length;
		bool is_parent;
	};

};


#include "HttpServer.h"
#include "HttpDefaults.h"




int main() {
	
 

	  auto theServer = new HttpServer{9091};
	  theServer
		  ->Get("/favicon.ico", http_send_static_html_file_favicon)
		  ->Get("/file", http_send_static_file)
		  ->Get("/mp3", http_send_static_mp3_file)
		  ->Get("/horse.mp3", http_send_static_mp3_file)
		  ->Get("/html", http_send_static_html_file)
		  ->Start()
		  ->Listen();
		 

	return 0;
}
