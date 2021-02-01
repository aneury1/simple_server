#include "HttpRequest.h"
#include <ctype.h>

bool compare_ignore_case(string s1, string s2)
{
    if(s1.size()!=s2.size())return false;

    if(s1==s2)return true;
    
    for(int i=0;i<s1.length();i++)
    {
        char c1=tolower(s1[i]), c2=tolower(s2[i]);
        if(c1!=c2)return false;
    }
    return true;
}

HttpHeader::HttpHeader(string bulk)
{

   int find_1 = bulk.find(":",0);

   int find_2 = bulk.find("\0", 0);

   if(find_1!= string::npos)
   {
       this->key =bulk.substr(0, find_1);
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
  return (key+":"+value+"\r\n");
}

HTTPRequest::HTTPRequest(const char *str)
{
    if(!str)return;

    string string_v=str;

    int find_1 = string_v.find(" ", 0);
    int find_2 =-1;

    if(find_1!=string::npos)
    {
         string tmp = string_v.substr(0, find_1);
            if(compare_ignore_case(tmp,"GET"))
           {
               this->verb =HTTP_VERB::HTTP_GET;
           }
           else if(compare_ignore_case(tmp,"POST"))
           {
                this->verb =HTTP_VERB::HTTP_POST;
           }
           else if(compare_ignore_case(tmp,"PUT"))
           {
                this->verb =HTTP_VERB::HTTP_PUT;
           }
           else if(compare_ignore_case(tmp,"OPTIONS"))
           {
                this->verb =HTTP_VERB::HTTP_OPTIONS;
           }
           else
           {
               this->verb =HTTP_VERB::HTTP_INVALID_VERB;
           }
    } 
    else
    {
        return ;
    }
    
    find_1++; 

    find_2 = string_v.find(" ", find_1); 

    if(find_1>=find_2)
    {
        path ="INVALID PATH";
        return ;
    }

    this->path = string_v.substr(find_1, find_2 - find_1);

    this->http_path = new HTTPPath(path);

    find_1 = string_v.find("\r\n", 0);

    if(find_1 != string::npos)
    {
         find_1+=2;
         int iter=0;
         while(1)
         {
            iter =  string_v.find("\r\n", find_1);
            if(iter!=string::npos)
            {
                  string tmp = string_v.substr(find_1, iter - find_1);
                  if(tmp[0]=='\r'&& tmp[1=='\n'])break;
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
    
    if(find_1!=string::npos)
    {
        find_1+=4;
    
        if(find_1<string_v.length())
        {
            this->body = string_v.substr(find_1, string_v.length()- find_1);
        }
    }
}
