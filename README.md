## Description

Program to do init, put, get, delete and dump leveldb databases.

## Getting the Source

```bash
git clone --recurse-submodules https://github.com/sepehrdaddev/levelctl
```

## Building

This project supports [CMake](https://cmake.org/) out of the box.

```bash
mkdir -p build && cd build
cmake .. && cmake --build .
```

## Usage

```
$ levelctl -h
  levelctl COMMAND {OPTIONS}

    levelctl - Manage Leveldb Databases

  OPTIONS:

      Available Commands:
        init                              Initialize database in the given path
        put                               Add key to the given database
        get                               Get value of key from database
        del                               Delete the given key from database
        dump                              Dump the given database
      Flags:
        -h, --help                        Display this help menu
        -v, --version                     Display levelctl version

```

## Get Involved

**Please, send us pull requests!**
