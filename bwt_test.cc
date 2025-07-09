#include <gtest/gtest.h>

#include "bwt.hpp"

TEST(CharBwtTest, EmptyCharBwt) {
  std::vector<char> empty;
  bwt::charBwt(&*empty.begin(), &*empty.end());

}

TEST(ICharBwtTest, EmptyICharBwt) {
  std::vector<char> empty;
  bwt::iCharBwt(empty, 0);
}

TEST(CharBwtTest, BananaCharBwt) {
  std::string banana_str = "banana";
  std::vector<char> banana(banana_str.begin(), banana_str.end());
  std::vector<char> ret;
  size_t key;
  std::tie(ret, key) = bwt::charBwt(&*banana.begin(), &*banana.end());

  std::string bwt_str = "bnnaaa";
  std::vector<char> bwt(bwt_str.begin(), bwt_str.end());
  ASSERT_EQ(bwt, ret);
  ASSERT_EQ(key, 3);

  std::vector<char> decoded_str = bwt::iCharBwt(ret, key);

  ASSERT_EQ(banana, decoded_str);

}

TEST(Bwt, Empty) {
  std::vector<int> empty;
  bwt::bwt(empty.begin(), empty.end());
}

TEST(Bwt, Small) {
  std::vector<int> small = {5, 1, 2, 1, 2};
  std::vector<int> ret;
  size_t key;
  std::tie(ret, key) = bwt::bwt(small.begin(), small.end());
  std::cerr << "return:\n";
  std::cerr << ret[0] << "\n";
  ASSERT_EQ(key, 4);
  std::vector<int> expected = {5, 2, 1, 1, 2};
  ASSERT_EQ(ret, expected);

}

//TODO: add inverse bwt test
TEST(IBwt, Small) {
  std::vector<int> small = {5, 2, 1, 1, 2};
  size_t key = 4;
  bool success;
  std::vector<int> ret;
  std::tie(ret, success) = bwt::iBwt(small.begin(), small.end(), key);

  ASSERT_TRUE(success);

  std::vector<int> expected = {5, 1, 2, 1, 2};
  ASSERT_EQ(ret, expected);
}
