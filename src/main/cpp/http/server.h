#pragma once

#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <glog/logging.h>

namespace http {

struct ServerOpts {
  constexpr static int DEFAULT_SERVER_BACK_LOG = 32;
  constexpr static int DEFAULT_SERVER_PORT = 14396;

  int port = DEFAULT_SERVER_PORT;
  int backlog = DEFAULT_SERVER_BACK_LOG;
};

// Ref: https://github.com/bozkurthan/Simple-TCP-Server-Client-CPP-Example/blob/master/tcp-Server.cpp
class Server {
 public:
  explicit Server(ServerOpts opts) : opts_(opts) {};

  void Initialize();
  void Run();
  void Stop();

 protected:
  virtual void Handle(int connection_sd) = 0;

 private:
  sockaddr_in server_addr_;
  int server_sd_;

  ServerOpts opts_;
  std::atomic_bool stopped{false};
};

} // namespace http
