/**
 * \brief
 */
#ifndef CJ_FUZZY_SET_HH_
#define CJ_FUZZY_SET_HH_

#include "cj/details/common.hh"

namespace cj {

  /**
   * \brief A class for discrete fuzzy sets.
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

    auto empty() -> bool {
      return m_set.empty();
    }

    auto size() -> size_t {
      return m_set.size();
    }

    auto insert(element_type const& e, membership_type m) -> void {
      m_set[e] = m;
    }

    auto remove(element_type const& e) -> void {
      m_set.erase(e);
    }

    auto membership(T const& t) const -> membership_type {
      auto it = m_set.find(t);
      return it != m_set.end()? it->second : membership_type{0};
    }

    auto operator[](T const& t) const -> membership_type {
      auto it = m_set.find(t);
      return it != m_set.end()? it->second : membership_type{0};
    }

    auto highest() const -> membership_type {
      auto m = membership_type{0};
      for (auto const& e : m_set) {
        if (e.second > m) {
          m = e.second;
        }
      }
      return m;
    }

    auto cardinality() const -> membership_type {
      return std::accumulate(m_set.begin(), m_set.end(), membership_type{0});
    }

    auto begin() -> const_iterator {
      return m_set.begin();
    }

    auto end() -> const_iterator {
      return m_set.end();
    }

   private:
    fuzzy_set_type m_set;
  };

  template<typename T, typename M, template<typename, typename...> typename MT>
  auto operator<<(std::ostream& os, fuzzy_set<T, M, MT> const& f) -> std::ostream& {
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
