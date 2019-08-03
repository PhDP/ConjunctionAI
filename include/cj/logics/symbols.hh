#ifndef CJ_SYMBOLS_HH_
#ifndef CJ_SYMBOLS_HH_

#include "cj/common.hh"

namespace cj {

  class symbols {
   public:
    symbols(
      map_type<unary_kind, string> const& u,
      map_type<binary_kind, string> const& b,
      map_type<quantifier_kind, string> const& q,
      map_type<string, string> const& others) : m_u{u}, m_b{b}, m_q{q}, m_others(others) {}

    auto operator()(unary_kind k) const -> string {
      return m_u.at(k);
    }

    auto operator()(binary_kind k) const -> string {
      return m_b.at(k);
    }

    auto operator()(quantifier_kind k) const -> string {
      return m_q.at(k);
    }

    auto operator()(string const& s) const -> string {
      return m_others.at(s);
    }

   private:
    flat_map<unary_kind, string> m_u;
    flat_map<binary_kind, string> m_b;
    flat_map<quantifier_kind, string> m_q;
    flat_map<string, string> m_others;
  };

  static const auto unicode = logic_symbols{
    {{unary_kind::negation,"¬"}, {unary_kind::delta,"Δ"}},
    {{binary_kind::conjunction,"⊗"}, {binary_kind::weak_conjunction,"∧"},
      {binary_kind::disjunction,"⊕"}, {binary_kind::weak_disjunction,"∨"},
      {binary_kind::implication,"⇒"}, {binary_kind::equivalence,"⇔"},
      {binary_kind::ex_disjunction,"⊻"}},
    {{quantifier_kind::universal,"∀"}, {quantifier_kind::existential,"∃"},
      {quantifier_kind::unique,"∃!"}},
    {}
  };

} /* end namespace cj */

#endif
