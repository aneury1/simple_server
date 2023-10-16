

#include "HttpServer.h"
#pragma comment(lib, "ws2_32.lib")

int main() {

	return HttpServer(9293).start();
}


#if 0
void test() {
	/*registered_endpoint["/"] = home;
	registered_endpoint["/user/:id/:aneury"] = user;

	Request request;
	request.url = "/user/1/aneury";
	parseRequest(request, nullptr)(&request);*/
}
#endif