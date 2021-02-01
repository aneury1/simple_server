#ifndef SERVERHANDLER_H_DEFINED
#define SERVERHANDLER_H_DEFINED


struct ServerHandler
{
   
   const char *instance_name=nullptr;
  
   ServerHandler(const char *instance_name=nullptr){}
  
   virtual ~ServerHandler(){}
  
   virtual int create_socket(int &sock) = 0;
  
   virtual int handle_thread(int &sock) = 0;
  
   virtual int handle_response(int &sock, void *buffer, int len)=0;
  
   virtual const char *get_instance_name()=0;
  
   virtual void launch()=0;
};


#endif