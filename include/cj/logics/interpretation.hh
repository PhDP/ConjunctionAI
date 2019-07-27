/**
 *
 */
#ifndef CJ_INTERPRETATION_HH_
#define CJ_INTERPRETATION_HH_

#include "cj/common.hh"

namespace cj {

  template<
    template<typename, typename...> typename MapType = unordered_map>
  class interpretation {
   public:
    using domain_name_type = string;
    using symbol_name_type = string;
    using domain_type = ???

    interpretation() {};

    auto add_domain(domain_name_type const& name, domain_type const& domain) -> bool;

    auto add_symbol(symbol_name_type const& name, vector<domain_name_type> const& domains) -> bool;

   private:
    map_type<domain_name_type, domain> m_domains;
    map_type<string, vector<domain_name_type>> m_symbols;
  };

} /* end namespace cj */

#endif
