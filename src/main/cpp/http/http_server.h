#pragma once

#include <sys/epoll.h>
#include <thread>
#include <atomic>
#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>

#include <event2/event.h>

#include "http/server.h"
#include "http/http_response.h"
#include "http/http_request.h"

namespace http {

struct HttpServerOpts {
  ServerOpts server_opts;
  int num_workers;
};

class HttpServer : public Server {
 public:
  constexpr static int DEFAULT_HTTP_REQUEST_SIZE = 8192;

  explicit HttpServer(const HttpServerOpts &opts) : Server(opts.server_opts), opts_(opts) {
    for (int i = 0; i < opts_.num_workers; ++i) {
      workers_.emplace_back(std::make_unique<Worker>(WorkerOpts()));
    }
    LOG(INFO) << "Started " << opts_.num_workers << " workers";
  }

  struct WorkerOpts {
    int max_http_request_size = DEFAULT_HTTP_REQUEST_SIZE;
  };

  class Worker {
   public:
    Worker() = default;
    explicit Worker(const WorkerOpts &opts) : opts_(opts) { }

    constexpr static int MAX_EPOLL_EVENTS = 64;
    constexpr static int EPOLL_TIMEOUT_MS = 10000; // 10 seconds

    void Initialize();

    void Stop();

    void HandleConnection(int connection_sd);

    void AddConnection(int connection_sd);

   private:
    WorkerOpts opts_;
    std::unique_ptr<char[]> buffer_;
    std::atomic_bool stopped_{false};

    std::condition_variable cond_;
    std::queue<int> task_queue_;
    std::mutex mtx_;
    std::thread thread_;

    virtual response::HttpResponse HandleRequest(const request::HttpRequest &request) const;
  };

  using WorkerPtr = std::unique_ptr<Worker>;

  void Initialize();

  void AssignConnectionToWorker(int connection_sd);

  void Handle(int connection_sd) override;

 protected:

  void Stop();

 private:
  HttpServerOpts opts_;

  std::vector<WorkerPtr> workers_;
  int current_worker_idx_ = 0;

  struct event_config *event_cfg_;
  struct event_base *event_base_;
};

} // namespace http
