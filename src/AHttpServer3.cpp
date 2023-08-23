#include <string>
#include <sstream>
#include <cstdint>
#include "HttpServer.h"
#include "HttpDefaults.h"
#include "ASqlWrapper.h"
#include "Utils.h"
#include "HOF.h"


void serve() {

	auto theServer = new HttpServer{ 9091 };
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
}

int main() {
	
	serve();
	return 0;
}
