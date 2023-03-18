#pragma once

#include <atomic>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <glog/logging.h>
#include <event2/listener.h>

namespace http {

struct ServerSocketOpts {
  constexpr static int DEFAULT_SERVER_BACK_LOG = 32;
  constexpr static int DEFAULT_SERVER_PORT = 14396;

  int port = DEFAULT_SERVER_PORT;
  int backlog = DEFAULT_SERVER_BACK_LOG;
};

class ServerSocket {
 public:
  explicit ServerSocket(ServerSocketOpts opts) : opts_(opts) {};
  ~ServerSocket() {
    if (!stopped) Stop();
  }

  void Run();

  virtual void Handle(int connection_sd) = 0;
  virtual void HandleError(int err);

 protected:
  virtual void Initialize();
  virtual void Stop();

 private:
  sockaddr_in server_addr_;
  int server_sd_;

  struct event_config *event_cfg_;
  struct event_base *event_base_;
  struct evconnlistener *ev_conn_listener_;

  ServerSocketOpts opts_;
  std::atomic_bool stopped{false};
};

} // namespace http
