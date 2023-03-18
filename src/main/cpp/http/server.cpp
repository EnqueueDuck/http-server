#include "http/server.h"

#include <fcntl.h>
#include <functional>

#include "http/http_request.h"

static void conn_read_callback(int fd, short what, void *arg) {
  if (what & EV_READ) {
    http::Server::Worker *worker = reinterpret_cast<http::Server::Worker*>(arg);
    auto callback = std::bind(&http::Server::Worker::HandleConnection, worker, fd);
    callback();
  } else {
    LOG(ERROR) << "Got a strange event "
               << ((what & EV_TIMEOUT) ? " timeout" : "")
               << ((what & EV_READ)    ? " read"    : "")
               << ((what & EV_WRITE)   ? " write"   : "")
               << ((what & EV_SIGNAL)  ? " signal"  : "");
  }
}

namespace http {

void Server::Worker::HandleConnection(int connection_sd) {
  int bytes_read = 1;
  while (bytes_read) {
    bytes_read = recv(connection_sd, buffer_.get(), opts_.max_http_request_size, 0);
    if (bytes_read < 0) {
      // Empty read/write due to non-blocking read, should continue listening
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
        AddConnection(connection_sd);
        return;
      }

      // Connection reset by peer is normal
      if (errno != ECONNRESET) {
        LOG(ERROR) << "Error reading connection data " << errno;
      }
      close(connection_sd);
      return;
    }
    if (bytes_read == 0) break;

    // Parse the http request
    request::HttpRequest request;
    request.ParseFromString(buffer_.get());

    // Handle the request
    response::HttpResponse response = HandleHttpRequest(request);

    // Send the response
    auto data = response.SerializeToString();
    send(connection_sd, data.c_str(), data.size(), 0);
  }
  close(connection_sd);
}

void Server::Worker::AddConnection(int connection_sd) {
  // Connection received from ev_conn_listener should be non-blocking already
  auto new_event = event_new(
    event_base_,
    connection_sd,
    EV_READ,
    conn_read_callback,
    this
  );
  event_add(new_event, NULL);
}

void Server::Worker::Stop() {
  thread_.join();

  event_base_loopexit(event_base_, NULL);
  event_base_free(event_base_);
}

void Server::Worker::Initialize() {
  buffer_ = std::make_unique<char[]>(opts_.max_http_request_size);

  // Initialize event_base
  auto cfg = event_config_new(); 
  event_config_require_features(cfg, EV_FEATURE_ET);
  event_base_ = event_base_new_with_config(cfg);
  event_config_free(cfg);

  thread_ = std::thread([this]() {
    event_base_loop(event_base_, EVLOOP_NO_EXIT_ON_EMPTY);
  });
}

void Server::Handle(int connection_sd) {
  workers_[current_worker_idx_++]->AddConnection(connection_sd);
  if (current_worker_idx_ >= static_cast<int>(workers_.size())) {
    current_worker_idx_ = 0;
  }
}

void Server::Initialize() {
  ServerSocket::Initialize();
  for (auto &worker : workers_) {
    worker->Initialize();
  }
}

void Server::Stop() {
  ServerSocket::Stop();
  for (auto &worker : workers_) {
    worker->Stop();
  }
}

response::HttpResponse Server::Worker::HandleHttpRequest(
    const request::HttpRequest &request
) const {
  response::HttpResponse response;
  response.status_code = response::HTTP_200_OK;
  response.body = "ok";
  return response;
}

} // namespace http
