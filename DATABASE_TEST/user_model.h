#pragma once
#include <cstdint>
#include <string>
using std::string;
#include "Models.h"
#define USER_NAME_LENGTH   252 
#define USER_NAME_PASSWORD 252


struct User : public ContentValues
{
   std::uint32_t user_id; 
   string   user_name;
   string   password;    
   
   User();
   User(uint32_t user_id, string user_name, string password);
   
   
   virtual bool isPrimaryKey(int index){return index==0;}
   virtual string getFieldName(int index);
   virtual MODEL_TYPE getFieldType(int index);
   virtual const int getFieldNumbers();
   virtual void setFieldValue(int index, string data);
   virtual int getFieldWithTheName(const char *name);
};



