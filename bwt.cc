#include "bwt.hpp"

#include <numeric>

/**
 * The Burrows-Wheeler tranform of the string.
 *
 * @param first Pointer to the first charater in the string.
 * @param last Pointer to the one over last charater in the string.
 * @return The encoded string as a char vector and relative location of the end
 * character.
 */
std::pair<std::vector<char>, size_t> bwt::charBwt(char *first, char *last) {
  size_t size = last - first;
  // TODO(kevintownsend): check inputs.
  if (size == 0) {
    return make_pair(std::vector<char>(), 0);
  }
  constexpr size_t radix = 256; // UCHAR_MAX + 1.
  // Get counts
  std::vector<char> counts(radix);
  for (char *it = first; it != last; ++it) {
    ++counts[*it];
  }

  // Create prefix sums.
  std::vector<char> offsets(radix + 1);
  std::partial_sum(counts.begin(), counts.end(), offsets.begin() + 1);
  for (size_t i = first[0] + 1; i < radix; ++i) {
    --offsets[i];
  }

  // Create the return vector (the bwt transformation).
  std::vector<char> ret(size);

  for (size_t i = 0; i < size - 1; ++i) {
    ret[offsets[first[i + 1]]++] = first[i];
  }
  ret[size - 1] = first[size - 1];
  return std::make_pair(ret, offsets[first[0]]);
}

/**
 * The inverse operation of charBwt.
 *
 * @param encododed_bwt A Burrows-Wheeler transformed vector.
 * @param key The relative location of the end character.
 * @return The decoded character vector.
 */
std::vector<char> bwt::iCharBwt(const std::vector<char> &encoded_bwt,
                                const size_t key) {
  size_t n = encoded_bwt.size();
  std::vector<char> ret(n);
  constexpr size_t radix = 256;
  if (encoded_bwt.empty()) {
    return ret;
  }

  std::vector<size_t> count(radix), offset(radix + 1);

  for (char val : encoded_bwt) {
    count[val]++;
  }
  std::partial_sum(count.begin(), count.end(), offset.begin());

  size_t curr_offset = n;
  size_t key_offset = key;
  for (size_t i = 0; i < n; i++) {
    if (curr_offset == key_offset) {
      // TODO(kevintownsend): Return an error.
    }
    size_t effective_offset =
        (curr_offset > key_offset) ? curr_offset - 1 : curr_offset;
    char c = encoded_bwt[effective_offset];
    ret[n - i - 1] = c;
    curr_offset = --offset[(size_t)c];
  }
  return ret;
}
