#include <winsock2.h>
#include <windows.h>
#include <iostream>
#include <cstring>
#include <mutex>
#include <thread>
#include <cstdarg>
#include <cstdio>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <io.h>
#include <map>
#include <string>
#include <vector>
#include <cstdio>
std::mutex  mutex;
SOCKET      server_socket;
sockaddr_in server_address;

#ifdef __WIN32__
struct winsock_init{
	winsock_init(){
		WSAData wds;
		WSAStartup(MAKEWORD(2,2), &wds);
        std::cout <<"Winsock init now\n";
	}
   ~winsock_init(){
	   std::cout <<"Winsock clean now\n";
	   WSACleanup();
   }
}init_now;
#endif

struct HTTPRequestHeader
{
private:
   bool valid_header;
   std::string buffer;
   void parse_header();
   void parse_buffer();
   std::string verb;
   std::string path;
   std::string http_version;
   std::map<std::string, std::string>fields;
public:
   HTTPRequestHeader(const char *request);
   ~HTTPRequestHeader();
   std::string getFieldValue(std::string key){return fields[key];}
   std::string getVerb(){return verb;}
   std::string getPath(){return path;}
   std::string getHTTPVersion(){return http_version;}
   std::string getRawBody(){return buffer;}
};

std::vector<std::string> split( const char *buffer, const char *tk)
{
	std::vector<std::string> ret;
	if(buffer && tk)
	{
       int find       =0;
       int last_found =0;
	   //int iter       =0;
	   int length     = std::strlen(tk);
	   static std::string current(buffer);
	   while(1)
	   {
           find = current.find(tk, last_found);
           if((find != std::string::npos)&&(find > last_found))
           {
        	   std::string tmp = current.substr(last_found, find);
        	   last_found = find;
        	   ret.push_back(tmp);
        	   tmp.clear();
        	   last_found+=length;
           }
           else
           {
        	   break;
           }
	   }
	}
	return ret;
}

struct Locker
{
   Locker() {}
};

 void safe_printer(const char *fmt,...)
 {
	 ///! I thought printf use to call flockfile() as said in POSIX.1-2008 but not sure for some reason..
	 std::lock_guard<std::mutex>locker(mutex);
	 va_list p;
	 va_start(p, fmt);
     char __stream[2048]={'\0'};
     vsprintf (__stream, fmt,p);
     printf("%s\n",__stream);
 }

 void print_vector(std::vector<std::string> r)
 {
   for( std::vector<std::string>::const_iterator iter = r.begin(); iter != r.end();iter++)
   {
       safe_printer("%s", iter->c_str());
   }
 }

 void Log_hex_buffer (char *FileName, int LineNum, unsigned char *in_buf, int offset, int num_bytes, bool ascii_rep)
 {
 	int index=0;
 	int len;
 	char temp_buf[128];
 	char disp_buf[128];

 	std::memset(temp_buf, 0x00,sizeof (temp_buf));
 	std::memset(disp_buf, 0x00,sizeof (disp_buf));
 	std::sprintf(temp_buf, "%04X ", offset);
 	strcat (disp_buf, temp_buf);
 	for (index=0; index < num_bytes; ++index)
 	{
 		std::memset (temp_buf, 0x00,sizeof (temp_buf));
 		std::sprintf (temp_buf, "%02X ", in_buf[offset+index]);
 		std::strcat (disp_buf, temp_buf);
 	}
 	if ( ascii_rep == true )
 	{
 		char    c;
 		std::strcat (disp_buf, "- ");
 		len=std::strlen(disp_buf);
 		for (index=0; index < num_bytes; ++index)
 		{
 			c = (in_buf[offset+index] >= 0x20 && in_buf[offset+index] <= 0x7F) ? in_buf[offset+index] : '.';
 			if (c == 0x25)
 			{
 				c = '.';
 			}
  			disp_buf[len+index]=c;
 		}
 	}
 	safe_printer( "%s", disp_buf);
 }



typedef void (*ResponseFunc)(SOCKET s);

void Echo(SOCKET c)
{
	while(1)
	{
		char bgh[2048]={'\0'};
		 int rc= recv(c, bgh, 2048,0);
		 if(rc<=0)
			 break;
		 safe_printer("%s",bgh);
         send(c, bgh, rc, 0);
	}
}



const char* read_file(const char *sm)
{
	Locker lck;
	const char *sub_path="C:\\websites";

	HTTPRequestHeader request(sm);

	char path[2048]={'\0'};
    strcat(path, sub_path);
    strcat(path, "\\");
    //strcat(path, request.getPath() or something.
    strcat(path, "index.html");

    int fd = open(path, O_RDONLY);
	if( fd != -1)
	{
	    char res[4096]={'\0'};
		int fl = lseek(fd, 0, SEEK_END);
		lseek(fd, 0, SEEK_SET);
		read(fd, res, fl);
		const char *stm = res;

		return stm;
	}
	else
	{
		return "HTTP/1.1 200 OK\r\nContent-Type: text/Plain\r\nContent-Length : 5\r\n\r\nhello";
	}
}


void HTTPRequestHeader::parse_header()
{
   verb.clear();
   int i1(0),iter(-1);
   for(int i=0;i<buffer.size();i++)
   {
      if(buffer[i] == ' ')
      {
    	  i1++;
    	  ++i;
      }
      else if(buffer[i] == '\r' && buffer[i+1] == '\n')
      {
    	  iter = i;
    	  std::string temp;
    	  temp = buffer.substr(iter, buffer.length());
    	  buffer.clear();
    	  buffer.assign(temp);
    	  break;
      }

	  switch(i1)
      {
      	  case 0:
      		  verb+=buffer[i];
      		  break;
      	  case 1:
      		  path+=buffer[i];
      		  break;
      	  case 2:
      		  http_version+=buffer[i];
      		  break;
      	  default:
      		  break;
      }
   }
}
//\this will parse header content.
void HTTPRequestHeader::parse_buffer()
{

}

HTTPRequestHeader::HTTPRequestHeader(const char *request):valid_header(false)
{
    if(!request)
    	return;
    buffer.assign(request);
    parse_header();
    parse_buffer();
}
HTTPRequestHeader::~HTTPRequestHeader()
{

}




void http_connection(SOCKET c)
{
	Locker lck;
	const int buffer_size = 4096;
	char bgh[buffer_size]={'\0'};
	int rc= recv(c, bgh, buffer_size,0);
	 if(rc<=0)
		return;
	 safe_printer("\n%s\n",bgh);
	 const char *response = read_file("");
	 safe_printer("\nReponse : \n%s", response);

	 char response_send_info[8096]={'\0'};
	 strcat(response_send_info, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length : ");
	 int length = strlen(response);
	 char len[6]={'\0'};
	 sprintf(len, "%d", length);
	 strcat(response_send_info, len);
	 strcat(response_send_info, "\r\n\r\n");
     strcat(response_send_info, response);
	 send( c, response_send_info, strlen(response_send_info),0);

}





class ResponseThread
{
	std::thread the_thread;
public:
	ResponseThread(ResponseFunc func, SOCKET  s): the_thread(func,s){safe_printer("Client Arrive!!");}
	~ResponseThread(){if(the_thread.joinable())the_thread.join();}
};


int main(int argc, char *argv[])
{

	std::vector<std::string>test= split( "hola\r\ncomo\r\nestas?\r\nesto es una prueba","\r\n");

	print_vector(test);

	/*const int port =9001;
     server_socket = socket(AF_INET, SOCK_STREAM, 0);
     std::memset(&server_address, 0x00, sizeof(server_address));
     server_address.sin_addr.s_addr = INADDR_ANY;
     server_address.sin_port        = htons(port);
     server_address.sin_family      = AF_INET;

     int iOptVal = 1;
     int iOptLen = sizeof (int);

     bind(server_socket, (sockaddr *)&server_address, sizeof(server_address));
     setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR,(char *)&iOptVal, iOptLen );
     listen(server_socket, 30);

     safe_printer("Waiting for connection on port : %d", port);
     while(1)
     {
        SOCKET client = accept(server_socket, nullptr, nullptr);
        if((signed int)client != -1)
        {
        	ResponseThread(http_connection, client );
        	safe_printer("client disconected");
        	shutdown(client, 2);
        }
        else
        {

        }

     }
*/
	return 0;
}
