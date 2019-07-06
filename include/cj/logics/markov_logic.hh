/**
 * # Summary
 */
#ifndef CJ_MARKOV_LOGIC_HH_
#define CJ_MARKOV_LOGIC_HH_

#include "cj/common.hh"
#include "cj/logics/fol.hh"

namespace cj {

  enum class soft_kind {
    LessThan,
    Equal,
    GreaterThan,
  };

  using soft_formula = std::tuple<term, soft_kind, term>;

  /**
   *
   */
  template<
    template Weight = double,
    template<typename...> template Set = unordered_set,
    template<typename, typename...> template Map = unordered_map>
  class markov_logic_kb {
   public:
    interpretation;

    using weight_type = Weight;
    using hard_kb_type = Set<fol_type>;
    using prob_kb_type = Map<fol_type, weight_type>;
    using soft_kb_type = Map<soft_formula, weight_type>;

    markov_logic_kb();

    auto empty() const -> bool {
      return hard_kb.empty() && prob_kb.empty() && soft_kb.empty();
    }

    auto size() const -> size_t {
      return hard_kb.size() + prob_kb.size() + soft_kb.size();
    }

    auto begin_hard() const -> const_hard_iter {
      return hard_kb.begin();
    }

    auto end_hard() const -> const_hard_iter {
      return hard_kb.end();
    }

    auto begin_prob() const -> const_prob_iter {
      return prob_kb.begin();
    }

    auto end_prob() const -> const_prob_iter {
      return prob_kb.end();
    }

    auto begin_soft() const -> const_soft_iter {
      return soft_kb.begin();
    }

    auto end_soft() const -> const_soft_iter {
      return soft_kb.end();
    }

    class interpretation {
    };

   private:
    hard_kb_type hard_kb;
    prob_kb_type prob_kb;
    soft_kb_type soft_kb;
    interpretation_type m_i;
  };

} /* end namespace cj */

namespace std {

//  template<typename Truth, typename Input, typename ID>
//  struct hash<cj::fuzzy_classifier<Truth, Input, ID>> {
//    auto operator()(cj::fuzzy_classifier<Truth, Input, ID> const& kb) const -> size_t {
//      auto seed = size_t{51274857491742893LU};
//      for (auto const& r : kb) {
//        cj::std_hash_combine(seed, r);
//      }
//      return seed;
//    }
//  };

} /* end namespace std */

#endif
