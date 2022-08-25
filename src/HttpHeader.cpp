#include "HttpHeader.h"
using std::string;


HttpHeader::HttpHeader(string bulk)
{
    int find_1 = bulk.find(":", 0);
    int find_2 = bulk.find("\0", 0);
    if (find_1 != string::npos)
    {
        this->key = bulk.substr(0, find_1);
        find_1++;
        this->value = bulk.substr(find_1, find_2 - find_1);
    }
}
HttpHeader::HttpHeader(string key, string value)
{
    this->key = key;
    this->value = value;
}
string HttpHeader::toString()
{
    return (key + ":" + value + "\r\n");
}