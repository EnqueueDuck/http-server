# Introduction

A sample HTTP Server implemented using `libevent`.

This repository also contains several versions of a HTTP Server implemented in cpp.

# Releases

To run the server, use the following command, default host is 14396:

```
bazel run -c opt src/main/cpp:server
```

### Version 3.1.0

The base class for server is `ServerSocket` at `src/main/cpp/server_socket.h`. I use an
`evconnlistener` from `libevent` to bind and listen on the server address. For each new connection
, `accept_conn_cb` is called. The server passes the connection to one of the workers to handle
(defined in `src/main/cpp/server`).

The server worker runs an event loop, connections received from the server will be added to the
loop for monitoring. When the connection becomes active, `conn_read_callback` will be triggered. It
reads from the socket, parses the HTTP request, call `HandleHttpRequest` to produce an
`HttpResponse` and write the response to the connection socket.

Some notes:
- Connections received from `evconnlistener` are non-blocking by default. When reading from
non-blocking fd, if the buffer is not ready, error `EAGAIN` or `EWOULDBLOCK` would be returned. We
should not close the connection there, but simply skip it, continue to proceed other connections.
- When the event loop triggers `conn_read_callback`, the event associated with the connection
becomes non-pending and will not trigger callbacks anymore, so in the callback function, we need
to add the connection again after we're done processing it.
- We run the event loop in one thread, when there's no event, and then adding connection to it from
another thread, so we need to set flag `EVLOOP_NO_EXIT_ON_EMPTY`.

HTTP Server with N workers. Connections are monitored by libevent, active connections are passed
to workers for handling.

```
user~/wrk (master) ./wrk -t8 -c10000 -d20s http://host:14396
Running 20s test @ http://host:14396
  8 threads and 10000 connections
  Thread Stats   Avg      Stdev     Max   +/- Stdev
    Latency     3.39ms   16.09ms   1.68s    98.65%
    Req/Sec    37.57k    11.24k  107.14k    71.88%
  5897359 requests in 20.08s, 224.97MB read
  Socket errors: connect 8987, read 0, write 0, timeout 0
Requests/sec: 293685.43
Transfer/sec:     11.20MB
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

### Version 1.1.0

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
