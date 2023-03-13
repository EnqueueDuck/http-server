#include "http/http_server.h"

#include "http/http_request.h"

namespace http {

void HttpServer::InternalHandle(int connection_sd) {
  char buffer[opts_.max_http_request_size];

  int bytes_read = 1;
  while (bytes_read) {
    bytes_read = recv(connection_sd, buffer, sizeof(buffer), 0);
    if (bytes_read < 0) {
      LOG(ERROR) << "Error reading connection data";
      close(connection_sd);
      return;
    }
    if (bytes_read == 0) break;

    VLOG(1) << "Received " << bytes_read << " bytes";
    VLOG(1) << buffer;

    // Parse the http request
    request::HttpRequest request;
    request.ParseFromString(buffer);

    // Handle the request
    response::HttpResponse response = HandleRequest(request);

    // Send the response
    auto data = response.SerializeToString();
    send(connection_sd, data.c_str(), data.size(), 0);
  }
  close(connection_sd);
}

void HttpServer::Handle(int connection_sd) {
  executor_.Execute(std::bind(&HttpServer::InternalHandle, this, connection_sd));
}

} // namespace http
