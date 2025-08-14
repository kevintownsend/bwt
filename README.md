![tests](https://github.com/kevintownsend/bwt/actions/workflows/ci.yml/badge.svg)

# A C++ Burrows Wheeler Transform Library

This is a C++ implementation of the Burrows wheeler transform.

## Overview

### Burrows Wheeler Transform

The Burrows Wheeler transform is a near 1 to 1 transformation. It has many uses, in particular data compressiond and data retrieval. It's used in bzip and the DNA aligner bowtie.

Concisely, the transformation is the concatenation of the last elements of the cyclic rotations of the original sequence.

There are several libraries that just encode and decode byte/chars but this library can encode and decode arbitrary types by using templates.

## Usage

Because the transformation has one extra EOL character a key pointing to where the EOL should be is returned. Likewise the third argument of the decoder requires that key.

Here are the function signatiures:

```c++
template <typename RandomAccessIterator>
std::pair<std::vector<
              typename std::iterator_traits<RandomAccessIterator>::value_type>,
          size_t>
bwt(RandomAccessIterator first, RandomAccessIterator last);

template <typename RandomAccessIterator>
std::pair<std::vector<
              typename std::iterator_traits<RandomAccessIterator>::value_type>,
          bool>
iBwt(RandomAccessIterator first, RandomAccessIterator last, const size_t key);
```

Copying bwt.hpp should enable you to use these two functions.

## Contributing

If you want to contribute you can file an issue. Submit a change request by forking the
repo and issuing a pull request to the main branch.

Clone the repo:

```bash
git clone https://github.com/kevintownsend/bwt.git
```

prerequisites to run tests:
[install bazel](https://bazel.build/install)

run tests:

```bash
bazel test :all
```
