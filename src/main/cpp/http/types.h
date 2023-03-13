#pragma once

#include <string>
#include <vector>

namespace http {

enum Protocol {
  HTTP_1_0         = 0,
  HTTP_1_1         = 1,
};

using HeaderKey = std::string;
using HeaderValue = std::string;
using Header = std::pair<HeaderKey, HeaderValue>;
using Headers = std::vector<Header>;
using Body = std::string;

namespace request {

constexpr const char *CRLF = "\r\n";
constexpr const int CRLF_LENGTH = 2;

enum Method {
  GET            = 0,
  POST           = 1,
  HEAD           = 2,
  PUT            = 3,
  DELETE         = 4,
  CONNECT        = 5,
  OPTIONS        = 6
};

using Target = std::string;

} // namespace request

namespace response {

enum StatusCode {
  HTTP_200_OK = 0,
  HTTP_500_INTERNAL_SERVER_ERROR = 1,
};

} // namespace response

} // namespace http
