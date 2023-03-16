#include "http/http_server.h"

#include <fcntl.h>

#include "http/http_request.h"


static void set_nonblocking(int fd) {
  int flags = fcntl(fd, F_GETFL, 0);
  if (flags < 0) {
    LOG(ERROR) << "Error setting fd to non-blocking";
    return;
  }
  if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
    LOG(ERROR) << "Error setting fd to non-blocking";
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

void HttpServer::Worker::Stop() {
  stopped_ = true;
  thread_.join();
}

void HttpServer::Worker::Initialize() {
  buffer_ = std::make_unique<char[]>(opts_.max_http_request_size);
  epoll_fd_ = epoll_create1(0);
  thread_ = std::thread([this]() {
    struct epoll_event events[MAX_EPOLL_EVENTS];
    int num_events;
    while (!stopped_) {
      num_events = epoll_wait(epoll_fd_, events, MAX_EPOLL_EVENTS, EPOLL_TIMEOUT_MS);
      for (int i = 0; i < num_events; ++i) {
        // Client hang up
        if (events[i].events & EPOLLHUP) {
          close(events[i].data.fd);
          continue;
        }

        // Error
        if ((events[i].events & EPOLLERR) || !(events[i].events & EPOLLIN)) {
          LOG(ERROR) << "Error caught with fd " << events[i].data.fd;
          continue;
        }

        HandleConnection(events[i].data.fd);
      }
    }
  });
}

void HttpServer::Worker::AddConnection(int connection_sd) {
  set_nonblocking(connection_sd);

  struct epoll_event event;
  event.data.fd = connection_sd;
  event.events = EPOLLIN;

  if (epoll_ctl(epoll_fd_, EPOLL_CTL_ADD, connection_sd, &event) < 0) {
    LOG(ERROR) << "Error adding connection: " << connection_sd << " to epoll manager";
  }
}

void HttpServer::Handle(int connection_sd) {
  auto &worker = workers_[current_worker_idx_++];
  worker->AddConnection(connection_sd);
  if (current_worker_idx_ >= workers_.size()) current_worker_idx_ = 0;
}

void HttpServer::Initialize() {
  Server::Initialize();
  for (auto &worker : workers_) {
    worker->Initialize();
  }
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
