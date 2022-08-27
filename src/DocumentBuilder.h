#pragma once 
#include <string>
#include <sstream>
#include <vector>

struct DocumentAttribute {
	std::string key;
	std::string value;
	std::string toString() {
		std::stringstream stream;
		stream << key << " : " << value;
		return stream.str();
	}
	explicit DocumentAttribute(std::string k, std::string v): key{k},value{v}{}
};


class DocumentBuilder {
	virtual DocumentBuilder build() = 0;
	virtual std::string toString() = 0;
};

class HtmlBuilder : public DocumentBuilder{
public:
	virtual DocumentBuilder build() {
		return *this;
	}
	virtual std::string toString() {
		return "";
	}
};



template<class T>
struct Tag {

};

template<>
struct Tag<HtmlBuilder> {
	HtmlBuilder builder;
	std::vector<Tag> children_list;
	unsigned int identation_level;
	std::string title;

	std::string toString() {

	}
};






