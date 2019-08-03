#include "gtest/gtest.h"
#include "cj/utils/value_ptr.hh"

TEST(CJValuePtr, Equality) {
  auto const x = cj::value_ptr<int>{new int{42}};
  auto const y = cj::value_ptr<int>{new int{42}};
  EXPECT_EQ(x, y);
}

TEST(CJValuePtr, Inequality) {
  auto const x = cj::value_ptr<cj::string>{new cj::string{"Alex"}};
  auto const y = cj::value_ptr<cj::string>{new cj::string{"Alexandra"}};
  EXPECT_NE(x, y);
}

TEST(CJValuePtr, LessThan) {
  auto const x = cj::value_ptr<double>{new double{0.5}};
  auto const y = cj::value_ptr<double>{new double{8128}};
  EXPECT_TRUE((x < y));
  EXPECT_FALSE((y < x));
}

TEST(CJValuePtr, ValuePtrInHashMap) {
  auto xs = cj::unordered_set<cj::value_ptr<int>>{};
  xs.insert(cj::value_ptr<int>{nullptr});
  xs.insert(cj::value_ptr<int>{new int{6}});
  xs.insert(cj::value_ptr<int>{new int{0}});
  xs.insert(cj::value_ptr<int>{new int{6}});
  xs.insert(cj::value_ptr<int>{nullptr});

  EXPECT_EQ(3, xs.size());
  EXPECT_TRUE(cj::contains(xs, cj::value_ptr<int>{new int{6}}));
  EXPECT_TRUE(cj::contains(xs, cj::value_ptr<int>{nullptr}));
  EXPECT_FALSE(cj::contains(xs, cj::value_ptr<int>{new int{7}}));
}
