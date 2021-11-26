#pragma once
#include <string>
#include <vector>

struct HTMLAttribute
{
	std::string name;
	std::string attribute_value;
	std::string toString();
};

struct HTMLTag
{
	std::string name;
	std::vector<HTMLAttribute> attribute;
	std::string value;
	std::string toString();
};

HTMLTag makeHTMLTag(std::string name, std::string value);
HTMLTag makeHTMLTag(std::string name, std::vector<HTMLAttribute> attribute ,std::string value);


class HTMLBuilder
{
	std::vector<HTMLTag> tags;
public:
	std::string toString();
	HTMLBuilder& AddTag(std::string tag);
	HTMLBuilder& AddTag(HTMLTag tag);
};