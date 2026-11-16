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

#ifndef BWT_HPP_
#define BWT_HPP_

#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <iterator>
#include <limits>
#include <optional>
#include <stdexcept>
#include <string_view>
#include <type_traits>
#include <utility>
#include <vector>

namespace bwt {

/**
 * Result of forward Burrows-Wheeler encoding.
 *
 * @tparam T Element type of the encoded sequence.
 */
template <typename T>
struct Encoded {
  std::vector<T> data;
  std::size_t key = 0;

  bool operator==(const Encoded& other) const {
    return key == other.key && data == other.data;
  }

  bool operator!=(const Encoded& other) const { return !(*this == other); }
};

namespace detail {

template <typename T>
inline constexpr bool is_byte_integral_v =
    sizeof(T) == 1 && std::is_integral_v<T> && CHAR_BIT == 8;

template <typename It>
inline constexpr bool is_random_access_it_v =
    std::is_base_of_v<std::random_access_iterator_tag,
                      typename std::iterator_traits<It>::iterator_category>;

template <typename T, typename = void>
struct is_less_comparable : std::false_type {};

template <typename T>
struct is_less_comparable<
    T, std::void_t<decltype(static_cast<bool>(std::declval<const T&>() <
                                              std::declval<const T&>()))>>
    : std::true_type {};

template <typename Range, typename = void>
struct is_random_access_range : std::false_type {};

template <typename Range>
struct is_random_access_range<
    Range, std::void_t<decltype(std::begin(std::declval<const Range&>())),
                       decltype(std::end(std::declval<const Range&>()))>>
    : std::integral_constant<
          bool, !std::is_array_v<std::remove_reference_t<Range>> &&
                    !std::is_convertible_v<const Range&, std::string_view> &&
                    is_random_access_it_v<decltype(std::begin(
                        std::declval<const Range&>()))>> {};

template <typename T>
inline std::size_t byte_bucket(T value) {
  return static_cast<std::size_t>(
      static_cast<int>(value) -
      static_cast<int>(std::numeric_limits<T>::min()));
}

// Builds the suffix array of [0..n] (where index n is the unique minimal '$')
// via O(n log n) prefix doubling with stable counting sorts.
inline std::vector<std::size_t> suffix_array_from_ranks(
    std::vector<std::size_t>& rank, std::size_t alphabet_size) {
  const std::size_t m = rank.size();
  std::vector<std::size_t> sa(m);
  std::vector<std::size_t> tmp(m);
  std::vector<std::size_t> new_rank(m);
  std::vector<std::size_t> cnt(std::max(m, alphabet_size));

  std::fill(cnt.begin(), cnt.begin() + alphabet_size, 0);
  for (std::size_t i = 0; i < m; ++i) {
    ++cnt[rank[i]];
  }
  for (std::size_t c = 1; c < alphabet_size; ++c) {
    cnt[c] += cnt[c - 1];
  }
  for (std::size_t i = m; i > 0; --i) {
    const std::size_t idx = i - 1;
    sa[--cnt[rank[idx]]] = idx;
  }

  for (std::size_t step = 1; step < m && alphabet_size < m; step *= 2) {
    // Sort by second key: rank[i + step] (0 if i + step >= m).
    std::size_t pos = 0;
    for (std::size_t i = m - step; i < m; ++i) {
      tmp[pos++] = i;
    }
    for (std::size_t k = 0; k < m; ++k) {
      if (sa[k] >= step) {
        tmp[pos++] = sa[k] - step;
      }
    }

    // Stable counting sort by first key: rank[tmp[k]].
    std::fill(cnt.begin(), cnt.begin() + alphabet_size, 0);
    for (std::size_t k = 0; k < m; ++k) {
      ++cnt[rank[tmp[k]]];
    }
    for (std::size_t c = 1; c < alphabet_size; ++c) {
      cnt[c] += cnt[c - 1];
    }
    for (std::size_t k = m; k > 0; --k) {
      const std::size_t idx = tmp[k - 1];
      sa[--cnt[rank[idx]]] = idx;
    }

    // Recompute equivalence classes.
    new_rank[sa[0]] = 0;
    std::size_t classes = 1;
    for (std::size_t k = 1; k < m; ++k) {
      const std::size_t cur = sa[k];
      const std::size_t prev = sa[k - 1];
      const std::size_t cur2 = (cur + step < m) ? rank[cur + step] : 0;
      const std::size_t prev2 = (prev + step < m) ? rank[prev + step] : 0;
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

template <typename RandomAccessIt>
std::size_t build_initial_ranks(RandomAccessIt first, std::size_t n,
                                std::vector<std::size_t>& rank) {
  using T = typename std::iterator_traits<RandomAccessIt>::value_type;
  rank.resize(n + 1);
  rank[n] = 0;  // Reserve rank 0 for the virtual sentinel '$'.

  if constexpr (is_byte_integral_v<T>) {
    std::array<std::size_t, 256> char_rank{};
    for (std::size_t i = 0; i < n; ++i) {
      char_rank[byte_bucket<T>(first[i])] = 1;
    }
    std::size_t alphabet_size = 1;
    for (std::size_t b = 0; b < 256; ++b) {
      if (char_rank[b] != 0) {
        char_rank[b] = alphabet_size++;
      }
    }
    for (std::size_t i = 0; i < n; ++i) {
      rank[i] = char_rank[byte_bucket<T>(first[i])];
    }
    return alphabet_size;
  } else {
    std::vector<std::size_t> order(n);
    for (std::size_t i = 0; i < n; ++i) {
      order[i] = i;
    }
    std::sort(
        order.begin(), order.end(),
        [&first](std::size_t a, std::size_t b) { return first[a] < first[b]; });
    std::size_t r = 1;
    rank[order[0]] = r;
    for (std::size_t i = 1; i < n; ++i) {
      if (first[order[i - 1]] < first[order[i]]) {
        ++r;
      }
      rank[order[i]] = r;
    }
    return r + 1;
  }
}

template <typename RandomAccessIt>
std::vector<std::size_t> build_lf_mapping(RandomAccessIt first, std::size_t n,
                                          std::size_t key) {
  using T = typename std::iterator_traits<RandomAccessIt>::value_type;
  std::vector<std::size_t> lf(n + 1);
  lf[key] = 0;

  if constexpr (is_byte_integral_v<T>) {
    std::array<std::size_t, 256> counts{};
    for (std::size_t i = 0; i < n; ++i) {
      ++counts[byte_bucket<T>(first[i])];
    }
    std::array<std::size_t, 256> offsets{};
    std::size_t sum = 1;  // Row 0 in F is reserved for the sentinel '$'.
    for (std::size_t b = 0; b < 256; ++b) {
      offsets[b] = sum;
      sum += counts[b];
    }
    for (std::size_t r = 0; r <= n; ++r) {
      if (r != key) {
        const std::size_t idx = (r > key) ? r - 1 : r;
        lf[r] = offsets[byte_bucket<T>(first[idx])]++;
      }
    }
  } else {
    std::vector<std::size_t> order(n);
    for (std::size_t i = 0; i < n; ++i) {
      order[i] = i;
    }
    std::stable_sort(
        order.begin(), order.end(),
        [&first](std::size_t a, std::size_t b) { return first[a] < first[b]; });
    for (std::size_t k = 0; k < n; ++k) {
      const std::size_t idx = order[k];
      const std::size_t row = (idx >= key) ? idx + 1 : idx;
      lf[row] = k + 1;
    }
  }
  return lf;
}

}  // namespace detail

/**
 * Computes the forward Burrows-Wheeler transform of [first, last).
 *
 * Uses a virtual end-of-input sentinel '$' that compares strictly smaller than
 * all elements of the input sequence.
 *
 * @param first Iterator to the first element.
 * @param last Iterator one past the last element.
 * @return Encoded sequence (with '$' omitted) and the row index of '$'.
 */
template <typename RandomAccessIt>
Encoded<typename std::iterator_traits<RandomAccessIt>::value_type> encode(
    RandomAccessIt first, RandomAccessIt last) {
  static_assert(detail::is_random_access_it_v<RandomAccessIt>,
                "bwt::encode requires random-access iterators");
  using T = typename std::iterator_traits<RandomAccessIt>::value_type;
  static_assert(detail::is_less_comparable<T>::value,
                "bwt::encode requires elements comparable with operator<");

  if (last < first) {
    throw std::invalid_argument("bwt::encode: invalid iterator range");
  }
  const std::size_t n = static_cast<std::size_t>(last - first);
  if (n == 0) {
    return Encoded<T>{};
  }

  std::vector<std::size_t> rank;
  const std::size_t alphabet_size = detail::build_initial_ranks(first, n, rank);
  const std::vector<std::size_t> sa =
      detail::suffix_array_from_ranks(rank, alphabet_size);

  Encoded<T> result;
  result.data.reserve(n);
  for (std::size_t r = 0; r <= n; ++r) {
    if (sa[r] == 0) {
      result.key = r;
    } else {
      result.data.push_back(first[sa[r] - 1]);
    }
  }
  return result;
}

/**
 * Computes the forward Burrows-Wheeler transform of a contiguous or
 * random-access range.
 */
template <
    typename Range,
    std::enable_if_t<detail::is_random_access_range<Range>::value, int> = 0>
auto encode(const Range& input) {
  return encode(std::begin(input), std::end(input));
}

/**
 * Computes the forward Burrows-Wheeler transform of a character string.
 */
inline Encoded<char> encode(std::string_view input) {
  return encode(input.begin(), input.end());
}

/**
 * Inverts a Burrows-Wheeler encoded sequence produced by bwt::encode.
 *
 * @param first Iterator to the first encoded element.
 * @param last Iterator one past the last encoded element.
 * @param key Row index where the omitted virtual sentinel '$' belongs.
 * @return The original sequence, or std::nullopt if key or payload is invalid.
 */
template <typename RandomAccessIt>
std::optional<
    std::vector<typename std::iterator_traits<RandomAccessIt>::value_type>>
decode(RandomAccessIt first, RandomAccessIt last, std::size_t key) {
  static_assert(detail::is_random_access_it_v<RandomAccessIt>,
                "bwt::decode requires random-access iterators");
  using T = typename std::iterator_traits<RandomAccessIt>::value_type;
  static_assert(detail::is_less_comparable<T>::value,
                "bwt::decode requires elements comparable with operator<");

  if (last < first) {
    throw std::invalid_argument("bwt::decode: invalid iterator range");
  }
  const std::size_t n = static_cast<std::size_t>(last - first);
  if (n == 0) {
    if (key != 0) {
      return std::nullopt;
    }
    return std::vector<T>{};
  }
  if (key == 0 || key > n) {
    return std::nullopt;
  }

  const std::vector<std::size_t> lf = detail::build_lf_mapping(first, n, key);

  std::vector<std::size_t> seq(n);
  std::size_t curr = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (curr == key) {
      return std::nullopt;
    }
    const std::size_t eff = (curr > key) ? curr - 1 : curr;
    seq[n - 1 - i] = eff;
    curr = lf[curr];
  }
  if (curr != key) {
    return std::nullopt;
  }

  std::vector<T> ret;
  ret.reserve(n);
  for (std::size_t idx : seq) {
    ret.push_back(first[idx]);
  }
  return ret;
}

/**
 * Inverts a Burrows-Wheeler encoded range produced by bwt::encode.
 */
template <
    typename Range,
    std::enable_if_t<detail::is_random_access_range<Range>::value, int> = 0>
auto decode(const Range& encoded, std::size_t key) {
  return decode(std::begin(encoded), std::end(encoded), key);
}

/**
 * Inverts a Burrows-Wheeler encoded character string produced by bwt::encode.
 */
inline std::optional<std::vector<char>> decode(std::string_view encoded,
                                               std::size_t key) {
  return decode(encoded.begin(), encoded.end(), key);
}

/**
 * Convenience overload to decode directly from an Encoded<T>.
 */
template <typename T>
std::optional<std::vector<T>> decode(const Encoded<T>& encoded) {
  return decode(encoded.data, encoded.key);
}

}  // namespace bwt

#endif  // BWT_HPP_
