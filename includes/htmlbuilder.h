#ifndef HTMLBUILDER_H
#define HTMLBUILDER_H
#include <sstream>
#include <string>
#include <map>
using std::string;
typedef std::stringstream StringStream;
typedef std::string String;
typedef std::map<String, String> Parameter;


struct HtmlElement{
    String tag_name;
    String value;
    Parameter parameter;
    virtual ~HtmlElement(){}
    HtmlElement(String tag, String v): tag_name{tag},value{v}{}
    virtual String toString() = 0;
    void addParameter(String k, String v){
        parameter[k]=v;
    }
};

struct SingleHtmlElement : public HtmlElement{
    SingleHtmlElement(String tag) : HtmlElement{tag, ""}{}
    virtual String toString(){
        /// Todo: append Parameter.
        StringStream stream;
        stream << "<"<<this->tag_name;
        for(auto it : this->parameter){
            stream <<"\""<<it.first<<"\"="<<"\""<<it.second<<"\" ";
        }
        stream.str().pop_back();
        stream <<"/>";
        return stream.str();
    }
};


struct ComposeHtmlElement : public HtmlElement{

    ComposeHtmlElement(String tag, String value):HtmlElement{tag, value}{

    }

    ComposeHtmlElement setBody(String body){
        this->value = body;
        return *this;
    }

    virtual String toString(){
        /// Todo: append Parameter.
        StringStream stream;
        stream << "<"<<this->tag_name;
        for(auto it : this->parameter){
            stream <<it.first<<"=\""<<it.second<<"\" ";
        }
        stream.str().pop_back();
        stream <<">";
        stream <<value << "</"<<this->tag_name<<">";
        return stream.str();
    }

};


struct SimpleElementStyle{
};

class HtmlBuilder
{
public:
    HtmlBuilder();
};

class BootStrap5HtmlBuilder : public HtmlBuilder{


};

class SimpleBootStrapLogin : public HtmlBuilder{

};

#endif // HTMLBUILDER_H
