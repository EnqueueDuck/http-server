#include "thread/executor.h"

#include <thread>

int main() {
  thread::ExecutorOpts opts;
  opts.num_worker_threads = 8;

  thread::Executor executor(opts);
  executor.Initialize();

  LOG(INFO) << "Main thread is " << std::this_thread::get_id();
  for (int i = 0; i < opts.num_worker_threads; ++i) {
    executor.Execute([]() {
      LOG(INFO) << "This is run in thread " << std::this_thread::get_id();
    });
  }
  executor.Stop();
  return 0;
}
