#include <iostream>
#include <thread>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "http/http_server.h"

int main(int argc, char *argv[]) {
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);

  http::HttpServerOpts opts;
  opts.num_workers = std::thread::hardware_concurrency();

  http::HttpServer server(opts);
  server.Initialize();
  server.Run();
  return 0;
}
