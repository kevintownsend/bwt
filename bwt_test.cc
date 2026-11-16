#include <gtest/gtest.h>

#include <algorithm>
#include <cstdint>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <tuple>
#include <vector>

#include "bwt.hpp"

namespace {

template <typename T>
std::pair<std::vector<T>, size_t> naive_bwt(const std::vector<T> &input) {
  const size_t n = input.size();
  if (n == 0) {
    return {{}, 0};
  }
  std::vector<size_t> rots(n + 1);
  std::iota(rots.begin(), rots.end(), size_t{0});
  std::sort(rots.begin(), rots.end(), [&](size_t a, size_t b) {
    for (size_t d = 0; d <= n; ++d) {
      const size_t pa = (a + d) % (n + 1);
      const size_t pb = (b + d) % (n + 1);
      if (pa == pb) {
        continue;
      }
      if (pa == n) {
        return true; // '$' is strictly smaller than every element of T.
      }
      if (pb == n) {
        return false;
      }
      if (input[pa] < input[pb]) {
        return true;
      }
      if (input[pb] < input[pa]) {
        return false;
      }
    }
    return false;
  });

  std::vector<T> out;
  out.reserve(n);
  size_t key = 0;
  for (size_t r = 0; r <= n; ++r) {
    if (rots[r] == 0) {
      key = r;
    } else {
      out.push_back(input[rots[r] - 1]);
    }
  }
  return {out, key};
}

template <typename T>
void expect_matches_oracle_and_roundtrips(const std::vector<T> &input) {
  const auto expected = naive_bwt(input);
  const auto actual = bwt::bwt(input.begin(), input.end());
  EXPECT_EQ(actual.second, expected.second);
  EXPECT_EQ(actual.first, expected.first);

  const auto decoded =
      bwt::iBwt(actual.first.begin(), actual.first.end(), actual.second);
  EXPECT_TRUE(decoded.second);
  EXPECT_EQ(decoded.first, input);
}

struct NonDefaultItem {
  int val;
  explicit NonDefaultItem(int v) : val(v) {}
  bool operator<(const NonDefaultItem &other) const { return val < other.val; }
  bool operator==(const NonDefaultItem &other) const {
    return val == other.val;
  }
};

} // namespace

TEST(CharBwtTest, EmptyCharBwt) {
  std::vector<char> empty;
  std::vector<char> ret;
  size_t key = 0;
  std::tie(ret, key) = bwt::charBwt(empty.data(), empty.data());
  ASSERT_TRUE(ret.empty());
  ASSERT_EQ(key, 0u);
}

TEST(ICharBwtTest, EmptyICharBwt) {
  std::vector<char> empty;
  std::vector<char> decoded = bwt::iCharBwt(empty, 0);
  ASSERT_TRUE(decoded.empty());
}

TEST(CharBwtTest, BananaCharBwt) {
  std::string banana_str = "banana";
  std::vector<char> banana(banana_str.begin(), banana_str.end());
  std::vector<char> ret;
  size_t key = 0;
  std::tie(ret, key) =
      bwt::charBwt(banana.data(), banana.data() + banana.size());

  std::string bwt_str = "annbaa";
  std::vector<char> expected(bwt_str.begin(), bwt_str.end());
  ASSERT_EQ(expected, ret);
  ASSERT_EQ(key, 4u);

  std::vector<char> decoded_str = bwt::iCharBwt(ret, key);
  ASSERT_EQ(banana, decoded_str);
}

TEST(CharBwtTest, VectorIteratorOverload) {
  std::string s = "mississippi";
  std::vector<char> input(s.begin(), s.end());
  const auto encoded = bwt::charBwt(input.begin(), input.end());
  const std::string expected_str = "ipssmpissii";
  ASSERT_EQ(std::string(encoded.first.begin(), encoded.first.end()),
            expected_str);
  ASSERT_EQ(encoded.second, 5u);
  ASSERT_EQ(bwt::iCharBwt(encoded.first, encoded.second), input);
}

TEST(CharBwtTest, HandlesSignedAndBinaryBytes) {
  std::vector<char> binary = {
      static_cast<char>(0x80), static_cast<char>(0xFF),
      static_cast<char>(0x00), static_cast<char>(0x7F),
      static_cast<char>(0x80), static_cast<char>(0x00),
      static_cast<char>(0xFF),
  };
  const auto encoded =
      bwt::charBwt(binary.data(), binary.data() + binary.size());
  const auto expected = naive_bwt(binary);
  ASSERT_EQ(encoded.second, expected.second);
  ASSERT_EQ(encoded.first, expected.first);
  ASSERT_EQ(bwt::iCharBwt(encoded.first, encoded.second), binary);
}

TEST(CharBwtTest, HandlesLengthsExceedingSignedAndUnsignedCharMax) {
  for (size_t len : {127u, 128u, 129u, 255u, 256u, 300u}) {
    std::vector<char> repeated(len, 'a');
    const auto encoded =
        bwt::charBwt(repeated.data(), repeated.data() + repeated.size());
    const auto expected = naive_bwt(repeated);
    ASSERT_EQ(encoded.second, expected.second);
    ASSERT_EQ(encoded.first, expected.first);
    ASSERT_EQ(bwt::iCharBwt(encoded.first, encoded.second), repeated);
  }
}

TEST(Bwt, Empty) {
  std::vector<int> empty;
  const auto res = bwt::bwt(empty.begin(), empty.end());
  ASSERT_TRUE(res.first.empty());
  ASSERT_EQ(res.second, 0u);
}

TEST(Bwt, Small) {
  std::vector<int> small = {5, 1, 2, 1, 2};
  std::vector<int> ret;
  size_t key = 0;
  std::tie(ret, key) = bwt::bwt(small.begin(), small.end());
  ASSERT_EQ(key, 5u);
  std::vector<int> expected = {2, 2, 5, 1, 1};
  ASSERT_EQ(ret, expected);
}

TEST(IBwt, Small) {
  std::vector<int> small = {2, 2, 5, 1, 1};
  size_t key = 5;
  bool success = false;
  std::vector<int> ret;
  std::tie(ret, success) = bwt::iBwt(small.begin(), small.end(), key);

  ASSERT_TRUE(success);

  std::vector<int> expected = {5, 1, 2, 1, 2};
  ASSERT_EQ(ret, expected);
}

TEST(Bwt, TextbookGoldenCases) {
  struct Case {
    std::string input;
    std::string expected_bwt;
    size_t expected_key;
  };
  const std::vector<Case> cases = {
      {"a", "a", 1},
      {"aa", "aa", 2},
      {"aab", "baa", 1},
      {"aba", "aba", 2},
      {"abba", "abba", 2},
      {"aaaa", "aaaa", 4},
      {"gataca", "actgaa", 5},
      {"banana", "annbaa", 4},
      {"mississippi", "ipssmpissii", 5},
      {"abacab", "bcbaaa", 2},
      {"abracadabra", "ardrcaaaabb", 3},
  };
  for (const auto &c : cases) {
    const auto enc = bwt::bwt(c.input.begin(), c.input.end());
    EXPECT_EQ(std::string(enc.first.begin(), enc.first.end()), c.expected_bwt)
        << "input=" << c.input;
    EXPECT_EQ(enc.second, c.expected_key) << "input=" << c.input;

    const auto dec = bwt::iBwt(enc.first.begin(), enc.first.end(), enc.second);
    EXPECT_TRUE(dec.second) << "input=" << c.input;
    EXPECT_EQ(std::string(dec.first.begin(), dec.first.end()), c.input)
        << "input=" << c.input;
  }
}

TEST(Bwt, ExhaustiveBinaryAgainstOracle) {
  for (size_t len = 0; len <= 9; ++len) {
    const size_t total = size_t{1} << len;
    for (size_t mask = 0; mask < total; ++mask) {
      std::vector<char> input(len);
      for (size_t i = 0; i < len; ++i) {
        input[i] = ((mask >> i) & 1u) ? 'b' : 'a';
      }
      expect_matches_oracle_and_roundtrips(input);
    }
  }
}

TEST(Bwt, ExhaustiveTernaryAgainstOracle) {
  for (size_t len = 0; len <= 6; ++len) {
    size_t total = 1;
    for (size_t i = 0; i < len; ++i) {
      total *= 3;
    }
    for (size_t code = 0; code < total; ++code) {
      std::vector<uint8_t> input(len);
      size_t x = code;
      for (size_t i = 0; i < len; ++i) {
        input[i] = static_cast<uint8_t>(x % 3);
        x /= 3;
      }
      expect_matches_oracle_and_roundtrips(input);
    }
  }
}

TEST(Bwt, RandomizedUint32AndByteAlphabetsAgainstOracle) {
  std::mt19937 rng(42);
  for (size_t len : {1u, 2u, 7u, 16u, 64u, 128u, 257u}) {
    // Low-cardinality uint32_t to force heavy suffix ties.
    std::uniform_int_distribution<uint32_t> dist_small(0, 3);
    std::vector<uint32_t> v_small(len);
    for (size_t i = 0; i < len; ++i) {
      v_small[i] = dist_small(rng);
    }
    expect_matches_oracle_and_roundtrips(v_small);

    // Full-range uint32_t.
    std::uniform_int_distribution<uint32_t> dist_full(0, UINT32_MAX);
    std::vector<uint32_t> v_full(len);
    for (size_t i = 0; i < len; ++i) {
      v_full[i] = dist_full(rng);
    }
    expect_matches_oracle_and_roundtrips(v_full);

    // Full byte range (including negative signed char values).
    std::uniform_int_distribution<int> dist_byte(-128, 127);
    std::vector<char> v_char(len);
    for (size_t i = 0; i < len; ++i) {
      v_char[i] = static_cast<char>(dist_byte(rng));
    }
    expect_matches_oracle_and_roundtrips(v_char);
  }
}

TEST(Bwt, WorksWithNonDefaultConstructibleCustomType) {
  std::vector<NonDefaultItem> items = {
      NonDefaultItem(3), NonDefaultItem(1), NonDefaultItem(2),
      NonDefaultItem(1), NonDefaultItem(3),
  };
  expect_matches_oracle_and_roundtrips(items);
}

TEST(Bwt, RejectsInvalidKeysAndRanges) {
  std::vector<int> v = {1, 2, 3};
  EXPECT_THROW(bwt::bwt(v.end(), v.begin()), std::invalid_argument);
  EXPECT_THROW(bwt::iBwt(v.end(), v.begin(), 0), std::invalid_argument);

  EXPECT_FALSE(bwt::iBwt(v.begin(), v.end(), 0).second);
  EXPECT_FALSE(bwt::iBwt(v.begin(), v.end(), 4).second);
  std::vector<int> empty;
  EXPECT_TRUE(bwt::iBwt(empty.begin(), empty.end(), 0).second);
  EXPECT_FALSE(bwt::iBwt(empty.begin(), empty.end(), 1).second);
}
