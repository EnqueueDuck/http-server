# Introduction

This repository contains several versions of a HTTP Server implemented in CPP. The purpose
is to compare performance of different implementations and shows how one can optimize the
server to achieve beter through-put.

# Releases

To run the server, default host is 14396:

```
bazel run -c opt src/main/cpp:server
```

### Version 2.0

HTTP Server with N workers. Connections are distributed to the workers, each work handles IO
using EPOLL.

```
user~/wrk (master) ./wrk -t8 -c 10000 -d20s http://host:14396
Running 20s test @ http://host:14396
  8 threads and 10000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency    51.63us  129.97us  48.81ms   99.68%
    Req/Sec   103.74k    11.08k  134.55k    71.14%
  2073920 requests in 20.10s, 79.11MB read
  Socket errors: connect 8987, read 0, write 0, timeout 0
Requests/sec: 103189.69
Transfer/sec:      3.94MB
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
