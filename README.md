# Introduction

This repository contains several versions of a HTTP Server implemented in CPP. The purpose
is to compare performance of different implementations and shows how one can optimize the
server to achieve beter through-put.

# Releases

To run the server, default host is 14396:

```
bazel run -c opt src/main/cpp:server
```

### Version 2.2.0

HTTP Server with N workers. Connections are distributed to the workers, each work handles IO
using EPOLL with non-blocking IOs.

```
user~/wrk (master) ./wrk -t8 -c 10000 -d20s http://host:14396
Running 20s test @ http://host:14396
  8 threads and 10000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     2.17ms    3.95ms 240.01ms   89.26%
    Req/Sec    48.79k     9.00k  108.25k    83.06%
  7711554 requests in 20.09s, 294.17MB read
  Socket errors: connect 8987, read 0, write 0, timeout 0
Requests/sec: 383928.67
Transfer/sec:     14.65MB
```

### Version 2.1.0

HTTP Server with N workers. Connections are distributed to the workers, each work handles IO
using EPOLL.

```
user~/wrk (master) ./wrk -t8 -c 10000 -d20s http://host:14396
Running 20s test @ http://host:14396
  8 threads and 10000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    31.02us   37.16us  15.38ms   99.95%
    Req/Sec    74.05k    26.72k  103.45k    59.33%
  4420272 requests in 20.03s, 168.62MB read
  Socket errors: connect 8987, read 0, write 0, timeout 0
Requests/sec: 220632.74
Transfer/sec:      8.42MB
```

### Version 1.1

HTTP Server with a multi-threaded backend. The backend employs a ThreadPool, which manages
N worker threads. Each request is encapsulated in a Task object and passed to one of the worker
threads for handling.

Benchmark with WRK:

```
user@another_host:~/wrk$ ./wrk -t16 -c10000 -d20s http://host:14396
Running 20s test @ http://host:14396
  16 threads and 10000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency   142.03us  462.59us 205.24ms   99.97%
    Req/Sec    20.89k     2.72k   24.44k    70.00%
  415862 requests in 20.09s, 15.86MB read
  Socket errors: connect 19, read 15, write 0, timeout 0
Requests/sec:  20699.89
Transfer/sec:    808.59KB
```
