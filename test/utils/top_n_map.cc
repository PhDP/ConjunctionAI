#include "gtest/gtest.h"
#include "cj/utils/top_n_map.hh"

TEST(CJTopNMap, CreatesBoundedMultimap) {
  auto const m = cj::top_n_multimap<double, size_t>(42);
  EXPECT_TRUE(m.empty());
  EXPECT_FALSE(m.is_full());
}

TEST(CJTopNMap, BoundedMultimapIsFull) {
  auto m = cj::top_n_multimap<double, char>(4);
  EXPECT_TRUE(m.empty());
  EXPECT_FALSE(m.is_full());

  m.try_insert(0.5, 'c');
  EXPECT_FALSE(m.is_full());
  EXPECT_EQ(1, m.size());

  m.try_insert(0.5, 'c');
  EXPECT_FALSE(m.is_full());
  EXPECT_EQ(2, m.size());

  m.try_insert(0.5, 'a');
  EXPECT_FALSE(m.is_full());
  EXPECT_EQ(3, m.size());

  m.try_insert(0.5, 'b');
  EXPECT_TRUE(m.is_full());
  EXPECT_EQ(4, m.size());

  m.try_insert(0.5, 'e');
  EXPECT_TRUE(m.is_full());
  EXPECT_EQ(4, m.size());

  m.try_insert(0.6, 'f');
  EXPECT_TRUE(m.is_full());
  EXPECT_EQ(4, m.size());
}

TEST(CJTopNMap, BoundedMapGetMinimum) {
  auto m = cj::top_n_multimap<double, char>(2);
  using pair = cj::top_n_multimap<double, char>::value_type;

  m.try_insert(0.5, 'c');
  EXPECT_EQ(pair(0.5, 'c'), m.minimum());
  EXPECT_EQ(1, m.size());

  m.try_insert(0.5, 'c');
  EXPECT_EQ(pair(0.5, 'c'), m.minimum());
  EXPECT_EQ(2, m.size());

  m.try_insert(0.6, 'd');
  EXPECT_EQ(pair(0.5, 'c'), m.minimum());
  EXPECT_EQ(2, m.size());

  m.try_insert(0.7, 'e');
  EXPECT_EQ(pair(0.6, 'd'), m.minimum());
  EXPECT_EQ(2, m.size());

  m.try_insert(0.8, 'e');
  EXPECT_EQ(pair(0.7, 'e'), m.minimum());
  EXPECT_EQ(2, m.size());
}

TEST(CJTopNMap, BoundedMapGetMaximum) {
  auto m = cj::top_n_multimap<int, std::string>(3);
  using pair = cj::top_n_multimap<int, std::string>::value_type;

  m.try_insert(10, "Kyoto");
  EXPECT_EQ(pair(10, "Kyoto"), m.maximum());
  EXPECT_EQ(1, m.size());

  m.try_insert(12, "Vancouver");
  EXPECT_EQ(pair(12, "Vancouver"), m.maximum());
  EXPECT_EQ(2, m.size());

  m.try_insert(5, "Paris");
  EXPECT_EQ(pair(12, "Vancouver"), m.maximum());
  EXPECT_EQ(3, m.size());

  m.try_insert(13, "Sapporo");
  EXPECT_EQ(pair(13, "Sapporo"), m.maximum());
  EXPECT_EQ(3, m.size());
}

TEST(CJTopNMap, BoundedMultimapSetOfValues) {
  auto m = cj::top_n_multimap<double, uint64_t>(4);
  using pair = cj::top_n_multimap<double, uint64_t>::value_type;

  m.try_insert(0.1, 19);
  m.try_insert(0.9, 28);
  m.try_insert(0.5,  0);
  m.try_insert(0.4, 13);
  m.try_insert(0.4, 12);
  m.try_insert(0.9,  6);
  m.try_insert(0.8, 28);
  m.try_insert(0.7, 15);

  EXPECT_EQ((cj::ordered_set<uint64_t>{28, 6, 15}), m.set_of_values());
  EXPECT_EQ((cj::ordered_multiset<uint64_t>{28, 28, 6, 15}), m.multiset_of_values());
}
