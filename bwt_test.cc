#include "bwt.hpp"

#include <gtest/gtest.h>

#include <algorithm>
#include <array>
#include <cstdint>
#include <deque>
#include <numeric>
#include <optional>
#include <random>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

namespace {

template <typename T>
bwt::Encoded<T> naive_bwt(const std::vector<T>& input) {
  const std::size_t n = input.size();
  if (n == 0) {
    return bwt::Encoded<T>{};
  }
  std::vector<std::size_t> rots(n + 1);
  std::iota(rots.begin(), rots.end(), std::size_t{0});
  std::sort(rots.begin(), rots.end(), [&](std::size_t a, std::size_t b) {
    for (std::size_t d = 0; d <= n; ++d) {
      const std::size_t pa = (a + d) % (n + 1);
      const std::size_t pb = (b + d) % (n + 1);
      if (pa == pb) {
        continue;
      }
      if (pa == n) {
        return true;  // '$' is strictly smaller than every element of T.
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

  bwt::Encoded<T> result;
  result.data.reserve(n);
  for (std::size_t r = 0; r <= n; ++r) {
    if (rots[r] == 0) {
      result.key = r;
    } else {
      result.data.push_back(input[rots[r] - 1]);
    }
  }
  return result;
}

template <typename T>
void expect_matches_oracle_and_roundtrips(const std::vector<T>& input) {
  const bwt::Encoded<T> expected = naive_bwt(input);
  const bwt::Encoded<T> actual = bwt::encode(input);
  EXPECT_EQ(actual, expected);

  const std::optional<std::vector<T>> decoded = bwt::decode(actual);
  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(*decoded, input);
}

struct NonDefaultItem {
  int val;
  explicit NonDefaultItem(int v) : val(v) {}
  bool operator<(const NonDefaultItem& other) const { return val < other.val; }
  bool operator==(const NonDefaultItem& other) const {
    return val == other.val;
  }
};

}  // namespace

TEST(BwtEncodeDecode, EmptyInputs) {
  const std::vector<int> empty_ints;
  const bwt::Encoded<int> encoded_ints = bwt::encode(empty_ints);
  EXPECT_TRUE(encoded_ints.data.empty());
  EXPECT_EQ(encoded_ints.key, 0u);
  const auto decoded_ints = bwt::decode(encoded_ints);
  ASSERT_TRUE(decoded_ints.has_value());
  EXPECT_TRUE(decoded_ints->empty());

  const bwt::Encoded<char> encoded_str = bwt::encode("");
  EXPECT_TRUE(encoded_str.data.empty());
  EXPECT_EQ(encoded_str.key, 0u);
  const auto decoded_str = bwt::decode("", 0);
  ASSERT_TRUE(decoded_str.has_value());
  EXPECT_TRUE(decoded_str->empty());
}

TEST(BwtEncodeDecode, TextbookGoldenCases) {
  struct Case {
    std::string input;
    std::string expected_bwt;
    std::size_t expected_key;
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
  for (const auto& c : cases) {
    const auto [data, key] = bwt::encode(c.input);
    EXPECT_EQ(std::string(data.begin(), data.end()), c.expected_bwt)
        << "input=" << c.input;
    EXPECT_EQ(key, c.expected_key) << "input=" << c.input;

    const auto dec = bwt::decode(data, key);
    ASSERT_TRUE(dec.has_value()) << "input=" << c.input;
    EXPECT_EQ(std::string(dec->begin(), dec->end()), c.input)
        << "input=" << c.input;
  }
}

TEST(BwtEncodeDecode, SmallIntVector) {
  const std::vector<int> small = {5, 1, 2, 1, 2};
  const auto [data, key] = bwt::encode(small.begin(), small.end());
  EXPECT_EQ(key, 5u);
  EXPECT_EQ(data, (std::vector<int>{2, 2, 5, 1, 1}));

  const auto decoded = bwt::decode(data.begin(), data.end(), key);
  ASSERT_TRUE(decoded.has_value());
  EXPECT_EQ(*decoded, small);
}

TEST(BwtEncodeDecode, SupportsContainersLiteralsPointersAndEmbeddedNulls) {
  // String literal (must not include trailing '\0').
  const auto lit_enc = bwt::encode("banana");
  EXPECT_EQ(std::string(lit_enc.data.begin(), lit_enc.data.end()), "annbaa");
  EXPECT_EQ(lit_enc.key, 4u);
  const auto lit_dec = bwt::decode("annbaa", 4);
  ASSERT_TRUE(lit_dec.has_value());
  EXPECT_EQ(std::string(lit_dec->begin(), lit_dec->end()), "banana");

  // Raw C pointer pair, C string pointer, and std::string_view.
  const char raw[] = {'g', 'a', 't', 'a', 'c', 'a'};
  EXPECT_EQ(bwt::encode(raw, raw + 6), bwt::encode(std::string_view("gataca")));
  const char* cstr = "banana";
  EXPECT_EQ(bwt::encode(cstr), lit_enc);

  // std::string with embedded NUL bytes.
  const std::string with_nuls("\0a\0b\0", 5);
  const auto nul_enc = bwt::encode(with_nuls);
  EXPECT_EQ(nul_enc.data.size(), 5u);
  const auto nul_dec = bwt::decode(nul_enc);
  ASSERT_TRUE(nul_dec.has_value());
  EXPECT_EQ(std::string(nul_dec->begin(), nul_dec->end()), with_nuls);

  // std::array and std::deque.
  const std::array<int, 5> arr = {5, 1, 2, 1, 2};
  const auto arr_enc = bwt::encode(arr);
  EXPECT_EQ(arr_enc.data, (std::vector<int>{2, 2, 5, 1, 1}));
  EXPECT_EQ(bwt::decode(arr_enc.data, arr_enc.key),
            (std::vector<int>{5, 1, 2, 1, 2}));

  const std::deque<int> deq = {3, 1, 4, 1, 5};
  const auto deq_enc = bwt::encode(deq);
  const auto deq_dec = bwt::decode(deq_enc);
  ASSERT_TRUE(deq_dec.has_value());
  EXPECT_EQ(*deq_dec, (std::vector<int>{3, 1, 4, 1, 5}));
}

TEST(BwtEncodeDecode, HandlesSignedAndBinaryBytes) {
  const std::vector<char> binary = {
      static_cast<char>(0x80), static_cast<char>(0xFF), static_cast<char>(0x00),
      static_cast<char>(0x7F), static_cast<char>(0x80), static_cast<char>(0x00),
      static_cast<char>(0xFF),
  };
  expect_matches_oracle_and_roundtrips(binary);
}

TEST(BwtEncodeDecode, HandlesLengthsExceedingByteThresholds) {
  for (std::size_t len : {127u, 128u, 129u, 255u, 256u, 300u}) {
    const std::vector<char> repeated(len, 'a');
    expect_matches_oracle_and_roundtrips(repeated);
  }
}

TEST(BwtEncodeDecode, ExhaustiveBinaryAgainstOracle) {
  for (std::size_t len = 0; len <= 10; ++len) {
    const std::size_t total = std::size_t{1} << len;
    for (std::size_t mask = 0; mask < total; ++mask) {
      std::vector<char> input(len);
      for (std::size_t i = 0; i < len; ++i) {
        input[i] = ((mask >> i) & 1u) ? 'b' : 'a';
      }
      expect_matches_oracle_and_roundtrips(input);
    }
  }
}

TEST(BwtEncodeDecode, ExhaustiveTernaryAgainstOracle) {
  for (std::size_t len = 0; len <= 6; ++len) {
    std::size_t total = 1;
    for (std::size_t i = 0; i < len; ++i) {
      total *= 3;
    }
    for (std::size_t code = 0; code < total; ++code) {
      std::vector<uint8_t> input(len);
      std::size_t x = code;
      for (std::size_t i = 0; i < len; ++i) {
        input[i] = static_cast<uint8_t>(x % 3);
        x /= 3;
      }
      expect_matches_oracle_and_roundtrips(input);
    }
  }
}

TEST(BwtEncodeDecode, RandomizedUint32AndByteAlphabetsAgainstOracle) {
  std::mt19937 rng(42);
  for (std::size_t len : {1u, 2u, 7u, 16u, 64u, 128u, 257u, 512u}) {
    std::uniform_int_distribution<uint32_t> dist_small(0, 3);
    std::vector<uint32_t> v_small(len);
    for (std::size_t i = 0; i < len; ++i) {
      v_small[i] = dist_small(rng);
    }
    expect_matches_oracle_and_roundtrips(v_small);

    std::uniform_int_distribution<uint32_t> dist_full(0, UINT32_MAX);
    std::vector<uint32_t> v_full(len);
    for (std::size_t i = 0; i < len; ++i) {
      v_full[i] = dist_full(rng);
    }
    expect_matches_oracle_and_roundtrips(v_full);

    std::uniform_int_distribution<int> dist_byte(-128, 127);
    std::vector<char> v_char(len);
    for (std::size_t i = 0; i < len; ++i) {
      v_char[i] = static_cast<char>(dist_byte(rng));
    }
    expect_matches_oracle_and_roundtrips(v_char);
  }
}

TEST(BwtEncodeDecode, WorksWithNonDefaultConstructibleCustomType) {
  const std::vector<NonDefaultItem> items = {
      NonDefaultItem(3), NonDefaultItem(1), NonDefaultItem(2),
      NonDefaultItem(1), NonDefaultItem(3),
  };
  expect_matches_oracle_and_roundtrips(items);
}

TEST(BwtEncodeDecode, RejectsInvalidKeysRangesAndMultiCyclePayloads) {
  const std::vector<int> v = {1, 2, 3};
  EXPECT_THROW(bwt::encode(v.end(), v.begin()), std::invalid_argument);
  EXPECT_THROW(bwt::decode(v.end(), v.begin(), 0), std::invalid_argument);

  // Invalid keys.
  EXPECT_FALSE(bwt::decode(v, 0).has_value());
  EXPECT_FALSE(bwt::decode(v, 4).has_value());
  const std::vector<int> empty;
  EXPECT_TRUE(bwt::decode(empty, 0).has_value());
  EXPECT_FALSE(bwt::decode(empty, 1).has_value());

  // Multi-cycle / corrupted payloads (LF-permutation splits into >1 cycle).
  EXPECT_FALSE(bwt::decode("ab", 1).has_value());
  EXPECT_FALSE(bwt::decode("aab", 1).has_value());
  EXPECT_FALSE(bwt::decode("baab", 2).has_value());
  EXPECT_FALSE(bwt::decode(std::vector<uint32_t>{1, 2}, 1).has_value());
  EXPECT_FALSE(bwt::decode(std::vector<uint32_t>{2, 1, 1, 2}, 2).has_value());
}
