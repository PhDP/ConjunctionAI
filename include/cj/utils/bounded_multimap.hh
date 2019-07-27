/**
 * \brief A multimap that will accept a limited number of elements and remove its smallest
 *        element to make place for larger elements.
 */
#ifndef CJ_BOUNDED_MULTIMAP_HH_
#define CJ_BOUNDED_MULTIMAP_HH_

#include "cj/common.hh"
#include "cj/math/set.hh"

namespace cj {

  /**
   * \brief A multimap that accepts only a fixed number of elements. Will remove
   *        the smallest (key, value) pair when largest keys are being inserted
   *        and the container is full.
   */
  template<
    typename Key,
    typename T,
    template<typename, typename...> typename Multimap = ordered_multimap>
  class bounded_multimap {
   public:
    using key_type = Key;
    using mapped_type = T;
    using value_type = pair<const key_type, mapped_type>;
    using multimap_type = Multimap<key_type, mapped_type>;
    using key_set_type = typename map_traits<multimap_type>::key_set_type;
    using key_multiset_type = typename map_traits<multimap_type>::key_multiset_type;
    using value_set_type = typename map_traits<multimap_type>::value_set_type;
    using value_multiset_type = typename map_traits<multimap_type>::value_multiset_type;
    using const_iterator = typename multimap_type::const_iterator;
    using const_reverse_iterator = typename multimap_type::const_reverse_iterator;

    /**
     * \brief Creates a bounded_multimap by setting its maximum number of elements.
     */
    bounded_multimap(size_t max_size) : m_max_size{max_size} {};

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
    auto try_insert(key_type const& k, mapped_type const& m) -> bool;

    /**
     * \brief Returns the set of keys in the container.
     */
    auto set_of_keys() const -> key_set_type;

    /**
     * \brief Returns the multiset of keys in the container.
     */
    auto multiset_of_keys() const -> key_multiset_type;

    /**
     * \brief Returns the set of values (mapped_type).
     */
    auto set_of_values() const -> value_set_type;

    /**
     * \brief Returns the multiset of values (mapped_type).
     */
    auto multiset_of_values() const -> value_multiset_type;

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
     * \brief (Key, Value) pair with the smallest key.
     */
    auto minimum() const -> value_type const& {
      return *m_values.begin();
    }

    /**
     * \brief Smallest key.
     */
    auto minimum_key() const -> key_type const& {
      return m_values.begin()->first;
    }

    /**
     * \brief (Key, Value) pair with the largest key.
     */
    auto maximum() const -> value_type const& {
      return *m_values.rbegin();
    }

    /**
     * \brief Largest key.
     */
    auto maximum_key() const -> key_type const& {
      return m_values.rbegin()->first;
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
    multimap_type m_values;
    size_t m_max_size;
  };

  template<typename K, typename T, template<typename, typename...> typename M>
  auto bounded_multimap<K, T, M>::try_insert(key_type const& k, mapped_type const& m) -> bool {
    if (m_values.size() < m_max_size) {
      m_values.insert(value_type(k, m));
      return true;
    } else if (m_values.size() == m_max_size && m_values.begin()->first < k) {
      m_values.erase(m_values.begin());
      m_values.insert(value_type(k, m));
      return true;
    }
    return false;
  }

  template<typename K, typename T, template<typename, typename...> typename M>
  auto bounded_multimap<K, T, M>::set_of_keys() const -> key_set_type {
    auto s = key_set_type{};
    for (auto const& elem : m_values) {
      s.insert(elem.first);
    }
    return s;
  }

  template<typename K, typename T, template<typename, typename...> typename M>
  auto bounded_multimap<K, T, M>::multiset_of_keys() const -> key_multiset_type {
    auto s = key_multiset_type{};
    for (auto const& elem : m_values) {
      s.insert(elem.first);
    }
    return s;
  }

  template<typename K, typename T, template<typename, typename...> typename M>
  auto bounded_multimap<K, T, M>::set_of_values() const -> value_set_type {
    auto s = value_set_type{};
    for (auto const& elem : m_values) {
      s.insert(elem.second);
    }
    return s;
  }

  template<typename K, typename T, template<typename, typename...> typename M>
  auto bounded_multimap<K, T, M>::multiset_of_values() const -> value_multiset_type {
    auto s = value_multiset_type{};
    for (auto const& elem : m_values) {
      s.insert(elem.second);
    }
    return s;
  }

} /* end namespace cj */

#endif
