#include "http/server_socket.h"

#include <functional>
#include <netinet/in.h>

static void accept_conn_cb(struct evconnlistener *listener,
                           evutil_socket_t fd,
                           struct sockaddr *address,
                           int socklen,
                           void *ctx) {
  http::ServerSocket *server = reinterpret_cast<http::ServerSocket*>(ctx);
  auto callback = std::bind(&http::ServerSocket::Handle, server, fd);
  callback();
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx) {
  http::ServerSocket *server = reinterpret_cast<http::ServerSocket*>(ctx);
  int err = EVUTIL_SOCKET_ERROR();
  auto callback = std::bind(&http::ServerSocket::HandleError, server, err);
  callback();
}

namespace http {

void ServerSocket::Stop() {
  stopped = true;
  evconnlistener_free(ev_conn_listener_);
  event_base_free(event_base_);
}

void ServerSocket::Run() {
  // Initialize event_base
  auto cfg = event_config_new(); 
  event_config_require_features(cfg, EV_FEATURE_ET);
  event_base_ = event_base_new_with_config(cfg);
  event_config_free(cfg);

  // Initialize connection listener
  ev_conn_listener_ = evconnlistener_new_bind(
    event_base_,
    accept_conn_cb,
    this,
    LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE,
    opts_.backlog,
    (struct sockaddr*)&server_addr_,
    sizeof(server_addr_)
  );

  // Set error callback
  evconnlistener_set_error_cb(ev_conn_listener_, accept_error_cb);

  // Ready to dispatch
  event_base_dispatch(event_base_);
}

void ServerSocket::HandleError(int err) {
  LOG(ERROR) << "Error accepting connection: " << evutil_socket_error_to_string(err);
}

void ServerSocket::Initialize() {
  bzero((char*)&server_addr_, sizeof(server_addr_));
  server_addr_.sin_family = AF_INET;
  server_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr_.sin_port = htons(opts_.port);
}

} // namespace http
