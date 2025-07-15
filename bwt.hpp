/**
 * Copyright 2025 Kevin Townsend
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BWT_H_
#define BWT_H_

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <map>
#include <set>
#include <stdint.h>
#include <vector>

namespace bwt {
std::pair<std::vector<char>, size_t> charBwt(char *first, char *last);

std::pair<std::vector<char>, size_t> charBwt(std::vector<char>::iterator first,
                                             std::vector<char>::iterator last);
std::vector<char> iCharBwt(const std::vector<char> &encoded_bwt,
                           const size_t key);

template <class RandomAccessIterator>
std::pair<std::vector<
              typename std::iterator_traits<RandomAccessIterator>::value_type>,
          size_t>
bwt(RandomAccessIterator first, RandomAccessIterator last);

template <typename RandomAccessIterator>
std::pair<std::vector<
              typename std::iterator_traits<RandomAccessIterator>::value_type>,
          bool>
iBwt(RandomAccessIterator first, RandomAccessIterator last, const size_t key);
} // namespace bwt

/**
 * The templated Burrows-Wheeler tranform.
 *
 * @param first Pointer to the first value.
 * @param last Pointer to the one over last value.
 * @return The encoded vector and relative location of the end character.
 */
template <class RandomAccessIterator>
std::pair<std::vector<
              typename std::iterator_traits<RandomAccessIterator>::value_type>,
          size_t>
bwt::bwt(RandomAccessIterator first, RandomAccessIterator last) {
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type T;
  size_t n = last - first;
  if (n == 0) {
    return make_pair(std::vector<T>(), 0);
  }

  // Get counts.
  std::map<T, size_t> counts;
  for (auto it = first; it != last; it++) {
    counts[*it]++;
  }

  // Create prefix sums.
  std::map<T, size_t> offsets;
  size_t sum = 0;
  for (auto it = counts.begin(); it != counts.end(); it++) {
    offsets[it->first] = sum;
    sum += it->second;
  }
  for (auto it = ++offsets.find(*first); it != offsets.end(); it++) {
    it->second--;
  }

  // Create the return vector (the bwt transformation).

  std::vector<T> ret(n);
  for (size_t i = 0; i < n - 1; ++i) {
    ret[offsets[first[i + 1]]++] = first[i];
  }
  ret[n - 1] = first[n - 1];

  return make_pair(ret, offsets[first[0]]);
}

/**
 * The templated inverse Burrows-Wheeler tranform.
 *
 * @param first Pointer to the first value.
 * @param last Pointer to the one over last value.
 * @param the location of the end character.
 * @return The decoded vector and success boolean.
 */
template <typename RandomAccessIterator>
std::pair<std::vector<
              typename std::iterator_traits<RandomAccessIterator>::value_type>,
          bool>
bwt::iBwt(RandomAccessIterator first, RandomAccessIterator last,
          const size_t key) {
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type T;
  size_t n = last - first;
  std::vector<T> ret(n);

  // Get counts.
  std::map<T, size_t> counts;
  for (auto it = first; it != last; it++) {
    counts[*it]++;
  }

  // Create prefix sums.
  std::map<T, size_t> offsets;
  size_t sum = 0;
  for (auto it = counts.begin(); it != counts.end(); it++) {
    sum += it->second;
    offsets[it->first] = sum;
  }

  // Decoding
  size_t curr_offset = n;
  for (size_t i = 0; i < n; i++) {
    if (curr_offset == key) {
      return make_pair(std::vector<T>(), false);
    }
    size_t effective_offset =
        (curr_offset > key) ? curr_offset - 1 : curr_offset;
    T c = first[effective_offset];
    ret[n - i - 1] = c;
    curr_offset = --offsets[c];
  }

  return make_pair(ret, true);
}

namespace townsend { // Depricaded use bwt namespace instead.
namespace algorithm {
template <typename RandomAccessIterator>
[[deprecated]]
RandomAccessIterator bwtEncode(RandomAccessIterator first,
                               RandomAccessIterator last);
template <typename RandomAccessIterator>
[[deprecated]]
void bwtDecode(RandomAccessIterator first, RandomAccessIterator last,
               RandomAccessIterator key);
} // namespace algorithm
} // namespace townsend

template <class RandomAccessIterator>
[[deprecated]]
RandomAccessIterator townsend::algorithm::bwtEncode(RandomAccessIterator first,
                                                    RandomAccessIterator last) {
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type T;
  size_t n = last - first;
  std::vector<T> encoded(n);
  size_t key;
  std::tie(encoded, key) = bwt::bwt(first, last);
  for (auto it1 = first, it2 = encoded.begin(); it1 != last; it1++, it2++) {
    *it1 = *it2;
  }
  return first + key;
}

template <class RandomAccessIterator>
[[deprecated]]
void townsend::algorithm::bwtDecode(RandomAccessIterator first,
                                    RandomAccessIterator last,
                                    RandomAccessIterator key) {
  using namespace std;
  vector<typename RandomAccessIterator::value_type> bwtEncoded(first, last);
  typename RandomAccessIterator::value_type empty;
  bwtEncoded.insert(bwtEncoded.begin() + (key - first), empty);
  RandomAccessIterator bwtEncodedKey = bwtEncoded.begin() + (key - first);
  map<typename RandomAccessIterator::value_type, uint64_t> sums;
  map<typename RandomAccessIterator::value_type, uint64_t> cummulativeSum;
  uint64_t keyIndex = key - first;
  uint64_t sum = 0;
  for (auto i = first; i != last; i++) {
    sums[*i]++;
  }
  for (auto it = sums.begin(); it != sums.end(); it++) {
    cummulativeSum[it->first] = sum;
    sum += it->second;
  }
  vector<uint64_t> fmIndex;
  for (auto it = bwtEncoded.begin(); it != bwtEncoded.end(); it++) {
    if (it == bwtEncodedKey) {
      fmIndex.push_back(last - first);
    } else {
      uint64_t index = cummulativeSum[*it];
      fmIndex.push_back(index);
      cummulativeSum[*it]++;
    }
  }
  auto tmp = last - 1;
  for (auto trav = bwtEncoded.end() - 1; trav != bwtEncodedKey;
       trav = bwtEncoded.begin() + fmIndex[trav - bwtEncoded.begin()]) {
    *tmp-- = *trav;
  }
  assert(tmp + 1 == first);
  return;
}

#endif // BWT_H_
