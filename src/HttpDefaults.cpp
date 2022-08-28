#include <iostream>
#include "Utils.h"

#include "HttpDefaults.h"

void http_send_static_html_file(HttpRequest query_request) {

    string response_buffer;
    response_buffer += "HTTP/1.1 200 OK\r\n";
    response_buffer += "Content-Type: text/html\r\n";
    string body_response;
    body_response = readFile("D:\\c++\\AHTTPServer2\\page\\test.html");
    response_buffer += "Content-Length:" + std::to_string(body_response.length());
    response_buffer += "\r\n\r\n";
    response_buffer += body_response.c_str();
    int rc = send(query_request.client_connection, response_buffer.c_str(), response_buffer.size(), 0);
}

void http_send_static_html_file_favicon(HttpRequest query_request) {

    string response_buffer;
    response_buffer += "HTTP/1.1 200 OK\r\n";
    response_buffer += "Content-Type: text/html\r\n";
    string body_response;
    body_response = readFile("D:\\SimpleTransaction\\chfrontend\\public\\favicon.ico");
    response_buffer += "Content-Length:" + std::to_string(body_response.length());
    response_buffer += "\r\n\r\n";
    response_buffer += body_response.c_str();
    int rc = send(query_request.client_connection, response_buffer.c_str(), response_buffer.size(), 0);
}


void http_send_static_file(HttpRequest query_request) {
     
    string response_buffer;
    response_buffer += "HTTP/1.1 200 OK\r\n";
    response_buffer += "Content-Type: plain/text\r\n";
    string body_response;
    body_response =  readFile("D:\\SDKS\\flutter_windows_3.0.5-stable\\flutter\\README.md");
    response_buffer += "Content-Length:" + std::to_string(body_response.length());
    response_buffer += "\r\n\r\n";
    response_buffer += body_response.c_str();
    int rc = send(query_request.client_connection, response_buffer.c_str(), response_buffer.size(), 0);
}
#include <sstream>
void http_send_static_mp3_file(HttpRequest query_request) {




    std::cout << "mp3 file\n";
    std::string response_buffer;
    response_buffer += "HTTP/1.1 200 OK\r\n";
    response_buffer += "Content-Type: audio/mpeg3\r\n";
    auto body_response = readBinaryFile("D:\\c++\\AHTTPServer2\\page\\test.mp3");
   
   
    
    printf("C %ld\n\n", body_response.size());
   // response_buffer += "Content-Length:" + std::to_string(body_response.size());
    response_buffer += "\r\nTransfer-Encoding: chunked";
    response_buffer += "\r\n\r\n";
    ///response_buffer += (char *)body_response.data();
    
   int rc = send(query_request.client_connection, response_buffer.c_str(), response_buffer.size(), 0);
   
   
   int nCount = body_response.size();
    int pos = 0;
     rc = send(query_request.client_connection, response_buffer.c_str(), response_buffer.size(), 0);
    printf("Send Headers =%ld", rc);
    int total_sent=0;
    while (nCount > 0)
    {
        std::stringstream chunks;
        chunks << 1 << "\r\n" << body_response[pos] <<'\r\n';
        rc = send(query_request.client_connection, chunks.str().c_str(), chunks.str().size(), 0);
        if (rc == -1)
        {
            break;
        }
        total_sent += rc;
        

        pos++;
        nCount -= 1;
        printf("Sent => %ld, total sent=%ld, expected =%ld\n", rc, total_sent, body_response.size());
    }
    send(query_request.client_connection, "0\r\n\r\n", 5, 0);
    printf("Finish to Transfer");
}