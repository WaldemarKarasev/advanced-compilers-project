# Stella Garbage Collector

This repository contains the garbage collector for the Stella runtime. The implementation uses an incremental Cheney-Baker copying collector with two semispaces, a read barrier, and explicit dynamic and permanent roots.

## Build

Requirements: CMake 3.20 or newer, C and C++ compilers, and [`just`](https://github.com/casey/just). The project requests C11 and C++20. The recorded environment used CMake 3.28.3, GCC/G++ 13, and `just` 1.46.0 on Ubuntu 24.04.4 LTS x86_64.

Configure and build the project:

```sh
just configure
just build
```

The default configuration uses the `build` directory, `Debug` build type, `BUILD_TESTING=ON`, exports `compile_commands.json`, and builds with 10 jobs. CMake fetches GoogleTest `release-1.12.1` when tests are enabled though option `DBUILD_TESTING=ON`.

The `BUILD_DIR`, `BUILD_TYPE`, and `JOBS` environment variables override the corresponding defaults. For example:

```sh
BUILD_TYPE=Release JOBS=4 just build
```

## Test

Run the unit and integration test suites:

```sh
just test
```

Run the same suites with sanitize build (AddressSanitizer,UndefinedBehaviorSanitizer):

```sh
just sanitize-test
```

The sanitizer build uses the `build_sanitize` directory and adds `-fsanitize=address,undefined -fno-omit-frame-pointer` to C/C++ compilation and `-fsanitize=address,undefined` when linking. `SANITIZE_BUILD_DIR` overrides its build directory. Both test commands use `STELLA_GC_STEP_BUDGET=1` by default; set that variable to choose another positive step budget. To run CTest directly after building:

```sh
ctest --test-dir build --output-on-failure
```

## Execute Stella example

Stella executables accept a program argument and an optional heap-size limit in bytes:

```text
PROGRAM VALUE [MAX_HEAP_BYTES]
```

`max_heap_bytes` sets the heap-size limit for the selected garbage collector. Its exact meaning depends on the implementation; for Cheney-Baker, it specifies the capacity of each semispace. The runtime default is 64 MiB per semispace.

For example, run the `chat` workload with a 4096-byte semispace and print statistics:

```sh
STELLA_GC_STATS=1 STELLA_GC_STEP_BUDGET=1 \
  ./build/tests/integration/chat_cheney_baker 10 4096
```

The following environment variables control collector diagnostics and incremental work:

| Environment variable | Effect |
|---|---|
| `STELLA_GC_STATS=1` | Print collector statistics to `stderr`. |
| `STELLA_GC_STATE=1` | Print heap and collector state to `stderr`. |
| `STELLA_GC_STEP_BUDGET=N` | Set the positive number of objects processed per incremental scan step. The default is `1`. |

## Repository map

| Path | Contents |
|---|---|
| `gc/cheney-baker/` | Collector, semispace heap, forwarding, incremental scan, roots, statistics, and diagnostics. |
| `runtime/`, `include/` | Stella runtime and the C ABI used by generated programs and the collector. |
| `tests/gc/` | Unit tests for collector internals and the public Stella GC API. |
| `tests/integration/` | CTest registrations for compiled Stella examples. |
| `examples/` | Stella examples and their generated C sources. |
| `cmake/` | CMake configuration, including compiler options. |
| `justfile` | Build, test, sanitizer, and cleanup recipes. |

## Project parts

- [Stage 1: Garbage Collector](REPORT.md) — report for the completed first project stage.
