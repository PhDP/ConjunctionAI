#ifndef CJ_TEST_FORMULAS_HH_
#define CJ_TEST_FORMULAS_HH_

const auto prop_formulas = vector<string> {
  "a and b and c",
  "a and (b and c)",
  "a and b or c",
  "a and (b or c)"
};

const auto fol_formulas = vector<string> {
  "Forall p: Smoking(p) => Cancer(p)",
  "\\forall p_0, p_1: Friends(p_0, p_1) \\land Friends(p_1, p_2) \\Rightarrow Friends(p_0, p_2)",
  "Foo(x)",
  "Foo(x) = 6",
  "x + 28 + 2 = 2^3^4",
  "x + (28 + 2) = (2^(3^4))",
};

#endif
