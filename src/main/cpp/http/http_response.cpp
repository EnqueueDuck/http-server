#include "http/http_response.h"
#include "http/converter.h"

namespace http {

namespace response {

std::string HttpResponse::SerializeToString() const {
  return SerializeStatusLineToString() \
       + SerializeHeadersToString() \
       + std::string(CRLF) \
       + body;
}

std::string HttpResponse::SerializeStatusLineToString() const {
  Converter converter;
  return converter.protocol_to_str(protocol)
       + " " + converter.status_code_to_str(status_code)
       + std::string(CRLF);
}

std::string HttpResponse::SerializeHeadersToString() const {
  std::string CONTENT_LENGTH = "Content-Length";
  bool content_length_included = false;

  std::string data;
  for (auto const &[key, value] : headers) {
    data += key + ": " + value + std::string(CRLF);
    if (key == CONTENT_LENGTH) content_length_included = true;
  }
  if (!content_length_included) {
    data += CONTENT_LENGTH + ": " + std::to_string(body.size()) + std::string(CRLF);
  }
  return data;
}

} // namespace response

} // namespace http
