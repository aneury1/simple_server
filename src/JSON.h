#pragma once

#include <string>
#include <vector>
#include "Constant.h"


class JSONObject;


struct JSONArray
{
    std::vector<std::string> elements;

    void add(std::string value);
    void add(float number);
    void add(int   number);
    void add(bool boolean_value);

    std::string toString();
};


struct JsonPair
{
    std::string key;
    std::string value;
    JSON_DATATYPE type;
public:
    JsonPair(std::string key, const char* value);
    JsonPair(std::string key, std::string value);
    JsonPair(std::string key, float number);
    JsonPair(std::string key, int   number);

    JsonPair(std::string key, bool boolean_value);
    JsonPair(std::string key /*json null*/);
    JsonPair(std::string key, JSONArray list_of_values);
    JsonPair(std::string key, JSONObject object);

    std::string toString();
    const std::string getKey()const;
    const std::string getValue() const;
};




class JSONObject
{
    std::vector<JsonPair*> key_value_list;



public:

    JSONObject();

    void addElement(JsonPair* pair);

    JSONObject addElement(std::string key, const char* value);
    JSONObject addElement(std::string key, std::string value);
    JSONObject addElement(std::string key, bool value);
    JSONObject addElement(std::string key, int value);
    JSONObject addElement(std::string key, float value);
    JSONObject addElement(std::string key);

    std::string stringify();

};