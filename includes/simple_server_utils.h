#ifndef SIMPLE_SERVER_HTTP_SERVER_UTILS_DEFINED
#define SIMPLE_SERVER_HTTP_SERVER_UTILS_DEFINED

#pragma once
#include <vector>
#include <functional>
#include <algorithm>
#include "simple_server_definitions.h"
#include "simple_server_interface.h"
#include "simple_server_logger.h"

namespace simple_server {

std::string replace_all(std::string str, const std::string &from,
                       const std::string &to);

std::string read_whole_file(std::string src);

void print_error_of_descriptor(const socket_connection& fd);
std::vector<String> split_words(String word, char sp);
String to_lower_case_str(const String &str);
String extract_url_with_query_params(String request);
String extract_url(String request);
StringMap parse_headers(const String &request);
String extract_body(const String str);
StringMap parse_url_params(String url);
bool validate_pattern_by_url(std::vector<String> uri,
                         std::vector<String> pattern);

std::map<std::string, std::string> get_local_addresses();
std::map<std::string, std::string> get_local_ip_address_list();
std::string execute_cat_cmd(std::string path, std::string desc);
std::string get_port_by_tulnp_cmd();
std::string get_working_directory();
bool check_if_path_exist(const std::string& path);

}

#endif