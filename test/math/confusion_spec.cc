#include "gtest/gtest.h"
#include "cj/math/confusion.hh"

TEST(CJConfusion, IsEmpty) {
  auto const c = cj::confusion<>{};
  EXPECT_TRUE(c.empty());
}

TEST(CJConfusion, TestBasicMethods) {
  auto const c0 = cj::confusion<>{50, 0, 8, 3};
  auto c1 = cj::confusion<>{20, 30, 2, 0};
  c1 += cj::confusion<>{30, 20, 0, 2};
  auto const c = c0 + c1;
  EXPECT_TRUE(!c.empty());
  EXPECT_EQ(165, c.size());
  EXPECT_DOUBLE_EQ(0.90909090909090906, c.accuracy());
  EXPECT_DOUBLE_EQ(0.95238095238095233, c.tpr());
  EXPECT_DOUBLE_EQ(0.95238095238095233, c.recall());
  EXPECT_DOUBLE_EQ(0.95238095238095233, c.sensitivity());
  EXPECT_DOUBLE_EQ(0.83333333333333337, c.specificity());
  EXPECT_DOUBLE_EQ(0.90909090909090906, c.precision());
}
