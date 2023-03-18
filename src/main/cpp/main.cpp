#include <iostream>
#include <thread>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "http/server.h"

int main(int argc, char *argv[]) {
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);

  http::ServerOpts opts;
  opts.num_workers = std::thread::hardware_concurrency();

  http::Server server(opts);
  server.Initialize();
  server.Run();
  return 0;
}
