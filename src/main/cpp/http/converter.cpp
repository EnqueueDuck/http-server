#include "http/converter.h"

namespace http {

const Converter::ProtocolMapping &Converter::protocol_to_str() {
  static ProtocolMapping mapping = {
    ProtocolMappingEntry(HTTP_1_0, "HTTP/1.0"),
    ProtocolMappingEntry(HTTP_1_1, "HTTP/1.1")
  };
  return mapping;
}

const Converter::MethodMapping &Converter::method_to_str() {
  static MethodMapping mapping = {
    MethodMappingEntry(GET    , "GET"     ),
    MethodMappingEntry(POST   , "POST"    ),
    MethodMappingEntry(HEAD   , "HEAD"    ),
    MethodMappingEntry(PUT    , "PUT"     ),
    MethodMappingEntry(DELETE , "DELETE"  ),
    MethodMappingEntry(CONNECT, "CONNECT" ),
    MethodMappingEntry(OPTIONS, "OPTIONS" )
  };
  return mapping;
}

const Converter::StatusCodeMapping &Converter::status_code_to_str() {
  static StatusCodeMapping mapping = {
    StatusCodeMappingEntry(HTTP_200_OK, "200 OK"),
    StatusCodeMappingEntry(HTTP_500_INTERNAL_SERVER_ERROR, "500 Internal Server Error")
  };
  return mapping;
}

std::string Converter::protocol_to_str(const Protocol &proc) {
  for (auto const &[protocol, str] : protocol_to_str()) {
    if (protocol == proc) return str;
  }
  return std::string();
}

Protocol Converter::str_to_protocol(const std::string &str) {
  for (auto const &[protocol, str_val] : protocol_to_str()) {
    if (str == str_val) return protocol;
  }
  throw std::runtime_error("Invalid protocol: " + str);
}

std::string Converter::method_to_str(const Method &method) {
  for (auto const &[method_val, str] : method_to_str()) {
    if (method_val == method) return str;
  }
  return std::string();
}

Method Converter::str_to_method(const std::string &str) {
  for (auto const &[method, str_val] : method_to_str()) {
    if (str == str_val) return method;
  }
  throw std::runtime_error("Invalid method: " + str);
}

std::string Converter::status_code_to_str(const StatusCode &code) {
  for (auto const &[status_code, str_value] : status_code_to_str()) {
    if (status_code == code) return str_value;
  }
  return std::string();
}

StatusCode Converter::str_to_status_code(const std::string &str) {
  for (auto const &[status_code, str_value] : status_code_to_str()) {
    if (str_value == str) return status_code;
  }
  throw std::runtime_error("Invalid status code: " + str);
}

} // namespace http
