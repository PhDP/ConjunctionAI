#include "gtest/gtest.h"
#include "cj/math/confusion.hh"

TEST(CJConfusion, EmptyConfusion) {
  auto c = cj::confusion<size_t>(2);
  EXPECT_TRUE(c.empty());
  EXPECT_EQ(0, c.count());
  EXPECT_EQ(2, c.dim());
  EXPECT_EQ(0, c(0, 0));
  EXPECT_EQ(0, c(0, 1));
  EXPECT_EQ(0, c(1, 0));
  EXPECT_EQ(0, c(1, 1));
}

// https://www.dataschool.io/simple-guide-to-confusion-matrix-terminology/
TEST(CJConfusion, Adding2Dim) {
  auto c = cj::confusion<size_t>(2);
  EXPECT_TRUE(c.empty());
  c.add_count(0, 0, 50);
  c.add_count(0, 1);
  c.add_count(0, 1);
  c.add_count(0, 1);
  c.add_count(0, 1);
  c.add_count(0, 1);
  c.add_count(1, 0, 100);
  c.sub_count(1, 0, 100000);
  c.sub_count(1, 0, 100000);
  c.add_count(1, 0, 10);
  c.add_count(1, 1, 100);
  EXPECT_EQ(165, c.count());
  EXPECT_DOUBLE_EQ(0.90909090909090906, c.accuracy());
  EXPECT_DOUBLE_EQ(0.90909090909090906, c.accuracy(1));
  EXPECT_DOUBLE_EQ(0.90909090909090906, c.accuracy(0));
  EXPECT_EQ(100, c.true_positives(1));
  EXPECT_EQ(50, c.true_negatives(1));
  EXPECT_EQ(10, c.false_positives(1));
  EXPECT_EQ(5, c.false_negatives(1));
}

//TEST(CJConfusion, RemovingCounts) {
//}
