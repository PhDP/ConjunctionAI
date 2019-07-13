#include "gtest/gtest.h"
#include "cj/logics/fuzzy_classifier.hh"

TEST(CJFuzzyClassifier, CreatesLukasiewiczInterpretation) {
  using luka = cj::lukasiewicz<double>;
  using classifier = cj::fuzzy_classifier<luka, double>;

  auto i = classifier::make_interpretation({"No-interaction", "Interaction"});

  i->add_triangular_sets("Body mass", 3, 0.0, 500.0);
  EXPECT_EQ(1, i->num_input());
  EXPECT_EQ(3, i->num_partitions(0));
  EXPECT_EQ(2, i->num_categories());
  EXPECT_DOUBLE_EQ(luka{1.0}.value, i->get(0, 0, -100.0).value);
  EXPECT_DOUBLE_EQ(luka{1.0}.value, i->get(0, 0, 0.0).value);
  EXPECT_DOUBLE_EQ(luka{0.5}.value, i->get(0, 0, 125.0).value);
  EXPECT_DOUBLE_EQ(luka{0.0}.value, i->get(0, 0, 250.0).value);
  EXPECT_DOUBLE_EQ(luka{0.0}.value, i->get(0, 1, 0.0).value);
  EXPECT_DOUBLE_EQ(luka{0.5}.value, i->get(0, 1, 125.0).value);
  EXPECT_DOUBLE_EQ(luka{1.0}.value, i->get(0, 1, 250.0).value);
  EXPECT_DOUBLE_EQ(luka{0.5}.value, i->get(0, 1, 375.0).value);
  EXPECT_DOUBLE_EQ(luka{0.0}.value, i->get(0, 1, 500.0).value);
  EXPECT_DOUBLE_EQ(luka{0.0}.value, i->get(0, 1, 900.0).value);
  EXPECT_DOUBLE_EQ(luka{0.0}.value, i->get(0, 2, 250.0).value);
  EXPECT_DOUBLE_EQ(luka{0.5}.value, i->get(0, 2, 375.0).value);
  EXPECT_DOUBLE_EQ(luka{1.0}.value, i->get(0, 2, 500.0).value);
  EXPECT_DOUBLE_EQ(luka{1.0}.value, i->get(0, 2, 600.0).value);
  EXPECT_EQ("Triangular_{3}(0, 500)", i->partition_name(0));
  EXPECT_EQ("is low", i->label(0, 0));
  EXPECT_EQ("is average", i->label(0, 1));
  EXPECT_EQ("is high", i->label(0, 2));

  i->add_triangular_sets("Brain mass", 9, 10.0, 50.0);
  EXPECT_EQ(2, i->num_input());
  EXPECT_EQ(3, i->num_partitions(0));
  EXPECT_EQ(9, i->num_partitions(1));
  EXPECT_EQ(2, i->num_categories());
  EXPECT_DOUBLE_EQ(luka{1.0}.value, i->get(1, 0, 5.0).value);
  EXPECT_DOUBLE_EQ(luka{1.0}.value, i->get(1, 3, 25.0).value);
  EXPECT_DOUBLE_EQ(luka{0.5}.value, i->get(1, 4, 27.5).value);
  EXPECT_DOUBLE_EQ(luka{0.0}.value, i->get(1, 7, 80.0).value);
  EXPECT_DOUBLE_EQ(luka{1.0}.value, i->get(1, 8, 51.0).value);
  EXPECT_EQ("Triangular_{9}(10, 50)", i->partition_name(1));
  EXPECT_EQ("is low3", i->label(1, 3));
  EXPECT_EQ("is average", i->label(1, 4));
  EXPECT_EQ("is high0", i->label(1, 5));

  auto c = classifier{i};
  EXPECT_TRUE(c.empty());
}

TEST(CJFuzzyClassifier, CreatesLukasiewiczClassifier) {
  using luka = cj::lukasiewicz<double>;
  using classifier = cj::fuzzy_classifier<luka, double>;

  auto i = classifier::make_interpretation({"No-interaction", "Interaction"});
  i->add_triangular_sets("Body mass", 3, 0.0, 500.0);
  i->add_triangular_sets("Brain mass", 3, 10.0, 50.0);
  i->add_triangular_sets("Whatever", 3, 0.0, 1.0);
  auto c = classifier{i};

  EXPECT_TRUE(c.empty());

  c.add_rule({{0, 2}, {2, 1}}, 0);

  EXPECT_EQ("If Body mass is high and Whatever is average then No-interaction\n", boost::lexical_cast<cj::string>(c));
  EXPECT_EQ(1, c.size());

  c.add_rule({{2, 1}, {0, 2}}, 0);

  EXPECT_EQ(1, c.size());

  c.add_rule({{1, 1}}, 1);

  EXPECT_EQ("If Body mass is high and Whatever is average then No-interaction\nIf Brain mass is average then Interaction\n", boost::lexical_cast<cj::string>(c));
  EXPECT_EQ(2, c.size());

  EXPECT_EQ(0, c.evaluate({500, 20, 0.3}));
  EXPECT_EQ(0, c.evaluate({400, 10, 0.0}));
  EXPECT_EQ(1, c.evaluate({400, 30, 0.0}));
  EXPECT_EQ(1, c.evaluate({400, 25, 0.0}));
  EXPECT_EQ(1, c.evaluate({400, 15, 0.0}));
  EXPECT_EQ(1, c.evaluate({100, 11, 0.0}));
  EXPECT_EQ(0, c.evaluate({450, 11, 0.5}));
}

TEST(CJFuzzyClassifier, EvaluatesDataMatrixFuzzyClassifier) {
  using luka = cj::lukasiewicz<double>;
  using classifier = cj::fuzzy_classifier<luka, double>;

  auto i = classifier::make_interpretation({"No-interaction", "Interaction"});
  i->add_triangular_sets("Body mass", 3, 0.0, 500.0);
  i->add_triangular_sets("Brain mass", 3, 10.0, 50.0);
  i->add_triangular_sets("Whatever", 3, 0.0, 1.0);
  auto c = classifier{i};

  c.add_rule({{0, 2}, {2, 1}}, 0);
  c.add_rule({{2, 1}, {0, 2}}, 0);
  c.add_rule({{1, 1}}, 1);

  auto dm = cj::data_matrix<double, uint32_t>{{"Body Mass", "Brain Mass", "Whatever"}, "Interaction"};
  using row_type = cj::pair<cj::vector<double>, uint32_t>;
  dm.add_row(row_type({500, 20, 0.3}, 1));
  dm.add_row(row_type({400, 10, 0.0}, 1));
  dm.add_row(row_type({400, 30, 0.0}, 1));
  dm.add_row(row_type({400, 25, 0.0}, 0));
  dm.add_row(row_type({400, 15, 0.0}, 1));
  dm.add_row(row_type({100, 11, 0.0}, 1));
  dm.add_row(row_type({450, 11, 0.5}, 0));
  EXPECT_EQ(7, dm.nrows());
  EXPECT_EQ(3, dm.ncols());

  auto const con = c.evaluate_all(dm);
  EXPECT_EQ(3, con.true_positives(1));
  EXPECT_EQ(1, con.false_positives(1));
  EXPECT_EQ(1, con.true_negatives(1));
  EXPECT_EQ(2, con.false_negatives(1));
  EXPECT_DOUBLE_EQ(0.5714285714285714, con.accuracy());
}

TEST(CJFuzzyClassifier, PickRandomRuleFromEmptyClassifier) {
  using god = cj::godel<float>;
  using classifier = cj::fuzzy_classifier<god, float>;

  auto c = classifier{classifier::make_interpretation({"Blah", "Bleh"})};
  auto rng = std::mt19937_64(std::random_device{}());

  EXPECT_TRUE(c.get_random_rule(rng).first.empty());
}

TEST(CJFuzzyClassifier, PickRandomRuleFromClassifierWithOneRule) {
  using prod = cj::product<double>;
  using classifier = cj::fuzzy_classifier<prod, double>;

  auto c = classifier{classifier::make_interpretation({"Blah", "Bleh"})};
  auto r = typename classifier::rule_type{{{0, 2}, {2, 1}}, 0};
  c.add_rule(r);

  auto rng = std::mt19937_64(std::random_device{}());
  EXPECT_EQ(1, c.size());
  EXPECT_EQ(r, c.get_random_rule(rng));
  EXPECT_EQ(1, c.size());
}

TEST(CJFuzzyClassifier, PickRandomRuleFromClassifierWithSeveralRules) {
  using prod = cj::product<double>;
  using classifier = cj::fuzzy_classifier<prod, double>;

  auto c = classifier{classifier::make_interpretation({"Blah", "Bleh"})};
  auto r0 = typename classifier::rule_type{{{0, 2}, {2, 1}}, 0};
  auto r1 = typename classifier::rule_type{{{1, 2}, {3, 0}}, 1};
  auto r2 = typename classifier::rule_type{{{0, 1}}, 2};
  c.add_rule(r0);
  c.add_rule(r1);
  c.add_rule(r2);

  auto rng = std::mt19937_64(std::random_device{}());
  EXPECT_EQ(3, c.size());
  auto picked = c.get_random_rule(rng);
  EXPECT_EQ(1, ((picked == r0) + (picked == r1) + (picked == r2)));
  EXPECT_EQ(3, c.size());
}

TEST(CJFuzzyClassifier, PopRandomRuleFromEmptyClassifier) {
  using god = cj::godel<float>;
  using classifier = cj::fuzzy_classifier<god, float>;

  auto c = classifier{classifier::make_interpretation({"Blah", "Bleh"})};
  auto rng = std::mt19937_64(std::random_device{}());
  EXPECT_TRUE(c.pop_random_rule(rng).first.empty());

  EXPECT_TRUE(c.pop_random_rule(rng).first.empty());
}

TEST(CJFuzzyClassifier, PopRandomRuleFromClassifierWithOneRule) {
  using prod = cj::product<double>;
  using classifier = cj::fuzzy_classifier<prod, double>;

  auto c = classifier{classifier::make_interpretation({"Blah", "Bleh"})};
  auto r = typename classifier::rule_type{{{0, 2}, {2, 1}}, 0};
  c.add_rule(r);

  auto rng = std::mt19937_64(std::random_device{}());
  EXPECT_EQ(1, c.size());
  EXPECT_EQ(r, c.pop_random_rule(rng));
  EXPECT_EQ(0, c.size());
}

TEST(CJFuzzyClassifier, PopRandomRuleFromClassifierWithSeveralRules) {
  using prod = cj::product<double>;
  using classifier = cj::fuzzy_classifier<prod, double>;

  auto c = classifier{classifier::make_interpretation({"Blah", "Bleh"})};
  auto r0 = typename classifier::rule_type{{{0, 2}, {2, 1}}, 0};
  auto r1 = typename classifier::rule_type{{{1, 2}, {3, 0}}, 1};
  auto r2 = typename classifier::rule_type{{{0, 1}}, 2};
  c.add_rule(r0);
  c.add_rule(r1);
  c.add_rule(r2);

  auto rng = std::mt19937_64(std::random_device{}());
  EXPECT_EQ(3, c.size());

  auto picked0 = c.pop_random_rule(rng);
  EXPECT_EQ(1, ((picked0 == r0) + (picked0 == r1) + (picked0 == r2)));
  EXPECT_EQ(2, c.size());

  auto picked1 = c.pop_random_rule(rng);
  EXPECT_EQ(1, ((picked1 == r0) + (picked1 == r1) + (picked1 == r2)));
  EXPECT_EQ(1, c.size());

  auto picked2 = c.pop_random_rule(rng);
  EXPECT_EQ(1, ((picked2 == r0) + (picked2 == r1) + (picked2 == r2)));
  EXPECT_EQ(0, c.size());

  EXPECT_TRUE(c.pop_random_rule(rng).first.empty());
  EXPECT_TRUE(c.pop_random_rule(rng).first.empty());
}
