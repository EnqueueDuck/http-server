#pragma once

#include "http/types.h"

#include <string>
#include <vector>

namespace http {

namespace request {

class HttpRequest {
 public:
  Method method;
  Target target;
  Protocol protocol;

  Headers headers;
  Body body;

  void ParseFromString(const std::string &data);

 private:
  void ParseMethodFromString(const std::string &data);

  void ParseProtocolFromString(const std::string &data);
};

} // namespace request

} // namespace http
