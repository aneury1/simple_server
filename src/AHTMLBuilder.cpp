#include "AHTMLBuilder.h"
#include <sstream>


std::string HTMLAttribute::toString()
{
	std::stringstream stream;
	stream << "\"" << this->name << "\"";
	if (!this->attribute_value.empty())
	{
		stream << " =" << "\"" << this->attribute_value << "\"";
	}
	return stream.str();
}



std::string HTMLTag::toString()
{
	std::stringstream stream;
	stream << "<" << this->name << " ";
	for (auto it : this->attribute)
		stream << it.toString() << " ";
	if (!this->value.empty())
	{
		stream << ">" << this->value;
		stream << "</" << this->name << ">";
	}
	else
	{
		stream << ">";
		stream << "</" << this->name << ">";
	}
	return stream.str();
}

HTMLTag makeHTMLTag(std::string name, std::string value) 
{
	return makeHTMLTag(name, {}, value);
}

HTMLTag makeHTMLTag(std::string name, std::vector<HTMLAttribute> attribute, std::string value)
{
	HTMLTag tag;
	tag.attribute = attribute;
	tag.name = name;
	tag.value = value;
	return tag;
}



std::string HTMLBuilder::toString() 
{
	std::stringstream stream;

	for (auto it : this->tags)
	{
		stream << it.toString() << "\r\n";
	}

	return stream.str();
}

HTMLBuilder& HTMLBuilder::AddTag(std::string xtag) 
{
	HTMLTag tag = makeHTMLTag(xtag, "");
	this->tags.emplace_back(tag);
	return (*this);
}

HTMLBuilder& HTMLBuilder::AddTag(HTMLTag tag) 
{
	this->tags.emplace_back(tag);
	return (*this);
	return (*this);
}