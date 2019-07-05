/**
 * # Summary
 */
#ifndef CJ_MARKOV_LOGIC_HH_
#define CJ_MARKOV_LOGIC_HH_

#include "cj/common.hh"
#include "cj/logics/fol.hh"

namespace cj {

  /**
   *
   */
  template<
    template<typename...> template Set,
    template<typename, typename...> template Map>
  class markov_logic {
   public:
    using fol_set = 
    using 

    markov_logic();

   private:
    rules_type m_rules;
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
