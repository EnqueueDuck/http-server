# Introduction

This repository contains several versions of a HTTP Server implemented in CPP. The purpose
is to compare performance of different implementations and shows how one can optimize the
server to achieve beter through-put.

# Releases

### Version 1.1

To run the server:

```
bazel run -c opt src/main/cpp:server
```

Simple HTTP Server with a multi-threaded backend. The backend employs a ThreadPool, which manages
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
