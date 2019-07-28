#include "gtest/gtest.h"
#include "cj/utils/top_n_set.hh"

TEST(CJTopNSet, CreatesTopNSet) {
  auto const s = cj::top_n_set<double>(42);
  EXPECT_TRUE(s.empty());
  EXPECT_FALSE(s.is_full());
}

TEST(CJTopNSet, TopNSetIsFull) {
  auto s = cj::top_n_set<char>(4);
  EXPECT_TRUE(s.empty());
  EXPECT_FALSE(s.is_full());

  s.try_insert('c');
  EXPECT_FALSE(s.is_full());
  EXPECT_EQ(1, s.size());

  s.try_insert('c');
  EXPECT_FALSE(s.is_full());
  EXPECT_EQ(1, s.size());

  s.try_insert('a');
  EXPECT_FALSE(s.is_full());
  EXPECT_EQ(2, s.size());

  s.try_insert('b');
  EXPECT_FALSE(s.is_full());
  EXPECT_EQ(3, s.size());

  s.try_insert('e');
  EXPECT_TRUE(s.is_full());
  EXPECT_EQ(4, s.size());

  s.try_insert('f');
  EXPECT_TRUE(s.is_full());
  EXPECT_EQ(4, s.size());
}

TEST(CJTopNSet, TopNSetGetMinimum) {
  auto s = cj::top_n_set<uint32_t, cj::flat_set>(2);

  s.try_insert(28);
  EXPECT_EQ(28, s.minimum());
  EXPECT_EQ(1, s.size());

  s.try_insert(32);
  EXPECT_EQ(28, s.minimum());
  EXPECT_EQ(2, s.size());

  s.try_insert(20);
  EXPECT_EQ(28, s.minimum());
  EXPECT_EQ(2, s.size());

  s.try_insert(40);
  EXPECT_EQ(32, s.minimum());
  EXPECT_EQ(2, s.size());
}

TEST(CJTopNSet, TopNMultiMaximum) {
  auto s = cj::top_n_multiset<std::string>(3);

  s.try_insert("Kyoto");
  EXPECT_EQ("Kyoto", s.maximum());
  EXPECT_EQ(1, s.size());

  s.try_insert("Kyoto");
  EXPECT_EQ("Kyoto", s.maximum());
  EXPECT_EQ(2, s.size());

  s.try_insert("Vancouver");
  EXPECT_EQ("Vancouver", s.maximum());
  EXPECT_EQ(3, s.size());

  s.try_insert("Paris");
  EXPECT_EQ("Kyoto", s.minimum());
  EXPECT_EQ("Vancouver", s.maximum());
  EXPECT_EQ(3, s.size());

  s.try_insert("Paris");
  EXPECT_EQ("Paris", s.minimum());
  EXPECT_EQ("Vancouver", s.maximum());
  EXPECT_EQ(3, s.size());
}

TEST(CJTopNSet, TopNMaximum) {
  auto s = cj::top_n_set<int>(4, {4, 2, 1, 8, 8, 8});

  EXPECT_EQ(1, s.minimum());
  EXPECT_EQ(8, s.maximum());
  EXPECT_EQ(4, s.size());
}

TEST(CJTopNSet, PrintTopNSet) {
  auto const s = cj::top_n_set<char, cj::flat_set>(3, {'a', 'c', 'e', 'd', 'z', 'x', 'a', 'x', 'z'});
  EXPECT_EQ(cj::string{"{e, x, z}"}, boost::lexical_cast<cj::string>(s));
}

TEST(CJTopNSet, PrintTopNMultiSet) {
  auto const s = cj::top_n_multiset<char>(4, {'a', 'c', 'e', 'd', 'z', 'x', 'a', 'b', 'z'});
  EXPECT_EQ(cj::string{"{e, x, z, z}"}, boost::lexical_cast<cj::string>(s));
}
