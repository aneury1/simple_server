#pragma once

#include "Models.h"



class Sqlite3ConnectionHandler : public ConnectionHandler
{

   protected:
   
   sqlite3 *db;

   private:
   ContentValues *local_value;

   int query_callback(void *NotUsed, int argc, char **argv,  char **azColName);

   public:
    
    Sqlite3ConnectionHandler(const char *db_path);

     
    virtual int RawQuery(string query, ContentValues *values)override;
    
    virtual int RawInsert(string insert)override;

    
};