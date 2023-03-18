#include "http/server.h"

#include <functional>
#include <netinet/in.h>

static void accept_conn_cb(struct evconnlistener *listener,
                           evutil_socket_t fd,
                           struct sockaddr *address,
                           int socklen,
                           void *ctx) {
  http::Server *server = reinterpret_cast<http::Server*>(ctx);
  auto callback = std::bind(&http::Server::Handle, server, fd);
  callback();
}

static void accept_error_cb(struct evconnlistener *listener, void *ctx) {
  http::Server *server = reinterpret_cast<http::Server*>(ctx);
  int err = EVUTIL_SOCKET_ERROR();
  auto callback = std::bind(&http::Server::HandleError, server, err);
  callback();
}

namespace http {

void Server::Stop() {
  stopped = true;
  evconnlistener_free(ev_conn_listener_);
}

void Server::Run() {
  // Initialize event_base
  event_cfg_ = event_config_new(); 
  event_base_ = event_base_new_with_config(event_cfg_);
  event_config_free(event_cfg_);

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

void Server::HandleError(int err) {
  LOG(ERROR) << "Error accepting connection: " << evutil_socket_error_to_string(err);
}

void Server::Initialize() {
  bzero((char*)&server_addr_, sizeof(server_addr_));
  server_addr_.sin_family = AF_INET;
  server_addr_.sin_addr.s_addr = htonl(INADDR_ANY);
  server_addr_.sin_port = htons(opts_.port);
}

} // namespace http
