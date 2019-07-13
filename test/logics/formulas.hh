#ifndef CJ_TEST_FORMULAS_HH_
#define CJ_TEST_FORMULAS_HH_

// # prop_formulas
//
// ## Equalities to test
// 0 == 1 == 2
// 5 == 6
//
// ## Inequalities to test
// 3 != 4
//
const auto prop_formulas = vector<string> {
/*  0 */ "a and b and c",
         "a and (b and c)",
         "((a and (b and c)))",
         "a and b or c",
         "a and (b or c)",
/*  5 */ "a or b and c",
         "a or (b and c)",
};

const auto fol_formulas = vector<string> {
/*  0 */ "Forall p: Smoking(p) => Cancer(p)",
         "\\forall p_0, p_1: Friends(p_0, p_1) \\land Friends(p_1, p_2) \\Rightarrow Friends(p_0, p_2)",
         "Foo(x)",
         "Foo(x) = 6",
         "x + 28 + 2 = 2^3^4",
/*  5*/  "x + (28 + 2) = (2^(3^4))",
};

#endif
