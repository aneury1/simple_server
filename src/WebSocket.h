#ifndef WEBSOCKET_DEFINED
#define WEBSOCKET_DEFINED
#include <memory>
#include <string>
#include <vector>
#include "HttpConnection.h"

class AWebSocket {

public:

	AWebSocket& Open(std::shared_ptr<HttpConnection> connection);
	AWebSocket& Open(int code, std::string msg);
	AWebSocket& Ping(std::vector<unsigned char >msg);
	AWebSocket& Pong(std::vector<unsigned char >msg);
	AWebSocket& Message(std::vector<unsigned char >msg);
	AWebSocket& operator = (std::vector<unsigned char >msg);

};



#endif