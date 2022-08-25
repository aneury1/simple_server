#include "HttpRequest.h"
#include "Utils.h"


HttpRequest::HttpRequest(const char* str,std::string raw ,SocketServer::SocketFd client_connection)
{
    if (!str)return;

    this->client_connection = client_connection;
   
    string string_v = str;
    this->raw = raw;

    int find_1 = string_v.find(" ", 0);
    int find_2 = -1;

    if (find_1 != string::npos)
    {
        string tmp = string_v.substr(0, find_1);
        if (compare_ignore_case(tmp, "GET"))
        {
            this->verb = HTTP_VERB::HTTP_GET;
        }
        else if (compare_ignore_case(tmp, "POST"))
        {
            this->verb = HTTP_VERB::HTTP_POST;
        }
        else if (compare_ignore_case(tmp, "PUT"))
        {
            this->verb = HTTP_VERB::HTTP_PUT;
        }
        else if (compare_ignore_case(tmp, "OPTIONS"))
        {
            this->verb = HTTP_VERB::HTTP_OPTIONS;
        }
        else
        {
            this->verb = HTTP_VERB::HTTP_INVALID_VERB;
        }
    }
    else
    {
        return;
    }

    find_1++;
    find_2 = string_v.find(" ", find_1);
    if (find_1 >= find_2)
    {
        path = "INVALID PATH";
        return;
    }
    this->path = string_v.substr(find_1, find_2 - find_1);
    this->http_path = new HTTPPath(path);



    find_1 = string_v.find("\r\n", 0);
    if (find_1 != string::npos)
    {
        find_1 += 2;
        int iter = 0;
        while (1)
        {
            iter = string_v.find("\r\n", find_1);
            if (iter != string::npos)
            {
                string tmp = string_v.substr(find_1, iter - find_1);
                if (tmp[0] == '\r' && tmp[1 == '\n'])break;
                headers.emplace_back(tmp);
                find_1 = (iter + 2);
            }
            else
            {
                break;
            }

        }
    }

    find_1 = string_v.find("\r\n\r\n", 0);
    if (find_1 != string::npos)
    {
        find_1 += 4;
        if (find_1 < string_v.length())
        {
            this->body = string_v.substr(find_1, string_v.length() - find_1);
        }
    }
}