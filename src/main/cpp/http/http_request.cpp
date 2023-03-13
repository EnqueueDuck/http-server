#include "http/http_request.h"

#include <array>
#include <iostream>
#include <sstream>
#include <glog/logging.h>

#include "http/converter.h"

static std::vector<std::string> split_string(const std::string &data,
                                             const std::string &delimiter,
                                             int num_split = -1) {
  std::vector<std::string> tokens;
  size_t pos_start = 0, pos_end;

  while ((pos_end = data.find(delimiter, pos_start)) != std::string::npos) {
    auto token = data.substr(pos_start, pos_end - pos_start);
    pos_start = pos_end + delimiter.size();
    tokens.push_back(token);

    if (num_split > 0 && (--num_split) == 0) break;
  }

  tokens.push_back(data.substr(pos_start));
  return tokens;
}

static std::string get_next_line(const std::string &data, int &pos_start) {
  auto pos_end = data.find(http::request::CRLF, pos_start);
  auto token = data.substr(pos_start, pos_end - pos_start);
  pos_start = pos_end + http::request::CRLF_LENGTH;
  return token;
}

namespace http {

namespace request {

void HttpRequest::ParseMethodFromString(const std::string &data) {
  method = Converter().str_to_method(data);
}

void HttpRequest::ParseProtocolFromString(const std::string &data) {
  protocol = Converter().str_to_protocol(data);
}

void HttpRequest::ParseFromString(const std::string &data) {
  int pos_start = 0;
  std::string line;

  // Parse status line
  line = get_next_line(data, pos_start);
  auto tokens = split_string(line, " ");
  ParseMethodFromString(tokens[0]);
  target = tokens[1];
  ParseProtocolFromString(tokens[2]);

  // Parse headers
  line = get_next_line(data, pos_start);
  while (!line.empty()) {
    tokens = split_string(line, ": ", 1);
    headers.emplace_back(tokens[0], tokens[1]);
    line = get_next_line(data, pos_start);
  }

  // Body ends with CRLF
  body = data.substr(pos_start, data.size() - pos_start - CRLF_LENGTH);
}

} // namespace request

} // namespace http
