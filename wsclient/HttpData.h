#pragma once 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <vector>
#include <unordered_map>
#include <sstream>
#include <string>
#include <stdio.h>


namespace HTTPData
{
    
     enum class HttpVerb{
         Get, Post, Put, Delete,Head
     };

    typedef std::unordered_map<std::string, std::string> HeaderList;

    struct HTTPTransactionData{
        HeaderList headers;
        HttpVerb verb;
        std::string endpoint;
        std::string queryParameter;
        std::string databody;

        void addHeader(std::string title, std::string value){
            if(title.length()>0&&value.length()>0)
               headers[title] = value;
        }
        std::string getHeaderValue(std::string key){
            return headers[key];    
        }
        void setVerb(HttpVerb v){
            verb = v;
        }
        std::string getVerbString(){
            switch(this->verb){
                case HttpVerb::Get:return "GET";
                case HttpVerb::Post:return "POST";
                case HttpVerb::Put:return "PUT";
                case HttpVerb::Delete:return "DELETE";
            }
            return "GET";
        }


        std::string getPayload(){
            std::stringstream stream_result;
            stream_result << getVerbString() <<" "<<(this->endpoint.length()>0?endpoint:"/")<<" HTTP/1.1\r\n"; 

            return stream_result.str();
        }

        void Clear(){

        }
    };

    HTTPTransactionData getHttpObject(){
        static HTTPTransactionData object;
        object.Clear();
        return object;
    }
};