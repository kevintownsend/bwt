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
