[![ci](https://github.com/kevintownsend/bwt/actions/workflows/ci.yml/badge.svg)](https://github.com/kevintownsend/bwt/actions/workflows/ci.yml)

# C++ Burrows–Wheeler Transform Library

A single-header C++17 implementation of the **Burrows–Wheeler Transform (BWT)** and its **Last–First (LF) mapping inverse** for arbitrary `operator<`-comparable types.

## Overview

The Burrows–Wheeler Transform permutes a sequence so characters sharing similar right-hand contexts become adjacent, producing long runs of identical elements that compress well with run-length and entropy coding (as used in `bzip2` and read aligners such as Bowtie).

Unlike byte-only implementations, `bwt.hpp` works generically across `std::string`, `std::string_view`, `std::vector<T>`, `std::array`, `std::deque`, raw pointer ranges, and custom comparable structs while automatically selecting an `O(n)` 256-bucket initial pass for 1-byte integral types.

### How the Sentinel Works

To make every transformation unambiguously invertible, `bwt::encode` appends a virtual end-of-input sentinel `$` defined as strictly smaller than all elements in the sequence, omits `$` from the output vector, and returns its 0-based row index `key`.

For `"banana"`, sorting all cyclic rotations of `"banana$"` (`$` < `'a'`) produces:

| Row | Rotation | Last |
| :---: | :--- | :---: |
| 0 | `$banana` | `a` |
| 1 | `a$banan` | `n` |
| 2 | `ana$ban` | `n` |
| 3 | `anana$b` | `b` |
| 4 | `banana$` | **`$`** (`key = 4`) |
| 5 | `na$bana` | `a` |
| 6 | `nana$ba` | `a` |

Output: `data = {'a', 'n', 'n', 'b', 'a', 'a'}` (`"annb$aa"`), `key = 4`.

## Quickstart

Copy [bwt.hpp](bwt.hpp) into your project and compile with C++17 or newer:

```cpp
#include <cassert>
#include <string>

#include "bwt.hpp"

int main() {
  const auto [encoded, key] = bwt::encode("banana");
  // encoded == {'a', 'n', 'n', 'b', 'a', 'a'}, key == 4

  const auto decoded = bwt::decode(encoded, key);
  assert(decoded && std::string(decoded->begin(), decoded->end()) == "banana");
}
```

### API Summary

- `bwt::encode(first, last)` / `bwt::encode(range)` / `bwt::encode(std::string_view)` → returns `bwt::Encoded<T>{std::vector<T> data, std::size_t key}`.
- `bwt::decode(first, last, key)` / `bwt::decode(range, key)` / `bwt::decode(std::string_view, key)` / `bwt::decode(encoded)` → returns `std::optional<std::vector<T>>` (`std::nullopt` if `key` or the payload cycle structure is invalid).

## Complexity

| Operation | 1-byte integral `T` | General comparable `T` | Space |
| :--- | :--- | :--- | :--- |
| `bwt::encode` | `O(n)` rank + `O(n log n)` prefix-doubling SA | `O(n log n)` rank + `O(n log n)` prefix-doubling SA | `O(n)` |
| `bwt::decode` | `O(n)` counting-sort LF mapping | `O(n log n)` stable-rank LF mapping | `O(n)` |

## Building & Testing

### Bazel

```bash
bazel test //...
bazel run //examples:bwt_cli -- gataca
bazel run -c opt //benchmark:rand_int_benchmark_bin
```

### CMake

```bash
cmake -B build
cmake --build build --parallel
ctest --test-dir build --output-on-failure
```

## Benchmarks

Run via `bazel run -c opt //benchmark:rand_int_benchmark_bin` (Clang 21.1.8, Linux x86_64):

| Workload | Elements | Size (bytes) | Encode (ms) | Encode (MB/s) | Decode (ms) | Decode (MB/s) | Avg Run (In → Out) |
| :--- | ---: | ---: | ---: | ---: | ---: | ---: | ---: |
| `text` (prose) | 16,384 | 16,384 | 1.51 | 10.86 | 0.17 | 94.97 | 1.04 → 101.76 |
| `text` (prose) | 1,048,576 | 1,048,576 | 435.11 | 2.41 | 27.73 | 37.82 | 1.04 → 6472.69 |
| `dna` (synthetic) | 1,048,576 | 1,048,576 | 238.43 | 4.40 | 47.19 | 22.22 | 1.16 → 5.65 |
| `low_bin` (`uint8_t`) | 1,048,576 | 1,048,576 | 241.77 | 4.34 | 45.15 | 23.23 | 1.59 → 3.63 |
| `rand_u32` (`uint32_t`) | 1,048,576 | 4,194,304 | 184.90 | 22.68 | 178.08 | 23.55 | 1.00 → 1.00 |

## Contributing & License

Pull requests and issues are welcome. Please verify tests pass under both Bazel (including ASan/UBSan) and CMake, and check formatting with `clang-format` and `buildifier` before submitting.

Licensed under the [Apache License, Version 2.0](LICENSE).
