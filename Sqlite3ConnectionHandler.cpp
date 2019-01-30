#include "Models.h"
#include "SqliteConnectionHandler.h"
#include <sqlite3.h>
#include <stdio.h>



Sqlite3ConnectionHandler::Sqlite3ConnectionHandler(const char *db_path)
: ConnectionHandler(db_path)
{
   if(db_path==NULL)
   {
       printf("In Memory database is create..\n");
       sqlite3_open(":memory:", &db);
   }
   else
   {
       sqlite3_open(db_path, &db);
   }
   
}

int _callback(void *NotUsed, int argc, char **argv,  char **azColName)
{
   for (int i = 0; i < argc; i++) 
   {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
    
    printf("\n");
 return 0;
}



typedef int (*my_callback)(void*,int,char**,char**);
int
Sqlite3ConnectionHandler::RawQuery(string query, ContentValues *values)
{
   if(db)
   {
      this->local_value = values;
      char *err_msg=nullptr;
      int rc  = sqlite3_exec(db, query.c_str(), _callback, 0, &err_msg);
      if(rc == SQLITE_OK)
      {
             printf("Raw Query done correctly....\n"); 
      }
      return rc;
   }
   return -1;
}




int
Sqlite3ConnectionHandler::RawInsert(string query)
{
   if(db)
   {
      
      char *err_msg=nullptr;
      
      int rc  = sqlite3_exec(db, query.c_str(), _callback, 0, &err_msg);
       
      if(rc == SQLITE_OK)
      {
         printf("Raw Insert done correctly....\n");      
      }
      else
      {
         printf("Error in insertion %s\n", err_msg);
      }
      return rc;
   }
   return -1;
}

 

int Sqlite3ConnectionHandler::query_callback(void *NotUsed, int argc, char **argv,  char **azColName)
{
   for (int i = 0; i < argc; i++) 
   {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
    
    printf("\n");
 return 0;
}



