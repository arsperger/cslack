# About

cslack is a simple C client for slack incoming webhook API. 

(Re)implementation of nonblocking TCP/TLS client based on Linux Epoll API multiplexing 

## Example (send_slack.c)
See `example/send_slack.c` for more info.

## Requirements
libssl-dev

## Installation 
`make` to build example
`make clean` will remove binaries and object files

## Comparing with other implementations:

libcurl:
```
time ./send_slack
real    0m1.505s
user    0m0.014s
sys     0m0.007s
```

basic sockets (blocking):
```
time ./send_slack 
real    1m2.569s
user    0m0.004s
sys     0m0.005s
```

epoll (master branch):
```
time ./send_slack
real    0m1.557s
user    0m0.713s
sys     0m0.213s
```
