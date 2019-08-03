#include "gtest/gtest.h"
#include "cj/utils/value_ptr.hh"

TEST(CJValuePtr, Equality) {
  auto const x = cj::value_ptr<int>{new int{42}};
  auto const y = cj::value_ptr<int>{new int{42}};
  EXPECT_TRUE((x == y));
}

TEST(CJValuePtr, EqualsNotEquals) {
  auto const x = cj::value_ptr<int>{new int{42}};
  auto const y = cj::value_ptr<int>{new int{0}};
  EXPECT_FALSE((x == y));
}

TEST(CJValuePtr, EqualityWithLHSNull) {
  auto const x = cj::value_ptr<float>{nullptr};
  auto const y = cj::value_ptr<float>{new float{2}};
  EXPECT_FALSE((x == y));
}

TEST(CJValuePtr, EqualityWithRHSNull) {
  auto const x = cj::value_ptr<char>{new char{'c'}};
  auto const y = cj::value_ptr<char>{nullptr};
  EXPECT_FALSE((x == y));
}

TEST(CJValuePtr, NullEqualsNull) {
  auto const x = cj::value_ptr<cj::string>{nullptr};
  auto const y = cj::value_ptr<cj::string>{nullptr};
  EXPECT_TRUE((x == y));
}

TEST(CJValuePtr, Inequality) {
  auto const x = cj::value_ptr<cj::string>{new cj::string{"Alex"}};
  auto const y = cj::value_ptr<cj::string>{new cj::string{"Alexandra"}};
  EXPECT_TRUE((x != y));
}

TEST(CJValuePtr, InequalityWithLHSNull) {
  auto const x = cj::value_ptr<int>{nullptr};
  auto const y = cj::value_ptr<int>{new int{2}};
  EXPECT_TRUE((x != y));
}

TEST(CJValuePtr, InequalityWithRHSNull) {
  auto const x = cj::value_ptr<uint64_t>{new uint64_t{8128}};
  auto const y = cj::value_ptr<uint64_t>{nullptr};
  EXPECT_TRUE((x != y));
}

TEST(CJValuePtr, InequalityWithNulls) {
  auto const x = cj::value_ptr<char>{nullptr};
  auto const y = cj::value_ptr<char>{nullptr};
  EXPECT_FALSE((x != y));
}

TEST(CJValuePtr, LessThan) {
  auto const x = cj::value_ptr<double>{new double{0.5}};
  auto const y = cj::value_ptr<double>{new double{8128}};
  EXPECT_TRUE((x < y));
  EXPECT_FALSE((y < x));
}

TEST(CJValuePtr, LessThanWithLHSNull) {
  auto const x = cj::value_ptr<int>{nullptr};
  auto const y = cj::value_ptr<int>{new int{-8128}};
  EXPECT_TRUE((x < y));
  EXPECT_FALSE((y < x));
}

TEST(CJValuePtr, LessThanWithRHSNull) {
  auto const x = cj::value_ptr<char>{new char{'z'}};
  auto const y = cj::value_ptr<char>{nullptr};
  EXPECT_FALSE((x < y));
  EXPECT_TRUE((y < x));
}

TEST(CJValuePtr, LessThanWithNulls) {
  auto const x = cj::value_ptr<cj::string>{nullptr};
  auto const y = cj::value_ptr<cj::string>{nullptr};
  EXPECT_FALSE((x < y));
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

TEST(CJValuePtr, ValuePtrInOrderedMap) {
  auto xs = cj::unordered_set<cj::value_ptr<char>>{};
  xs.insert(cj::value_ptr<char>{new char{'d'}});
  xs.insert(cj::value_ptr<char>{new char{'z'}});
  xs.insert(cj::value_ptr<char>{nullptr});
  xs.insert(cj::value_ptr<char>{new char{'d'}});
  xs.insert(cj::value_ptr<char>{nullptr});

  EXPECT_EQ(3, xs.size());
  EXPECT_TRUE(cj::contains(xs, cj::value_ptr<char>{new char{'z'}}));
  EXPECT_TRUE(cj::contains(xs, cj::value_ptr<char>{nullptr}));
  EXPECT_FALSE(cj::contains(xs, cj::value_ptr<char>{new char{'e'}}));
}
