#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <thread>
#include <queue>

#include <glog/logging.h>

namespace thread {

struct ExecutorOpts {
  int num_worker_threads;
};

class Executor {
 public:
  using Task = std::function<void()>;

  class Worker {
   public:
    Worker() = default;

    void Run();
    void AddTask(Task &&t);
    void Stop();

    int num_pending_tasks();

   private:
    std::condition_variable cond_;
    std::queue<Task> task_queue_;
    std::mutex mtx_;
    std::thread thread_;

    std::atomic_bool stopped_{false};
  };

  explicit Executor(ExecutorOpts opts) : opts_(opts), workers_(opts.num_worker_threads) {
    CHECK(opts_.num_worker_threads > 0) << "At least one worker is required";
  }

  void Initialize();
  void Stop();
  void Execute(Task &&t);

 private:
  ExecutorOpts opts_;
  std::vector<Worker> workers_;
};

} // namespace thread
