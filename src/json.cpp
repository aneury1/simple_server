#include "json.h"
#include <nlohmann/json.hpp>
#include <sstream>

/// @brief Please use another Json Parser this is and would be a toy
/// semiproject....
///        by default we use https://github.com/nlohmann/json which is good
///        enough...
/// @return
char json_parser::get_char() { return this->raw_input[current_position]; }

char json_parser::next() { current_position++; }

bool json_parser::is_eof() {
  if (current_position >= raw_input.size())
    ;
}

void json_parser::advance_to(int pos) { current_position += pos; }

std::string json_parser::extract_string(const std::string single) {
  int pos = raw_input.find(single, current_position);
  if (pos != std::string::npos) {
    current_position += pos;
    return raw_input.substr(current_position, pos);
  }
  throw std::exception();
}

void json_parser::parse_it() {
#if 0 
  while(!is_eof()){
     char ch = get_char();
     switch(ch)
     {
        case '{': tokens.emplace_back(json_token{json_token_type::curly_open}, "{"); next();break;
        case '}': tokens.emplace_back(json_token{json_token_type::curly_close}, "}");next(); break;
        case '[': tokens.emplace_back(json_token{json_token_type::square_open}, "[");next(); break;
        case ']': tokens.emplace_back(json_token{json_token_type::square_close}, "]");next(); break;
        case ':': tokens.emplace_back(json_token{json_token_type::colon}, ":");next(); break;
        case ',': tokens.emplace_back(json_token{json_token_type::comma}, ",");next(); break;
        case '\"': tokens.emplace_back(json_token{json_token_type::String}, extract_string("\"")); break;
        case '\'': tokens.emplace_back(json_token{json_token_type::String}, extract_string("\'")); break;
        case '\0':tokens.emplace_back(json_token{json_token_type::eof}, "\0"); break;
        case ' ': next();break;
        default: std::exception();
     }
  }
#endif
}

json_parser::json_parser(const std::string &str) { set_string(str); }

void json_parser::set_string(const std::string &str) {
  tokens.clear();
  current_position = 0;
  raw_input = str;
  parse_it();
}
