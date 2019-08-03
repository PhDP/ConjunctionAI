#include "gtest/gtest.h"
#include "cj/logics/formula.hh"

TEST(CJFormula, FormulaInHash) {
  using fm = cj::formula<cj::string>;
  auto kb = cj::unordered_set<fm>{};

  auto x = !!!!!!!!!(!fm{cj::string{"x"}} & !!!!fm{cj::string{"y"}});
  auto y = cj::double_neg_elim(x);
  auto z = cj::double_neg_elim(x);

  kb.insert(std::move(x));
  kb.insert(std::move(y));
  kb.insert(std::move(z));

  EXPECT_EQ(2, kb.size());
}
