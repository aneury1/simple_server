#pragma once

#include "HttpRequest.h"


void http_send_default_printer(HttpRequest query_request);
void http_send_static_html_file(HttpRequest request);
void http_send_static_mp3_file(HttpRequest query_request);
void http_send_static_file(HttpRequest request);
void http_send_static_html_file_favicon(HttpRequest query_request);
void http_send_default_default_session_guids(HttpRequest query_request);