#include <iostream>
#include <thread>
#include <gflags/gflags.h>
#include <glog/logging.h>

#include "http/server.h"

DEFINE_int32(port, 14396, "Server port");

int main(int argc, char *argv[]) {
  FLAGS_logtostderr = 1;
  google::InitGoogleLogging(argv[0]);
  google::ParseCommandLineFlags(&argc, &argv, true);

  http::ServerOpts opts;
  opts.num_workers = std::thread::hardware_concurrency();
  opts.socket_opts.port = FLAGS_port;

  http::Server server(opts);
  server.Initialize();
  server.Run();
  return 0;
}
