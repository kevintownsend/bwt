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
#include <array>
#include <cassert>
#include <climits>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <set>
#include <stdexcept>
#include <stdint.h>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace bwt {
std::pair<std::vector<char>, size_t> charBwt(const char *first,
                                             const char *last);

std::pair<std::vector<char>, size_t> charBwt(std::vector<char>::iterator first,
                                             std::vector<char>::iterator last);
std::vector<char> iCharBwt(const std::vector<char> &encoded_bwt,
                           const size_t key);

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
} // namespace bwt

namespace bwt {
namespace detail {

template <typename T>
struct is_byte_integral
    : std::integral_constant<bool, sizeof(T) == 1 &&
                                       std::is_integral<T>::value &&
                                       CHAR_BIT == 8> {};

inline std::vector<size_t> suffix_array_from_ranks(std::vector<size_t> &rank,
                                                   size_t alphabet_size) {
  const size_t m = rank.size();
  std::vector<size_t> sa(m);
  std::vector<size_t> tmp(m);
  std::vector<size_t> new_rank(m);
  std::vector<size_t> cnt(std::max(m, alphabet_size));

  // Initial counting sort on 1-element ranks.
  std::fill(cnt.begin(), cnt.begin() + alphabet_size, 0);
  for (size_t i = 0; i < m; ++i) {
    ++cnt[rank[i]];
  }
  for (size_t c = 1; c < alphabet_size; ++c) {
    cnt[c] += cnt[c - 1];
  }
  for (size_t i = m; i > 0; --i) {
    const size_t idx = i - 1;
    sa[--cnt[rank[idx]]] = idx;
  }

  for (size_t step = 1; step < m && alphabet_size < m; step *= 2) {
    // Sort indices by second key: rank[i + step] (0 if i + step >= m).
    size_t pos = 0;
    for (size_t i = m - step; i < m; ++i) {
      tmp[pos++] = i;
    }
    for (size_t k = 0; k < m; ++k) {
      if (sa[k] >= step) {
        tmp[pos++] = sa[k] - step;
      }
    }

    // Stable counting sort by first key: rank[tmp[k]].
    std::fill(cnt.begin(), cnt.begin() + alphabet_size, 0);
    for (size_t k = 0; k < m; ++k) {
      ++cnt[rank[tmp[k]]];
    }
    for (size_t c = 1; c < alphabet_size; ++c) {
      cnt[c] += cnt[c - 1];
    }
    for (size_t k = m; k > 0; --k) {
      const size_t idx = tmp[k - 1];
      sa[--cnt[rank[idx]]] = idx;
    }

    // Recompute equivalence classes.
    new_rank[sa[0]] = 0;
    size_t classes = 1;
    for (size_t k = 1; k < m; ++k) {
      const size_t cur = sa[k];
      const size_t prev = sa[k - 1];
      const size_t cur2 = (cur + step < m) ? rank[cur + step] : 0;
      const size_t prev2 = (prev + step < m) ? rank[prev + step] : 0;
      if (rank[cur] != rank[prev] || cur2 != prev2) {
        ++classes;
      }
      new_rank[cur] = classes - 1;
    }
    rank.swap(new_rank);
    alphabet_size = classes;
  }
  return sa;
}

template <typename T>
inline size_t byte_bucket(T value) {
  return static_cast<size_t>(
      static_cast<int>(value) -
      static_cast<int>(std::numeric_limits<T>::min()));
}

template <typename RandomAccessIterator>
size_t build_initial_ranks(RandomAccessIterator first, size_t n,
                           std::vector<size_t> &rank, std::true_type) {
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type T;
  std::array<size_t, 256> char_rank{};
  for (size_t i = 0; i < n; ++i) {
    char_rank[byte_bucket<T>(first[i])] = 1;
  }
  size_t alphabet_size = 1; // 0 is reserved for the sentinel '$'.
  for (size_t b = 0; b < 256; ++b) {
    if (char_rank[b] != 0) {
      char_rank[b] = alphabet_size++;
    }
  }
  rank.resize(n + 1);
  for (size_t i = 0; i < n; ++i) {
    rank[i] = char_rank[byte_bucket<T>(first[i])];
  }
  rank[n] = 0;
  return alphabet_size;
}

template <typename RandomAccessIterator>
size_t build_initial_ranks(RandomAccessIterator first, size_t n,
                           std::vector<size_t> &rank, std::false_type) {
  std::vector<size_t> order(n);
  for (size_t i = 0; i < n; ++i) {
    order[i] = i;
  }
  std::sort(order.begin(), order.end(),
            [&first](size_t a, size_t b) { return first[a] < first[b]; });
  rank.resize(n + 1);
  size_t r = 1; // 0 is reserved for the sentinel '$'.
  rank[order[0]] = r;
  for (size_t i = 1; i < n; ++i) {
    if (first[order[i - 1]] < first[order[i]]) {
      ++r;
    }
    rank[order[i]] = r;
  }
  rank[n] = 0;
  return r + 1;
}

template <typename RandomAccessIterator>
std::vector<size_t> build_lf_mapping(RandomAccessIterator first, size_t n,
                                     size_t key, std::true_type) {
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type T;
  std::array<size_t, 256> counts{};
  for (size_t i = 0; i < n; ++i) {
    ++counts[byte_bucket<T>(first[i])];
  }
  std::array<size_t, 256> offsets{};
  size_t sum = 1; // Row 0 in F is reserved for the sentinel '$'.
  for (size_t b = 0; b < 256; ++b) {
    offsets[b] = sum;
    sum += counts[b];
  }
  std::vector<size_t> lf(n + 1);
  for (size_t r = 0; r <= n; ++r) {
    if (r == key) {
      lf[r] = 0;
    } else {
      const size_t idx = (r > key) ? r - 1 : r;
      lf[r] = offsets[byte_bucket<T>(first[idx])]++;
    }
  }
  return lf;
}

template <typename RandomAccessIterator>
std::vector<size_t> build_lf_mapping(RandomAccessIterator first, size_t n,
                                     size_t key, std::false_type) {
  std::vector<size_t> order(n);
  for (size_t i = 0; i < n; ++i) {
    order[i] = i;
  }
  std::stable_sort(order.begin(), order.end(), [&first](size_t a, size_t b) {
    return first[a] < first[b];
  });
  std::vector<size_t> lf(n + 1);
  lf[key] = 0;
  for (size_t k = 0; k < n; ++k) {
    const size_t idx = order[k];
    const size_t row = (idx >= key) ? idx + 1 : idx;
    lf[row] = k + 1;
  }
  return lf;
}

} // namespace detail
} // namespace bwt

/**
 * The templated Burrows-Wheeler transform.
 *
 * Uses a virtual end-of-input sentinel '$' that compares strictly smaller than
 * all elements of the input sequence.
 *
 * @param first Iterator to the first value.
 * @param last Iterator to one past the last value.
 * @return The transformed vector (with the sentinel omitted) and the 0-based
 *         row index where the sentinel occurs in the full transform.
 */
template <typename RandomAccessIterator>
std::pair<std::vector<
              typename std::iterator_traits<RandomAccessIterator>::value_type>,
          size_t>
bwt::bwt(RandomAccessIterator first, RandomAccessIterator last) {
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type T;
  if (last < first) {
    throw std::invalid_argument("bwt::bwt: invalid iterator range");
  }
  const size_t n = static_cast<size_t>(last - first);
  if (n == 0) {
    return std::make_pair(std::vector<T>(), size_t(0));
  }

  std::vector<size_t> rank;
  const size_t alphabet_size = detail::build_initial_ranks(
      first, n, rank, detail::is_byte_integral<T>());
  const std::vector<size_t> sa =
      detail::suffix_array_from_ranks(rank, alphabet_size);

  std::vector<T> ret;
  ret.reserve(n);
  size_t key = 0;
  for (size_t r = 0; r <= n; ++r) {
    if (sa[r] == 0) {
      key = r;
    } else {
      ret.push_back(first[sa[r] - 1]);
    }
  }
  return std::make_pair(ret, key);
}

/**
 * The templated inverse Burrows-Wheeler transform.
 *
 * Expects the virtual sentinel '$' convention used by bwt::bwt (sentinel
 * strictly smaller than all elements, omitted from [first, last) at index key).
 *
 * @param first Iterator to the first value.
 * @param last Iterator to one past the last value.
 * @param key The row index where the virtual sentinel '$' belongs.
 * @return The decoded vector and success boolean.
 */
template <typename RandomAccessIterator>
std::pair<std::vector<
              typename std::iterator_traits<RandomAccessIterator>::value_type>,
          bool>
bwt::iBwt(RandomAccessIterator first, RandomAccessIterator last,
          const size_t key) {
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type T;
  if (last < first) {
    throw std::invalid_argument("bwt::iBwt: invalid iterator range");
  }
  const size_t n = static_cast<size_t>(last - first);
  if (n == 0) {
    return std::make_pair(std::vector<T>(), key == 0);
  }
  if (key == 0 || key > n) {
    return std::make_pair(std::vector<T>(), false);
  }

  const std::vector<size_t> lf =
      detail::build_lf_mapping(first, n, key, detail::is_byte_integral<T>());

  std::vector<size_t> seq(n);
  size_t curr = 0;
  for (size_t i = 0; i < n; ++i) {
    if (curr == key) {
      return std::make_pair(std::vector<T>(), false);
    }
    const size_t eff = (curr > key) ? curr - 1 : curr;
    seq[n - 1 - i] = eff;
    curr = lf[curr];
  }
  if (curr != key) {
    return std::make_pair(std::vector<T>(), false);
  }

  std::vector<T> ret;
  ret.reserve(n);
  for (size_t idx : seq) {
    ret.push_back(first[idx]);
  }
  return std::make_pair(ret, true);
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
