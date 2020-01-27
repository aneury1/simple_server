#include "JSON.h"
#include <sstream>

namespace implementation{

template<typename T>
std::string toString( T v){
   std::stringstream ss; 
   ss<< v;
   return ss.str();
}
};

void JSONArray::add( std::string value )
{
    
}
void JSONArray::add( float number){}
void JSONArray::add( int   number){}
void JSONArray::add( bool boolean_value){}

std::string JSONArray::toString()
{
   return "[9,2,45,true]";
}
#include <iostream>

 JsonPair::JsonPair( std::string key, const char * value )
{
  ////std::cout << "String\n";
  this->key   = key;
  this->value = !value?"null":std::string(value);
  this->type  = !value?JSON_NULL:JSON_STRING;
}

JsonPair::JsonPair( std::string key, std::string value )
{
 //// std::cout << "String\n";
  this->key   = key;
  this->value = value;
  this->type  = JSON_STRING;
}
JsonPair::JsonPair( std::string key, float number)
{
  ////  std::cout << "nUMBER\n";
  this->key   = key;
  this->value = implementation::toString<float>(number);
  this->type  = JSON_NUMBER;
}
JsonPair::JsonPair( std::string key, int   number)
{
  ///// std::cout << "nUMBER\n";  
  this->key   = key;
  this->value = implementation::toString<int>(number);
  this->type  = JSON_NUMBER;
}
JsonPair::JsonPair( std::string key, JSONArray list_of_values)
{
   ////  std::cout << "OBJECT\n"; 
  this->key   = key;
  this->value = list_of_values.toString();
  this->type  = JSON_ARRAY;
}
JsonPair::JsonPair( std::string key, bool boolean_value)
{
  ////std::cout << "BOOLEAN\n";
  this->key   = key;
  this->value = boolean_value?"true":"false";
  this->type  = JSON_BOOLEAN;
}
JsonPair::JsonPair( std::string key /*json null*/)
{
   //// std::cout << "null\n";
  this->key   = key;
  this->value = "null";
  this->type  = JSON_NULL;
}

std::string JsonPair::toString()
{
   std::string ret;
   ret +="\"";
   ret += this->key;
   ret +="\":";
   if(type==JSON_STRING)
   {
    /// std::cout <<"Converting to String this string \n";
     ret +="\"";
     ret += this->value;
     ret +="\"";
   }
   else
   {
       ret += this->value;
   }
   return ret;
}
const std::string JsonPair::getKey()const
{
  return key;
}    
const std::string JsonPair::getValue() const
{
   return value;
} 

JsonPair::JsonPair( std::string key, JSONObject object)
{
  this->key   = key;
  this->value = object.stringify();
  this->type  = JSON_OBJECT;
}



 
JSONObject::JSONObject()
{
}

void JSONObject::addElement(JsonPair* pair)
{
    this->key_value_list.emplace_back(pair);
}


JSONObject JSONObject::addElement(std::string key, const char *value)
{
    this->addElement(new JsonPair(key, value));
    return (*this);
}

JSONObject JSONObject::addElement(std::string key, std::string value)
{
    this->addElement(new JsonPair(key, value));
    return (*this);
}
JSONObject JSONObject::addElement(std::string key, bool value)
{
    this->addElement(new JsonPair(key, value));
    return (*this);
}
JSONObject JSONObject::addElement(std::string key, int value)
{
    this->addElement(new JsonPair(key, value));
    return (*this);
}
JSONObject JSONObject::addElement(std::string key, float value)
{
    this->addElement(new JsonPair(key, value));
    return (*this);
}
JSONObject JSONObject::addElement(std::string key)
{
    this->addElement(new JsonPair(key));
    return (*this);
}









std::string JSONObject::stringify()
{
    std::string ret;
    if(key_value_list.size()<0)
    {
        ret+="{ }";
    }
    else
    {
        int iter =0;
        ret+="{ \n\r";
        while(iter != key_value_list.size())
        {
             ret += key_value_list[iter]->toString();
             iter++;
             if(iter== key_value_list.size())
               break;
             else
               ret+=",";
             
        }
    }
    ret+="\r\n}";
    return ret;
}