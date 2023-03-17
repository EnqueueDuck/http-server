#include "http/http_server.h"

#include <fcntl.h>
#include <functional>

#include "http/http_request.h"

static void worker_event_callback(int fd, short what, void *arg) {
  if (what & EV_READ) {
    http::HttpServer *server = reinterpret_cast<http::HttpServer*>(arg);
    auto callback = std::bind(&http::HttpServer::AssignConnectionToWorker, server, fd);
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

void HttpServer::Worker::HandleConnection(int connection_sd) {
  int bytes_read = 1;
  while (bytes_read) {
    bytes_read = recv(connection_sd, buffer_.get(), opts_.max_http_request_size, 0);
    if (bytes_read < 0) {
      // Empty read due to non-blocking read, don't close yet
      if (errno == EAGAIN || errno == EWOULDBLOCK) {
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
    response::HttpResponse response = HandleRequest(request);

    // Send the response
    auto data = response.SerializeToString();
    send(connection_sd, data.c_str(), data.size(), 0);
  }
  close(connection_sd);
}

void HttpServer::Worker::AddConnection(int connection_sd) {
  std::unique_lock<std::mutex> lck(mtx_);
  task_queue_.push(connection_sd);
  cond_.notify_one();
}

void HttpServer::Worker::Stop() {
  stopped_ = true;
  thread_.join();
  cond_.notify_one();
}

void HttpServer::Worker::Initialize() {
  buffer_ = std::make_unique<char[]>(opts_.max_http_request_size);

  thread_ = std::thread([this]() {
    std::unique_lock<std::mutex> lck(mtx_);
    while (!stopped_) {
      cond_.wait(lck);
      if (!task_queue_.empty()) {
        auto t = task_queue_.front(); 
        task_queue_.pop();
        HandleConnection(t);
      }
    }
  });
}

void HttpServer::AssignConnectionToWorker(int connection_id) {
  workers_[current_worker_idx_++]->AddConnection(connection_id);
  if (current_worker_idx_ >= static_cast<int>(workers_.size())) {
    current_worker_idx_ = 0;
  }
}

void HttpServer::Handle(int connection_sd) {
  auto new_event = event_new(
    event_base_,
    connection_sd,
    EV_READ,
    worker_event_callback,
    this
  );
  event_add(new_event, NULL);
  event_base_dispatch(event_base_);
}

void HttpServer::Initialize() {
  Server::Initialize();
  for (auto &worker : workers_) {
    worker->Initialize();
  }

  // Initialize event_base
  event_cfg_ = event_config_new(); 
  event_config_require_features(event_cfg_, EV_FEATURE_ET);
  event_base_ = event_base_new_with_config(event_cfg_);
  event_config_free(event_cfg_);
}

void HttpServer::Stop() {
  Server::Stop();
  event_base_free(event_base_);
}

response::HttpResponse HttpServer::Worker::HandleRequest(
    const request::HttpRequest &request
) const {
  response::HttpResponse response;
  response.status_code = response::HTTP_200_OK;
  response.body = "ok";
  return response;
}

} // namespace http
