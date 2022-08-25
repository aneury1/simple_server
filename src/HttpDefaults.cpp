#include <iostream>
#include "Utils.h"

#include "HttpDefaults.h"

void http_send_static_html_file(HttpRequest query_request) {

    string response_buffer;
    response_buffer += "HTTP/1.1 200 OK\r\n";
    response_buffer += "Content-Type: text/html\r\n";
    string body_response;
    body_response = readFile("D:\\SimpleTransaction\\chfrontend\\src\\test.html");
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

void http_send_static_mp3_file(HttpRequest query_request) {


    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    std::cout << "mp3 file\n";
    string response_buffer;
    response_buffer += "HTTP/1.1 200 OK\r\n";
    response_buffer += "Content-Type: audio/mpeg3\r\n";
    string body_response;
    body_response = readFile("C:\\Users\\Aneury\\Downloads\\Davide Gatti - Metallyc.mp3");
    response_buffer += "Content-Length:" + std::to_string(body_response.length());
    response_buffer += "\r\n\r\n";
    response_buffer += body_response.c_str();
    int rc = send(query_request.client_connection, response_buffer.c_str(), response_buffer.size(), 0);
}