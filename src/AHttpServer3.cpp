
#include <memory>
#include "HttpServer.h"
#include "HttpDefaults.h"


int main() {

	auto theServer = new HttpServer{ 9991 };
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
