#include "gtest/gtest.h"
#include "cj/logics/fuzzy_classifier.hh"

TEST(CJFuzzyClassifier, CreatesLukasiewiczClassifier) {
  using classifier = cj::fuzzy_classifier<cj::lukasiewicz<double>, double, uint32_t>;
  auto i = classifier::make_interpretation({"Apple", "Cherry", "Orange"});
  auto c = classifier{i};
  EXPECT_TRUE(c.empty());
}
