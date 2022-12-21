#include <iostream>
#include "Utils.h"
#include "HttpDefaults.h"

#include "third_party/sqlite/sqlite3.h"



void OpenDB() {
    sqlite3* db;
    sqlite3_open("myDB.db", &db);
    sqlite3_close(db);
}


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



#include <Windows.h>

std::string getPrinters()
{
    bool fnReturn;
    DWORD dwNeeded;
    DWORD dwReturneed;
    DWORD dwReturned;
    PRINTER_INFO_1* pInfo;
    
    OpenDB();
    fnReturn = EnumPrinters(
        PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,
        NULL,
        1L,                // printer info level
        (LPBYTE)NULL,
        0L,
        &dwNeeded,
        &dwReturneed);

    if (dwNeeded > 0)
    {
        pInfo = (PRINTER_INFO_1*)HeapAlloc(
            GetProcessHeap(), 0L, dwNeeded);
    }

    if (NULL != pInfo)
    {
        dwReturned = 0;
        fnReturn = EnumPrinters(
            PRINTER_ENUM_LOCAL | PRINTER_ENUM_CONNECTIONS,
            NULL,
            1L,                // printer info level
            (LPBYTE)pInfo,
            dwNeeded,
            &dwNeeded,
            &dwReturned);
    }
    std::stringstream ret;
    if (fnReturn)
    {
        // Review the information from all the printers
        //  returned by EnumPrinters.
        for (DWORD i = 0; i < dwReturned; i++)
        {
            // pThisInfo[i]->pName contains the printer
            //  name to use in the CreateDC function call.
            //
            // When this desired printer is found in the list of
            //  returned printer, set the printerName value to 
            //  use in the call to CreateDC.
            ret << " " << pInfo->pName << " ";
            // printerName = pThisInfo[i]->pName
        }
    }
    return ret.str();
}

static std::vector<std::string> generated_guids;
void http_send_default_printer(HttpRequest query_request) {

//    char buffer[2048] = { 0x00 };
//    DWORD pcchBuffer =2048;
//#include <winspool.h>
//    GetDefaultPrinterW(
//            (LPTSTR)&buffer[0],
//        &pcchBuffer
//    );

    char guid[42] = { 0x00 };
    GenGUIDId(guid);
    std::stringstream stream;
    stream << guid;
    generated_guids.emplace_back(guid);

    string response_buffer;
    response_buffer += "HTTP/1.1 200 OK\r\n";
    response_buffer += "Content-Type: text/html\r\n";
    string body_response;
    body_response = 
    response_buffer += "Content-Length:" + std::to_string(stream.str().size());
    response_buffer += "\r\n\r\n";
    response_buffer += stream.str();
    int rc = send(query_request.client_connection, response_buffer.c_str(), response_buffer.size(), 0);
}


void http_send_default_default_session_guids(HttpRequest query_request) {

 

    char guid[42] = { 0x00 };
    GenGUIDId(guid);
    std::stringstream stream;
    for (auto v : generated_guids)
        stream <<"<p>" << v << "</p>";

    string response_buffer;
    response_buffer += "HTTP/1.1 200 OK\r\n";
    response_buffer += "Content-Type: text/html\r\n";
    string body_response;
    body_response =
        response_buffer += "Content-Length:" + std::to_string(stream.str().size());
    response_buffer += "\r\n\r\n";
    response_buffer += stream.str();
    int rc = send(query_request.client_connection, response_buffer.c_str(), response_buffer.size(), 0);
}