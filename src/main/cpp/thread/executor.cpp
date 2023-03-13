#include "thread/executor.h"

namespace thread {

void Executor::Worker::Run() {
  thread_ = std::thread([this]() {
    std::unique_lock<std::mutex> lck(mtx_);
    while (!stopped_) {
      cond_.wait(lck);
      if (!task_queue_.empty()) {
        Task t = std::move(task_queue_.front());
        task_queue_.pop();
        // Execute the task here
        t();
      }
    }
  });
}

void Executor::Worker::AddTask(Task &&t) {
  task_queue_.push(std::forward<Task>(t));
  cond_.notify_one();
}

void Executor::Worker::Stop() {
  stopped_ = true;
  cond_.notify_one();
  thread_.join();
}

int Executor::Worker::num_pending_tasks() {
  std::unique_lock<std::mutex> lck(mtx_);
  return task_queue_.size();
}

void Executor::Initialize() {
  for (auto &worker : workers_) {
    worker.Run();
  }
  LOG(INFO) << "Started " << workers_.size() << " workers";
}

void Executor::Stop() {
  for (auto &worker : workers_) {
    worker.Stop();
  }
}

void Executor::Execute(Task &&t) {
  int minimum_pending_task = std::numeric_limits<int>::max();
  int assigned_worker = -1;
  for (int i = 0; i < static_cast<int>(workers_.size()); ++i) {
    if (workers_[i].num_pending_tasks() < minimum_pending_task) {
      minimum_pending_task = workers_[i].num_pending_tasks();
      assigned_worker = i;
    }
    if (minimum_pending_task == 0) break;
  }
  workers_[assigned_worker].AddTask(std::forward<Task>(t));
}

} // namespace thread
