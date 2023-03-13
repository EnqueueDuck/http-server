#pragma once

#include "http/server.h"
#include "thread/executor.h"
#include "http/http_response.h"
#include "http/http_request.h"

namespace http {

struct HttpServerOpts {
  constexpr static int DEFAULT_MAX_HTTP_REQUEST_SIZE = 65536;

  ServerOpts server_opts;
  thread::ExecutorOpts executor_opts;

  int max_http_request_size = DEFAULT_MAX_HTTP_REQUEST_SIZE;
};

class HttpServer : public Server {
 public:
  explicit HttpServer(HttpServerOpts opts)
      : Server(opts.server_opts), opts_(opts), executor_(opts.executor_opts) { }

  void Initialize() {
    Server::Initialize();
    executor_.Initialize();
  }

 protected:
  void Handle(int connection_sd) override;

  virtual response::HttpResponse HandleRequest(const request::HttpRequest &request) const {
    response::HttpResponse response;
    response.status_code = response::HTTP_200_OK;
    response.body = "ok";
    return response;
  }

 private:
  HttpServerOpts opts_;
  thread::Executor executor_;

  void InternalHandle(int connection_sd);
};

} // namespace http
