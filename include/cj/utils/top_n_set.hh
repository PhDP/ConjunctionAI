/**
 * \brief An ordered set/multiset that will accept a limited number of elements and remove its
 *        smallest element to make place for larger elements.
 */
#ifndef CJ_TOP_N_SET_HH_
#define CJ_TOP_N_SET_HH_

#include "cj/common.hh"
#include "cj/math/set.hh"
#include "cj/utils/string.hh"

namespace cj {

  /**
   * \brief A set that accepts only a fixed number of elements. Will remove the smallest key when
   *        largest keys are being inserted and the container is full.
   */
  template<
    typename Key,
    template<typename...> typename Set = ordered_set>
  class top_n_set {
   public:
    using key_type = Key;
    using set_type = Set<key_type>;
    using const_iterator = typename set_type::const_iterator;
    using const_reverse_iterator = typename set_type::const_reverse_iterator;

    /**
     * \brief Creates a top_n_set by setting its maximum number of elements.
     */
    top_n_set(size_t max_size) : m_max_size{max_size} {};

    /**
     * \brief Creates a top_n_set with a max_size and an initializer_list.
     */
    top_n_set(size_t max_size, std::initializer_list<key_type> const& keys);

    /**
     * \brief Whether the container is empty.
     */
    auto empty() const -> bool {
      return m_values.empty();
    }

    /**
     * \brief Whether the container is full.
     */
    auto is_full() const -> bool {
      return m_values.size() == m_max_size;
    }

    /**
     * \brief Current number of elements.
     */
    auto size() const -> size_t {
      return m_values.size();
    }

    /**
     * \brief Maximum tolerated number of elements.
     */
    auto max_size() const -> size_t {
      return m_max_size;
    }

    /**
     * \brief Removes all elements from the container.
     */
    auto clear() {
      m_values.clear();
    }

    /**
     * \brief Inserts if the key is bigger than the minimum in the container (and if so, removes
     *        the previous minimum).
     */
    template<typename K = Key>
    auto try_insert(key_type const& k,
                    typename std::enable_if<set_traits<Set<K>>::is_multi, int>::type* = 0) -> bool;

    /**
     * \brief Inserts if the key is bigger than the minimum in the container (and if so, removes
     *        the previous minimum).
     */
    template<typename K = Key>
    auto try_insert(key_type const& k,
                    typename std::enable_if<!set_traits<Set<K>>::is_multi, int>::type* = 0) -> bool;

    /**
     * \brief Returns the number of elements matching specific key.
     */
    auto count(key_type const& k) const -> size_t {
      return m_values.count(k);
    }

    /**
     * \brief Finds element with specific key.
     */
    auto find(key_type const& k) const -> const_iterator {
      return m_values.find(k);
    }

    /**
     * \brief The smallest key.
     */
    auto minimum() const -> key_type const& {
      return *m_values.begin();
    }

    /**
     * \brief The largest key.
     */
    auto maximum() const -> key_type const& {
      return *m_values.rbegin();
    }

    /**
     * \brief Iterator at the smallest key.
     */
    auto begin() const -> const_iterator {
      return m_values.begin();
    }

    /**
     * \brief End of the iterator from smallest to largest keys.
     */
    auto end() const -> const_iterator {
      return m_values.end();
    }

    /**
     * \brief Iterator at the largest key.
     */
    auto rbegin() const -> const_reverse_iterator {
      return m_values.rbegin();
    }

    /**
     * \brief End of the iterator from largest to smallest keys.
     */
    auto rend() const -> const_reverse_iterator {
      return m_values.rend();
    }

   private:
    set_type m_values;
    size_t m_max_size;
  };

  template<
    typename Key,
    template<typename...> typename Multiset = ordered_multiset>
  using top_n_multiset = top_n_set<Key, Multiset>;

  template<typename K, template<typename...> typename S>
  top_n_set<K, S>::top_n_set(size_t max_size, std::initializer_list<key_type> const& keys)
    : m_max_size{max_size} {
    for (auto const& key : keys) {
      try_insert(key);
    }
  }

  template<typename K, template<typename...> typename S> template<typename K0>
  auto top_n_set<K, S>::try_insert(key_type const& k,
                                  typename std::enable_if<set_traits<S<K0>>::is_multi, int>::type*)
                                  -> bool {
    if (m_values.size() < m_max_size) {
      m_values.insert(k);
      return true;
    } else if (*m_values.begin() < k) {
      m_values.erase(m_values.begin());
      m_values.insert(k);
      return true;
    }
    return false;
  }

  template<typename K, template<typename...> typename S> template<typename K0>
  auto top_n_set<K, S>::try_insert(key_type const& k,
                                  typename std::enable_if<!set_traits<S<K0>>::is_multi, int>::type*)
                                  -> bool {
    if (m_values.size() < m_max_size) {
      m_values.insert(k);
      return true;
    } else if (*m_values.begin() < k && m_values.find(k) == m_values.end()) {
      m_values.erase(m_values.begin());
      m_values.insert(k);
      return true;
    }
    return false;
  }

  template<typename K, template<typename...> typename S>
  auto operator<<(std::ostream& os, top_n_set<K, S> const& s) -> std::ostream& {
    os << '{' << intersperse(s.begin(), s.end()) << '}';
    return os;
  }

} /* end namespace cj */

#endif
