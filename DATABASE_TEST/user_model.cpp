#include "user_model.h"

User::User()
{
  this->user_id = -1;
}

User::User(uint32_t user_id, string user_name, string password)
{
    this->user_id   = user_id;
    this->user_name =user_name;
    this->password  = password;
}
string User::getFieldName(int index)
{
    switch(index)
    {
        case 0: return "user_id";
        case 1: return "user_name";
        case 2: return "password";
        default: return "invalid_index";
    }
}
MODEL_TYPE User::getFieldType(int index)
{
    switch(index)
    {
        case 0: return INTEGER_TYPE;
        case 1: return STRING_TYPE;
        case 2: return STRING_TYPE;
        default: return INVALID_TYPE;
    }
}
const int User::getFieldNumbers()
{
    return 3;
}
void User::setFieldValue(int index, string data)
{
    switch(index)
    {
        case 0: this->user_id = std::stoi(data);return;
        case 1: this->user_name = data; return;
        case 2: this->password = data; return;
    }
}
int User::getFieldWithTheName(const char *name)
{
    if(!name)return -1;
    string str_name=name;
    if(str_name=="user_id")return 0;
    if(str_name=="user_name")return 1;
    if(str_name=="password")return 2;
    return -1;
}