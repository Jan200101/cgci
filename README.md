# CGCI

very simplistic and naive CI written in C using CGI

## How it works

### Routing

paths are split into 4 segments:
- project
- action
- index
- extra

they define what shuold be done with which project and how to do it.

examples:
- `/proj` lists builds for `proj`
- `/proj/builds/2` lists information for build `2` of `proj`
- `/proj/builds/2/raw_log` returns the raw logs for build `2` of `proj`

### Building

Upon a build being triggered the process forks off into a builder which keeps track of the running target process and writes it to disk on completion.

### States

Build states are stored in the persistant system cache (`/var/cache/cgci`), read by the running process and written by the builder.
State ID's are simply counted up and not unique.

## How to run

CGCI needs a web server that is able to serve CGI scripts.

During development [nginx](https://nginx.org/) with [fcgiwrap](https://github.com/gnosek/fcgiwrap) was used, other CGI servers may work but your mileage may vary.

Python's webserver claims to be capable of running CGI script but is not compliant with the CGI spec, DO NOT USE.
