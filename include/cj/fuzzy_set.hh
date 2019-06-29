/**
 * \brief
 */
#ifndef CJ_FUZZY_SET_HH_
#define CJ_FUZZY_SET_HH_

#include "cj/common.hh"

namespace cj {

  /**
   * \brief A function as a name and a vector of terms.
   *
   * \tparam Term   Type of the term inside the function.
   * \tparam Name   Type of the function's name (default: string).
   */
  template<
    typename T,
    typename Membership = double,
    template<typename, typename...> typename MapType = unordered_map
  >
  class fuzzy_set {
   public:
    using element_type = T;
    using membership_type = Membership;
    using fuzzy_set_type = MapType<element_type, membership_type>;
    using const_iterator = typename fuzzy_set_type::const_iterator;

    /**
     * \brief Builds a function with a name and a list of arguments.
     */
    fuzzy_set()
      : m_set{} {
    }

    auto insert() {
    }

    auto remove() {
    }

    auto membership(T const& t) const -> membership_type {
      auto it = m_set.find(t);
      return it != m_set.end()? it->second : membership_type{0};
    }

    auto operator[](T const& t) const -> membership_type {
      auto it = m_set.find(t);
      return it != m_set.end()? it->second : membership_type{0};
    }

    auto cardinality() const -> membership_type {
      auto c = membership_type{0};
      for (auto const& elem : m_set) {
        c += elem.second;
      }
      return c;
    }

   private:
    fuzzy_set_type m_set;
  };

  template<typename T, typename Membership, template<typename, typename...> typename MapType>
  auto operator<<(std::ostream& os, fuzzy_set<T, Membership, MapType> const& f) -> std::ostream& {
    os << '{';
    if (!f.empty()) {
      auto it = f.begin();
      os << it->first << " / " << it->second;
      ++it;
      for (; it != f.end(); ++it) {
        os << ", " << it->first << " / " << it->second;
      }
    }
    os << '}';
  }

} /* end namespace cj */

#endif
