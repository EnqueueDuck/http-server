#include "http/http_server.h"

#include <thread>

int main() {
  http::HttpServerOpts opts;
  opts.executor_opts.num_worker_threads = std::thread::hardware_concurrency();
  http::HttpServer server(opts);
  server.Initialize();
  server.Run();
  return 0;
}
