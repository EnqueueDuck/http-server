#pragma once

#include <array>

#include "http/types.h"

using namespace http::request;
using namespace http::response;

namespace http {

class Converter {
 public:
  using ProtocolMappingEntry = std::pair<Protocol, std::string>;
  using ProtocolMapping = std::array<ProtocolMappingEntry, 2>;

  using MethodMappingEntry = std::pair<Method, std::string>;
  using MethodMapping = std::array<MethodMappingEntry, 7>;

  using StatusCodeMappingEntry = std::pair<StatusCode, std::string>;
  using StatusCodeMapping = std::array<StatusCodeMappingEntry, 7>;

  const ProtocolMapping &protocol_to_str();
  const MethodMapping &method_to_str();
  const StatusCodeMapping &status_code_to_str();

  std::string protocol_to_str(const Protocol &proc);
  std::string method_to_str(const Method &method);
  std::string status_code_to_str(const StatusCode &status_code);

  Protocol str_to_protocol(const std::string &str);
  Method str_to_method(const std::string &str);
  StatusCode str_to_status_code(const std::string &value);
};

} // namespace http
