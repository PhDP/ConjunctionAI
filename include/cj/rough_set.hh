#ifndef CJ_ROUGH_SET_HH_
#define CJ_ROUGH_SET_HH_

#include "cj/details/common.hh"

namespace cj {

  template<
    typename T,
    template<typename...> typename SetType = unordered_set
  >
  class rough_set {
   public:
    using element_type = T;
    using set_type = SetType<element_type>;
    using const_iterator = typename set_type::const_iterator;

    rough_set();

  }

} /* end namespace cj */

#endif
