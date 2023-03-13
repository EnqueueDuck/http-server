#pragma once

#include "http/types.h"

namespace http {

namespace response {

class HttpResponse {
 public:
  Protocol protocol = HTTP_1_1;
  StatusCode status_code;

  Headers headers;
  Body body;

  std::string SerializeToString() const;

 private:
  std::string SerializeStatusLineToString() const;

  std::string SerializeHeadersToString() const;
};

} // namespace response

} // namespace http
