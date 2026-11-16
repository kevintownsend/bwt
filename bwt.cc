#include "bwt.hpp"

/**
 * The Burrows-Wheeler transform of the string.
 *
 * @param first Pointer to the first character in the string.
 * @param last Pointer to one past the last character in the string.
 * @return The encoded string as a char vector and row index of the virtual
 * sentinel '$'.
 */
std::pair<std::vector<char>, size_t> bwt::charBwt(const char *first,
                                                  const char *last) {
  return bwt::bwt(first, last);
}

std::pair<std::vector<char>, size_t>
bwt::charBwt(std::vector<char>::iterator first,
             std::vector<char>::iterator last) {
  return bwt::bwt(first, last);
}

/**
 * The inverse operation of charBwt.
 *
 * @param encoded_bwt A Burrows-Wheeler transformed vector.
 * @param key The row index of the virtual sentinel '$'.
 * @return The decoded character vector (empty if key/input is invalid).
 */
std::vector<char> bwt::iCharBwt(const std::vector<char> &encoded_bwt,
                                const size_t key) {
  std::pair<std::vector<char>, bool> decoded =
      bwt::iBwt(encoded_bwt.begin(), encoded_bwt.end(), key);
  if (!decoded.second) {
    return std::vector<char>();
  }
  return decoded.first;
}
