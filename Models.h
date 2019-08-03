#pragma once
#include <vector>
#include <string>
using std::string;
using std::vector;
// #include <sqlite3ext.h>
///#include <sqlite3.h>

struct sqlite3{};

enum MODEL_TYPE{INTEGER_TYPE, FLOAT_TYPE, BOOLEAN_TYPE, STRING_TYPE, CHAR_TYPE, BLOB_TYPE, INVALID_TYPE};

struct ContentValues
{
   virtual string getFieldName(int index)=0;
   virtual MODEL_TYPE getFieldType(int index)=0;
   virtual const int getFieldNumbers()=0;
   virtual void setFieldValue(int index, string data)=0;
   virtual int getFieldWithTheName(const char *name)=0;
   virtual bool isPrimaryKey(int index) =0;

};


class ConnectionHandler
{
    protected:
    std::string connection_string;
    public:
      ConnectionHandler(const char *str){
          connection_string = str;
      }

    virtual int RawQuery(string query , ContentValues *values) = 0;
    virtual int RawInsert(string insert)=0;
   
};




