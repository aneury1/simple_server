#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <variant>
#include <memory>

class json_value;
using json_object = std::unordered_map<std::string, std::shared_ptr<json_value>>;
using json_array = std::vector<std::shared_ptr<json_value>>;

class json_value {
public:
    using ValueType = std::variant<std::nullptr_t, bool, double, std::string, json_array, json_object>;

private:
    ValueType value;

public:
    json_value() : value(nullptr) {}
    json_value(bool b) : value(b) {}
    json_value(double d) : value(d) {}
    json_value(const std::string& s) : value(s) {}
    json_value(const json_array& a) : value(a) {}
    json_value(const json_object& o) : value(o) {}

    const ValueType& get() const { return value; }
};


enum class json_token_type {
    curly_open,   // {
    curly_close,  // }
    square_open,  // [
    square_close, // ]
    colon,       // :
    comma,       // ,
    String,      // "..."
    number,      // e.g., 123 or 3.14
    true_literal, // true
    false_literal,// false
    null, // null
    eof    // EOF
};

struct json_token {
    json_token_type type;
    std::string value; // For strings and numbers
    json_token(json_token_type t, const std::string& v = "") : type(t), value(v) {}
};


class json_parser{
   std::string raw_input;
   size_t current_position;
   std::vector<json_token> tokens;
   char get_char();
   char next();
   bool is_eof();
   void parse_it();
   void advance_to(int pos);
   std::string extract_string(const std::string single);
   public:
   
   json_parser(const std::string& str);

   void set_string(const std::string& str);

   const json_value operator[](const json_value key);
   const json_value operator[](const std::string& key);


};