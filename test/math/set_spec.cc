#include "gtest/gtest.h"
#include "cj/math/set.hh"
#include "cj/math/random.hh"
#include "cj/utils/string.hh"

TEST(CJSet, OrderedSetUnionSizeTwoEmptySets) {
  EXPECT_EQ(0, cj::set_union_size(cj::ordered_set<int>{}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetUnionSizeLeftEmptySets) {
  EXPECT_EQ(3, cj::set_union_size(cj::flat_set<size_t>{}, cj::flat_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, OrderedSetUnionSizeRightEmptySets) {
  EXPECT_EQ(4, cj::set_union_size(cj::ordered_set<int>{0, -5, 8, -5, 2}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetUnionSize) {
  EXPECT_EQ(6, cj::set_union_size(cj::flat_set<char>{'a', 'z', 'd', 'e'}, cj::flat_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, UnorderedSetUnionSizeTwoEmptySets) {
  EXPECT_EQ(0, cj::set_union_size(cj::unordered_set<int>{}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetUnionSizeLeftEmptySets) {
  EXPECT_EQ(3, cj::set_union_size(cj::unordered_set<size_t>{}, cj::unordered_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, UnorderedSetUnionSizeRightEmptySets) {
  EXPECT_EQ(4, cj::set_union_size(cj::unordered_set<int>{0, -5, 8, -5, 2}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetUnionSize) {
  EXPECT_EQ(6, cj::set_union_size(cj::unordered_set<char>{'a', 'z', 'd', 'e'}, cj::unordered_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, OrderedSetUnionTwoEmptySets) {
  EXPECT_EQ((cj::ordered_set<int>{}), cj::set_union(cj::ordered_set<int>{}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetUnionLeftEmptySets) {
  EXPECT_EQ((cj::flat_set<size_t>{3, 2, 1}), cj::set_union(cj::flat_set<size_t>{}, cj::flat_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, OrderedSetUnionRightEmptySets) {
  EXPECT_EQ((cj::ordered_set<int>{-5, 0, 2, 8}), cj::set_union(cj::ordered_set<int>{0, -5, 8, -5, 2}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetUnion) {
  EXPECT_EQ((cj::flat_set<char>{'a', 'd', 'z', 'e', 'h', 'i'}), cj::set_union(cj::flat_set<char>{'a', 'z', 'd', 'e'}, cj::flat_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, UnorderedSetUnionTwoEmptySets) {
  EXPECT_EQ((cj::unordered_set<int>{}), cj::set_union(cj::unordered_set<int>{}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetUnionLeftEmptySets) {
  EXPECT_EQ((cj::unordered_set<size_t>{3, 1, 2}), cj::set_union(cj::unordered_set<size_t>{}, cj::unordered_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, UnorderedSetUnionRightEmptySets) {
  EXPECT_EQ((cj::unordered_set<int>{8, 2, -5, 0}), cj::set_union(cj::unordered_set<int>{0, -5, 8, -5, 2}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetUnion) {
  EXPECT_EQ((cj::unordered_set<char>{'z', 'a', 'd', 'i', 'h', 'e'}), cj::set_union(cj::unordered_set<char>{'a', 'z', 'd', 'e'}, cj::unordered_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, OrderedSetIntersectionSizeTwoEmptySets) {
  EXPECT_EQ(0, cj::set_intersection_size(cj::ordered_set<int>{}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetIntersectionSizeLeftEmptySets) {
  EXPECT_EQ(0, cj::set_intersection_size(cj::flat_set<size_t>{}, cj::flat_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, OrderedSetIntersectionSizeRightEmptySets) {
  EXPECT_EQ(0, cj::set_intersection_size(cj::ordered_set<int>{0, -5, 8, -5, 2}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetIntersectionSize) {
  EXPECT_EQ(2, cj::set_intersection_size(cj::flat_set<char>{'a', 'z', 'd', 'e'}, cj::flat_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, UnorderedSetIntersectionSizeTwoEmptySets) {
  EXPECT_EQ(0, cj::set_intersection_size(cj::unordered_set<int>{}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetIntersectionSizeLeftEmptySets) {
  EXPECT_EQ(0, cj::set_intersection_size(cj::unordered_set<size_t>{}, cj::unordered_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, UnorderedSetIntersectionSizeRightEmptySets) {
  EXPECT_EQ(0, cj::set_intersection_size(cj::unordered_set<int>{0, -5, 8, -5, 2}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetIntersectionSize) {
  EXPECT_EQ(2, cj::set_intersection_size(cj::unordered_set<char>{'a', 'z', 'd', 'e'}, cj::unordered_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, OrderedSetIntersectionTwoEmptySets) {
  EXPECT_EQ((cj::ordered_set<int>{}), cj::set_intersection(cj::ordered_set<int>{}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetIntersectionLeftEmptySets) {
  EXPECT_EQ((cj::flat_set<size_t>{}), cj::set_intersection(cj::flat_set<size_t>{}, cj::flat_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, OrderedSetIntersectionRightEmptySets) {
  EXPECT_EQ((cj::ordered_set<int>{}), cj::set_intersection(cj::ordered_set<int>{0, -5, 8, -5, 2}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetIntersection) {
  EXPECT_EQ((cj::flat_set<char>{'z', 'a'}), cj::set_intersection(cj::flat_set<char>{'a', 'z', 'd', 'e'}, cj::flat_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, UnorderedSetIntersectionTwoEmptySets) {
  EXPECT_EQ((cj::unordered_set<int>{}), cj::set_intersection(cj::unordered_set<int>{}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetIntersectionLeftEmptySets) {
  EXPECT_EQ((cj::unordered_set<size_t>{}), cj::set_intersection(cj::unordered_set<size_t>{}, cj::unordered_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, UnorderedSetIntersectionRightEmptySets) {
  EXPECT_EQ((cj::unordered_set<int>{}), cj::set_intersection(cj::unordered_set<int>{0, -5, 8, -5, 2}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetIntersection) {
  EXPECT_EQ((cj::unordered_set<char>{'z', 'a'}), cj::set_intersection(cj::unordered_set<char>{'a', 'z', 'd', 'e'}, cj::unordered_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, OrderedSetHasEmptyIntersectionTwoEmptySets) {
  EXPECT_TRUE(cj::empty_set_intersection(cj::ordered_set<int>{}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetHasEmptyIntersectionLeftEmptySets) {
  EXPECT_TRUE(cj::empty_set_intersection(cj::flat_set<size_t>{}, cj::flat_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, OrderedSetHasEmptyIntersectionRightEmptySets) {
  EXPECT_TRUE(cj::empty_set_intersection(cj::ordered_set<int>{0, -5, 8, -5, 2}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetHasEmptyIntersection) {
  EXPECT_FALSE(cj::empty_set_intersection(cj::flat_set<char>{'a', 'z', 'd', 'e'}, cj::flat_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, UnorderedSetHasEmptyIntersectionTwoEmptySets) {
  EXPECT_TRUE(cj::empty_set_intersection(cj::unordered_set<int>{}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetHasEmptyIntersectionLeftEmptySets) {
  EXPECT_TRUE(cj::empty_set_intersection(cj::unordered_set<size_t>{}, cj::unordered_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, UnorderedSetHasEmptyIntersectionRightEmptySets) {
  EXPECT_TRUE(cj::empty_set_intersection(cj::unordered_set<int>{0, -5, 8, -5, 2}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetHasEmptyIntersection) {
  EXPECT_FALSE(cj::empty_set_intersection(cj::unordered_set<char>{'a', 'z', 'd', 'e'}, cj::unordered_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, OrderedSetDifferenceSizeTwoEmptySets) {
  EXPECT_EQ(0, cj::set_difference_size(cj::ordered_set<int>{}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetDifferenceSizeLeftEmptySets) {
  EXPECT_EQ(0, cj::set_difference_size(cj::flat_set<size_t>{}, cj::flat_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, OrderedSetDifferenceSizeRightEmptySets) {
  EXPECT_EQ(4, cj::set_difference_size(cj::ordered_set<int>{0, -5, 8, -5, 2}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetDifferenceSize) {
  EXPECT_EQ(2, cj::set_difference_size(cj::flat_set<char>{'a', 'z', 'd', 'e'}, cj::flat_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, UnorderedSetDifferenceSizeTwoEmptySets) {
  EXPECT_EQ(0, cj::set_difference_size(cj::unordered_set<int>{}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetDifferenceSizeLeftEmptySets) {
  EXPECT_EQ(0, cj::set_difference_size(cj::unordered_set<size_t>{}, cj::unordered_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, UnorderedSetDifferenceSizeRightEmptySets) {
  EXPECT_EQ(4, cj::set_difference_size(cj::unordered_set<int>{0, -5, 8, -5, 2}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetDifferenceSize) {
  EXPECT_EQ(2, cj::set_difference_size(cj::unordered_set<char>{'a', 'z', 'd', 'e'}, cj::unordered_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, OrderedSetDifferenceTwoEmptySets) {
  EXPECT_EQ((cj::ordered_set<int>{}), cj::set_difference(cj::ordered_set<int>{}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetDifferenceLeftEmptySets) {
  EXPECT_EQ((cj::flat_set<size_t>{}), cj::set_difference(cj::flat_set<size_t>{}, cj::flat_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, OrderedSetDifferenceRightEmptySets) {
  EXPECT_EQ((cj::ordered_set<int>{0, 2, -5, 8}), cj::set_difference(cj::ordered_set<int>{0, -5, 8, -5, 2}, cj::ordered_set<int>{}));
}

TEST(CJSet, OrderedSetDifference) {
  EXPECT_EQ((cj::flat_set<char>{'d', 'e'}), cj::set_difference(cj::flat_set<char>{'a', 'z', 'd', 'e'}, cj::flat_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, UnorderedSetDifferenceTwoEmptySets) {
  EXPECT_EQ((cj::unordered_set<int>{}), cj::set_difference(cj::unordered_set<int>{}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetDifferenceLeftEmptySets) {
  EXPECT_EQ((cj::unordered_set<size_t>{}), cj::set_difference(cj::unordered_set<size_t>{}, cj::unordered_set<size_t>{1, 2, 3, 3}));
}

TEST(CJSet, UnorderedSetDifferenceRightEmptySets) {
  EXPECT_EQ((cj::unordered_set<int>{0, 2, -5, 8}), cj::set_difference(cj::unordered_set<int>{0, -5, 8, -5, 2}, cj::unordered_set<int>{}));
}

TEST(CJSet, UnorderedSetDifference) {
  EXPECT_EQ((cj::unordered_set<char>{'d', 'e'}), cj::set_difference(cj::unordered_set<char>{'a', 'z', 'd', 'e'}, cj::unordered_set<char>{'h', 'i', 'z', 'a'}));
}

TEST(CJSet, SetIntersectionSplitUnion) {
  auto dist = std::uniform_int_distribution<size_t>(0, 6);
  auto rng = std::mt19937_64(42);

  for (auto i = 0; i < 100; ++i) {
    auto const x = cj::unique_integers(dist(rng), 0, 10, rng);
    auto const y = cj::unique_integers(dist(rng), 0, 10, rng);
    auto const z = cj::set_intersection_split_union(x, y, rng);
//    std::cout << "x = {" << cj::intersperse(x.begin(), x.end()) << "}\n"
//              << "y = {" << cj::intersperse(y.begin(), y.end()) << "}\n"
//              << "z = {" << cj::intersperse(z.begin(), z.end()) << "}\n\n";
    EXPECT_TRUE(z.size() >= cj::set_intersection_size(x, y));
    EXPECT_TRUE(z.size() <= cj::set_union_size(x, y));
  }
}

TEST(CJSet, MapIntersectionSplitUnion) {
  auto dist = std::uniform_int_distribution<size_t>(0, 6);
  auto unif = std::uniform_real_distribution<double>(0, 1);
  auto rng = std::mt19937_64(42);

  for (auto i = 0; i < 100; ++i) {
    auto const x_ = cj::unique_integers(dist(rng), 0, 10, rng);
    auto const y_ = cj::unique_integers(dist(rng), 0, 10, rng);

    auto x = cj::flat_map<size_t, double>{};
    for (auto const& i : x_) {
      x[i] = unif(rng);
    }

    auto y = cj::flat_map<size_t, double>{};
    for (auto const& i : y_) {
      y[i] = unif(rng);
    }

    auto const z = cj::map_intersection_split_union(x, y, rng);
//    std::cout << "x = {" << cj::intersperse_pairs(x.begin(), x.end()) << "}\n"
//              << "y = {" << cj::intersperse_pairs(y.begin(), y.end()) << "}\n"
//              << "z = {" << cj::intersperse_pairs(z.begin(), z.end()) << "}\n\n";

    for (auto const& i : x) {
      if (y.find(i.first) != y.end()) {
        EXPECT_TRUE((z.find(i.first) != z.end()));
      }
    }
    for (auto const& i : y) {
      if (x.find(i.first) != x.end()) {
        EXPECT_TRUE((z.find(i.first) != z.end()));
      }
    }
  }
}
