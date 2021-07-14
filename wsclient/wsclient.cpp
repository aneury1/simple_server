#include "TCPClient.h"
#include "HttpData.h"

#include <iostream>

int main()
{
    TCPClientSender::connect_to("chiripeando.com", 80);
    auto http = HTTPData::getHttpObject();
    http.setVerb(HTTPData::HttpVerb::Get);
    
    char result[3000]={0};
    auto payload = http.getPayload();
    TCPClientSender::sendPacket(payload.c_str(), payload.length());
    TCPClientSender::receivedPacket(result, 3000);

    std::cout << result;
    return 0;
}