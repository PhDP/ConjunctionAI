#include "gtest/gtest.h"
#include "cj/logics/clausal_kb.hh"

TEST(CJClausalKB, CreatesEmptyClause) {
  auto const c = cj::clausal_kb<char>();
  EXPECT_TRUE(c.empty());
  EXPECT_EQ(0, c.size());
  EXPECT_EQ(0, c.size_hard());
  EXPECT_EQ(0, c.size_prob());
}
