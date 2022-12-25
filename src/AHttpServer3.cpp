#include <string>
#include <sstream>
#include <cstdint>
#include "HttpServer.h"
#include "HttpDefaults.h"
#include "ASqlWrapper.h"

#include <thread>
using std::thread;

class Executors {

};

class ScheduledThreadPoolService {
	std::vector<std::thread> threads;
public:

};






int main() {

  
	 DBTable *table = create_table("LOGS", true);
	 table = add_column(table, "IDD", 1, " ");
	 table = add_column(table, "TEXT", 1, " ");
	 table = add_column(table, "VALUE", 1, " ");
	 table = add_column(table, "IDTABLE", 1, " ");

    
	  

	/// printf("%ld", CountColumn(table));
	 char* bff = generate_create_table_for_sqlite(table);
	 printf("SQL =%s ", bff);
	 delete[]bff;
	 bff = nullptr;

	 for (;;);


#if 0


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
#endif	 

	return 0;
}
