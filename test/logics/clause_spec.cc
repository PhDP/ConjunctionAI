#include "gtest/gtest.h"
#include "cj/logics/clause.hh"

TEST(CJClause, CreatesEmptyClause) {
  auto const c = cj::clause<char>();
  EXPECT_TRUE(c.empty());
  EXPECT_EQ(0, c.size());
  EXPECT_EQ(0, c.size_head());
  EXPECT_EQ(0, c.size_body());
}

TEST(CJClause, CreatesClause) {
  auto const c = cj::clause<char>({'a'}, {'b', 'c', 'b', 'd'});
  EXPECT_FALSE(c.empty());
  EXPECT_EQ(4, c.size());
  EXPECT_EQ(1, c.size_head());
  EXPECT_EQ(3, c.size_body());
}

TEST(CJClause, CreatesClauseWithMultiset) {
  auto const c = cj::clause<char, cj::flat_multiset>({'a'}, {'b', 'c', 'b', 'd'});
  EXPECT_FALSE(c.empty());
  EXPECT_EQ(5, c.size());
  EXPECT_EQ(1, c.size_head());
  EXPECT_EQ(4, c.size_body());
}

TEST(CJClause, PrintClauses) {
  auto const c = cj::clause<char>({'z', 'z'}, {'y', 'x', 'x', 'z'});
  EXPECT_EQ(cj::string{"z <- x, y, z"}, boost::lexical_cast<cj::string>(c));
}

TEST(CJClause, PrintMultiClauses) {
  auto const c = cj::clause<char, cj::ordered_multiset>({'z', 'z'}, {'y', 'x', 'x', 'z'});
  EXPECT_EQ(cj::string{"z, z <- x, x, y, z"}, boost::lexical_cast<cj::string>(c));
}

TEST(CJClause, CountLiteralInEmptyClause) {
  auto const c = cj::clause<int>();
  EXPECT_TRUE(c.empty());
  EXPECT_FALSE(c.count(42));
  EXPECT_FALSE(c.count(6));
  EXPECT_FALSE(c.count(0));
}

TEST(CJClause, CountLiteralClause) {
  auto const c = cj::clause<cj::string>({"Sherbrooke", "Vancouver", "Busan", "Seoul"}, {"Busan", "Toronto", "Busan", "Montreal", "Montreal"});
  EXPECT_FALSE(c.empty());
  EXPECT_EQ(1, c.count("Sherbrooke"));
  EXPECT_EQ(1, c.count("Montreal"));
  EXPECT_EQ(2, c.count("Busan"));
  EXPECT_EQ(1, c.count("Toronto"));
  EXPECT_EQ(0, c.count("Rimouski"));
  EXPECT_EQ(0, c.count("Tunis"));
  EXPECT_EQ(0, c.count("montreal"));
}

TEST(CJClause, CountLiteralMultiClause) {
  auto const c = cj::clause<cj::string, cj::flat_multiset>({"Sherbrooke", "Vancouver", "Busan", "Seoul"}, {"Busan", "Toronto", "Busan", "Montreal", "Montreal"});
  EXPECT_EQ(1, c.count("Sherbrooke"));
  EXPECT_EQ(2, c.count("Montreal"));
  EXPECT_EQ(3, c.count("Busan"));
  EXPECT_EQ(1, c.count("Toronto"));
  EXPECT_EQ(0, c.count("Rimouski"));
  EXPECT_EQ(0, c.count("Tunis"));
  EXPECT_EQ(0, c.count("montreal"));
}

TEST(CJClause, CountLiteralClauseHead) {
  auto const c = cj::clause<cj::string, cj::ordered_set>({"Vancouver", "Sherbrooke", "Vancouver", "Busan", "Seoul"}, {"Busan", "Toronto", "Busan", "Montreal", "Montreal"});
  EXPECT_FALSE(c.empty());
  EXPECT_EQ(1, c.count_head("Sherbrooke"));
  EXPECT_EQ(0, c.count_head("Montreal"));
  EXPECT_EQ(1, c.count_head("Busan"));
  EXPECT_EQ(0, c.count_head("Toronto"));
  EXPECT_EQ(0, c.count_head("Rimouski"));
  EXPECT_EQ(0, c.count_head("Tunis"));
  EXPECT_EQ(0, c.count_head("montreal"));
}

TEST(CJClause, CountLiteralMultiClauseHead) {
  auto const c = cj::clause<cj::string, cj::ordered_multiset>({"Vancouver", "Sherbrooke", "Vancouver", "Busan", "Seoul"}, {"Busan", "Toronto", "Busan", "Montreal", "Montreal"});
  EXPECT_EQ(1, c.count_head("Sherbrooke"));
  EXPECT_EQ(0, c.count_head("Montreal"));
  EXPECT_EQ(1, c.count_head("Busan"));
  EXPECT_EQ(2, c.count_head("Vancouver"));
  EXPECT_EQ(0, c.count_head("Rimouski"));
  EXPECT_EQ(0, c.count_head("Tunis"));
  EXPECT_EQ(0, c.count_head("montreal"));
}

TEST(CJClause, CountLiteralClauseBody) {
  auto const c = cj::clause<cj::string, cj::ordered_set>({"Vancouver", "Sherbrooke", "Vancouver", "Busan", "Seoul"}, {"Busan", "Toronto", "Busan", "Montreal", "Montreal"});
  EXPECT_FALSE(c.empty());
  EXPECT_EQ(0, c.count_body("Sherbrooke"));
  EXPECT_EQ(1, c.count_body("Montreal"));
  EXPECT_EQ(1, c.count_body("Busan"));
  EXPECT_EQ(1, c.count_body("Toronto"));
  EXPECT_EQ(0, c.count_body("Rimouski"));
  EXPECT_EQ(0, c.count_body("Tunis"));
  EXPECT_EQ(0, c.count_body("montreal"));
}

TEST(CJClause, CountLiteralMultiClauseBody) {
  auto const c = cj::clause<cj::string, cj::ordered_multiset>({"Vancouver", "Sherbrooke", "Vancouver", "Busan", "Seoul"}, {"Busan", "Toronto", "Busan", "Montreal", "Montreal"});
  EXPECT_EQ(0, c.count_body("Sherbrooke"));
  EXPECT_EQ(2, c.count_body("Montreal"));
  EXPECT_EQ(2, c.count_body("Busan"));
  EXPECT_EQ(0, c.count_body("Vancouver"));
  EXPECT_EQ(0, c.count_body("Rimouski"));
  EXPECT_EQ(0, c.count_body("Tunis"));
  EXPECT_EQ(0, c.count_body("montreal"));
}

TEST(CJClause, HasLiteralInClause) {
  auto const c = cj::clause<int, cj::ordered_set>({11, -28, 11, 3}, {-28, -28, 1, 5, -28, -28, 70});
  EXPECT_FALSE(c.empty());
  EXPECT_TRUE(c.has(11));
  EXPECT_TRUE(c.has(5));
  EXPECT_TRUE(c.has(-28));
  EXPECT_TRUE(c.has(3));
  EXPECT_TRUE(c.has(70));
  EXPECT_FALSE(c.has(2));
  EXPECT_FALSE(c.has(0));
  EXPECT_FALSE(c.has(80));
}

TEST(CJClause, HasLiteralInMultiClause) {
  auto const c = cj::clause<int, cj::ordered_multiset>({11, -28, 11, 3}, {1, 5, -28, -28, 70});
  EXPECT_TRUE(c.has(11));
  EXPECT_TRUE(c.has(5));
  EXPECT_TRUE(c.has(-28));
  EXPECT_TRUE(c.has(3));
  EXPECT_TRUE(c.has(70));
  EXPECT_FALSE(c.has(2));
  EXPECT_FALSE(c.has(0));
  EXPECT_FALSE(c.has(80));
}

TEST(CJClause, HasLiteralInClauseHead) {
  auto const c = cj::clause<int, cj::ordered_set>({11, -28, 11, 3}, {-28, -28, 1, 5, -28, -28, 70});
  EXPECT_FALSE(c.empty());
  EXPECT_TRUE(c.has_head(11));
  EXPECT_FALSE(c.has_head(5));
  EXPECT_TRUE(c.has_head(-28));
  EXPECT_TRUE(c.has_head(3));
  EXPECT_FALSE(c.has_head(70));
  EXPECT_FALSE(c.has_head(2));
  EXPECT_FALSE(c.has_head(0));
  EXPECT_FALSE(c.has_head(80));
}

TEST(CJClause, HasLiteralInMultiClauseHead) {
  auto const c = cj::clause<int, cj::ordered_multiset>({11, -28, 11, 3}, {1, 5, -28, -28, 70});
  EXPECT_FALSE(c.empty());
  EXPECT_TRUE(c.has_head(11));
  EXPECT_FALSE(c.has_head(5));
  EXPECT_TRUE(c.has_head(-28));
  EXPECT_TRUE(c.has_head(3));
  EXPECT_FALSE(c.has_head(70));
  EXPECT_FALSE(c.has_head(2));
  EXPECT_FALSE(c.has_head(0));
  EXPECT_FALSE(c.has_head(80));
}

TEST(CJClause, HasLiteralInClauseBody) {
  auto const c = cj::clause<int>({11, -28, 11, 3}, {-28, -28, 1, 5, -28, -28, 70});
  EXPECT_FALSE(c.empty());
  EXPECT_FALSE(c.has_body(11));
  EXPECT_TRUE(c.has_body(5));
  EXPECT_TRUE(c.has_body(-28));
  EXPECT_FALSE(c.has_body(3));
  EXPECT_TRUE(c.has_body(70));
  EXPECT_FALSE(c.has_body(2));
  EXPECT_FALSE(c.has_body(0));
  EXPECT_FALSE(c.has_body(80));
}

TEST(CJClause, HasLiteralInMultiClauseBody) {
  auto const c = cj::clause<int, cj::ordered_multiset>({11, -28, 11, 3}, {1, 5, -28, -28, 70});
  EXPECT_FALSE(c.empty());
  EXPECT_FALSE(c.has_body(11));
  EXPECT_TRUE(c.has_body(5));
  EXPECT_TRUE(c.has_body(-28));
  EXPECT_FALSE(c.has_body(3));
  EXPECT_TRUE(c.has_body(70));
  EXPECT_FALSE(c.has_body(2));
  EXPECT_FALSE(c.has_body(0));
  EXPECT_FALSE(c.has_body(80));
}

TEST(CJClause, RemoveLiteralFromClauseBody) {
  auto c = cj::clause<int, cj::flat_set>({11, -28, 11, 3}, {1, 5, -28, -28, 70});
  EXPECT_EQ(7, c.size());
  EXPECT_TRUE(c.has_body(-28));
  c.rmv_from_body(-28);
  EXPECT_FALSE(c.has_body(-28));
  EXPECT_EQ(6, c.size());
  c.rmv_from_body(0);
  c.rmv_from_body(6);
  c.rmv_from_body(42);
  EXPECT_EQ(6, c.size());
}

TEST(CJClause, RemoveLiteralFromMultiClauseBody) {
  auto c = cj::clause<int, cj::ordered_multiset>({11, -28, 11, 3}, {1, 5, -28, -28, 70});
  EXPECT_EQ(9, c.size());
  EXPECT_TRUE(c.has_body(-28));
  c.rmv_from_body(-28);
  EXPECT_TRUE(c.has_body(-28));
  EXPECT_EQ(8, c.size());
  c.rmv_from_body(0);
  c.rmv_from_body(6);
  c.rmv_from_body(42);
  EXPECT_EQ(8, c.size());
}

TEST(CJClause, RemoveLiteralFromClauseHead) {
  auto c = cj::clause<int, cj::flat_set>({11, -28, 11, 3}, {1, 5, -28, -28, 70});
  EXPECT_EQ(7, c.size());
  EXPECT_TRUE(c.has_head(11));
  c.rmv_from_head(11);
  EXPECT_FALSE(c.has_head(11));
  EXPECT_EQ(6, c.size());
  c.rmv_from_head(0);
  c.rmv_from_head(6);
  c.rmv_from_head(42);
  EXPECT_EQ(6, c.size());
}

TEST(CJClause, RemoveLiteralFromMultiClauseHead) {
  auto c = cj::clause<int, cj::ordered_multiset>({11, -28, 11, 3}, {1, 5, -28, -28, 70});
  EXPECT_EQ(9, c.size());
  EXPECT_TRUE(c.has_head(11));
  c.rmv_from_head(11);
  EXPECT_TRUE(c.has_head(11));
  EXPECT_EQ(8, c.size());
  c.rmv_from_head(0);
  c.rmv_from_head(6);
  c.rmv_from_head(42);
  EXPECT_EQ(8, c.size());
}
