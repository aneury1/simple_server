#ifndef HTMLBUILDER
#define HTMLBUILDER

#include <string>
#include <sstream>
#include <vector>

//\ this is our custom String handler for operation
typedef std::string String;

typedef std::stringstream Stringtream;


struct HTMLParameter
{
   const String name;

   const String value;

   HTMLParameter(String name);

   HTMLParameter(String name, String value);

   String toString();
};

struct CSSStyleSheet{

};

///todo: add event and other stuff
struct HTMLTag
{
   std::vector<HTMLParameter> parameters;   
   std::vector<CSSStyleSheet> style;
   String tagName;
   String content;
   String js;
   HTMLTag *childs;
   
}



#endif