/**
 * \brief Type traits for maps/sets, along with traditional set operations (union, intersection, ...).
 */
#ifndef CJ_SET_HH_
#define CJ_SET_HH_

#include "cj/common.hh"

namespace cj {

  /**
   * \brief The map trait is used to find the corresponding set type to every map, e.g. std::map
   *        has std::set, boost::container::flat_map has boost::container::flat_set, etc etc.
   *        Each specialization must define this corresponding set type as "set_type".
   */
  template<typename Container>
  struct map_traits;

  template<typename Key, typename Value>
  struct map_traits<flat_map<Key, Value>> {
    static const bool is_ordered = true;
    static const bool is_multi = false;
    using key_set_type = flat_set<Key>;
    using key_multiset_type = flat_multiset<Key>;
    using value_set_type = flat_set<Value>;
    using value_multiset_type = flat_multiset<Value>;
  };

  template<typename Key, typename Value>
  struct map_traits<flat_multimap<Key, Value>> {
    static const bool is_ordered = true;
    static const bool is_multi = true;
    using key_set_type = flat_set<Key>;
    using key_multiset_type = flat_multiset<Key>;
    using value_set_type = flat_set<Value>;
    using value_multiset_type = flat_multiset<Value>;
  };

  template<typename Key, typename Value>
  struct map_traits<ordered_map<Key, Value>> {
    static const bool is_ordered = true;
    static const bool is_multi = false;
    using key_set_type = ordered_set<Key>;
    using key_multiset_type = ordered_multiset<Key>;
    using value_set_type = ordered_set<Value>;
    using value_multiset_type = ordered_multiset<Value>;
  };

  template<typename Key, typename Value>
  struct map_traits<ordered_multimap<Key, Value>> {
    static const bool is_ordered = true;
    static const bool is_multi = true;
    using key_set_type = ordered_set<Key>;
    using key_multiset_type = ordered_multiset<Key>;
    using value_set_type = ordered_set<Value>;
    using value_multiset_type = ordered_multiset<Value>;
  };

  template<typename Key, typename Value>
  struct map_traits<unordered_map<Key, Value>> {
    static const bool is_ordered = false;
    static const bool is_multi = false;
    using key_set_type = unordered_set<Key>;
    using key_multiset_type = unordered_multiset<Key>;
    using value_set_type = unordered_set<Value>;
    using value_multiset_type = unordered_multiset<Value>;
  };

  template<typename Key, typename Value>
  struct map_traits<unordered_multimap<Key, Value>> {
    static const bool is_ordered = false;
    static const bool is_multi = true;
    using key_set_type = unordered_set<Key>;
    using key_multiset_type = unordered_multiset<Key>;
    using value_set_type = unordered_set<Value>;
    using value_multiset_type = unordered_multiset<Value>;
  };

  /**
   * \brief Traits for sets.
   */
  template<typename Container>
  struct set_traits;

  template<typename Key>
  struct set_traits<flat_set<Key>> {
    static const bool is_ordered = true;
    static const bool is_multi = false;
  };

  template<typename Key>
  struct set_traits<flat_multiset<Key>> {
    static const bool is_ordered = true;
    static const bool is_multi = true;
  };

  template<typename Key>
  struct set_traits<ordered_set<Key>> {
    static const bool is_ordered = true;
    static const bool is_multi = false;
  };

  template<typename Key>
  struct set_traits<ordered_multiset<Key>> {
    static const bool is_ordered = true;
    static const bool is_multi = true;
  };

  template<typename Key>
  struct set_traits<unordered_set<Key>> {
    static const bool is_ordered = false;
    static const bool is_multi = false;
  };

  template<typename Key>
  struct set_traits<unordered_multiset<Key>> {
    static const bool is_ordered = false;
    static const bool is_multi = true;
  };

  /**
   * \brief Returns the size of the union of two sets.
   */
  template<
    typename Set,
    typename std::enable_if<set_traits<Set>::is_ordered, int>::type = 0>
  auto set_union_size(Set const& xs, Set const& ys) -> size_t {
    if (xs.empty()) {
      return ys.size();
    }
    if (ys.empty()) {
      return xs.size();
    }
    auto count = size_t{0};
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();

    while (xs_it != xs_end && ys_it != ys_end) {
      ++count;
      if (!(*xs_it < *ys_it) && !(*ys_it < *xs_it)) {
        ++xs_it;
        ++ys_it;
      } else if (*xs_it < *ys_it) {
        ++xs_it;
      } else { // *ys_it < *xs_it
        ++ys_it;
      }
    }
    while (xs_it++ != xs_end) {
      ++count;
    }
    while (ys_it++ != ys_end) {
      ++count;
    }
    return count;
  }

  /**
   * \brief Returns the size of the union of two sets.
   */
  template<
    typename Set,
    typename std::enable_if<!set_traits<Set>::is_ordered, int>::type = 0>
  auto set_union_size(Set const& xs, Set const& ys) -> size_t {
    if (xs.size() < ys.size()) {
      return set_union_size(ys, xs);
    }
    auto count = xs.size();
    for (auto const& y : ys) {
      count += (xs.find(y) == xs.end());
    }
    return count;
  }

  /**
   * \brief Returns the union of two sets.
   */
  template<
    typename Set,
    typename std::enable_if<set_traits<Set>::is_ordered, int>::type = 0>
  auto set_union(Set const& xs, Set const& ys) -> Set {
    if (xs.empty()) {
      return ys;
    }
    if (ys.empty()) {
      return xs;
    }

    auto u = Set{};

    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();

    while (xs_it != xs_end && ys_it != ys_end) {
      if (!(*xs_it < *ys_it) && !(*ys_it < *xs_it)) {
        u.emplace_hint(u.end(), *xs_it++);
        ++ys_it;
      } else if (*xs_it < *ys_it) {
        u.emplace_hint(u.end(), *xs_it++);
      } else { // *ys_it < *xs_it
        u.emplace_hint(u.end(), *ys_it++);
      }
    }
    while (xs_it != xs_end) {
      u.emplace_hint(u.end(), *xs_it++);
    }
    while (ys_it != ys_end) {
      u.emplace_hint(u.end(), *ys_it++);
    }
    return u;
  }

  /**
    \brief Returns the union of two sets.
   */
  template<
    typename Set,
    typename std::enable_if<!set_traits<Set>::is_ordered, int>::type = 0>
  auto set_union(Set const& xs, Set const& ys) -> Set {
    if (xs.size() < ys.size()) {
      return set_union(ys, xs);
    }
    auto u = xs;
    u.insert(ys.begin(), ys.end());
    return u;
  }

  /**
    \brief Returns the size of the intersection two sets.
   */
  template<
    typename Set,
    typename std::enable_if<set_traits<Set>::is_ordered, int>::type = 0>
  auto set_intersection_size(Set const& xs, Set const& ys) -> size_t {
    if (xs.empty() || ys.empty()) {
      return 0;
    }
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();
    auto count = size_t{0};

    while (xs_it != xs_end && ys_it != ys_end) {
      if (*xs_it < *ys_it) {
        ++xs_it;
      } else {
        count += !(*ys_it < *xs_it);
        ++ys_it;
      }
    }
    return count;
  }

  /**
    \brief Returns the size of the intersection two sets.
   */
  template<
    typename Set,
    typename std::enable_if<!set_traits<Set>::is_ordered, int>::type = 0>
  auto set_intersection_size(Set const& xs, Set const& ys) -> size_t {
    if (xs.empty() || ys.empty()) {
      return 0;
    }
    if (xs.size() < ys.size()) {
      return set_intersection_size(ys, xs);
    }
    auto count = size_t{0};
    for (auto const& y : ys) {
      count += (xs.find(y) != xs.end());
    }
    return count;
  }

  /**
   * \brief Whether the intersection of two sets is empty.
   */
  template<
    typename Set,
    typename std::enable_if<set_traits<Set>::is_ordered, int>::type = 0>
  auto empty_set_intersection(Set const& xs, Set const& ys) -> bool {
    if (xs.empty() || ys.empty()) {
      return true;
    }
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();

    while (xs_it != xs_end && ys_it != ys_end) {
      if (*xs_it < *ys_it) {
        ++xs_it;
      } else {
        if (!(*ys_it < *xs_it)) {
          return false;
        }
        ++ys_it;
      }
    }
    return true;
  }

  /**
   * \brief Whether the intersection of two sets is empty.
   */
  template<
    typename Set,
    typename std::enable_if<!set_traits<Set>::is_ordered, int>::type = 0>
  auto empty_set_intersection(Set const& xs, Set const& ys) -> bool {
    if (xs.size() < ys.size()) {
      return empty_set_intersection(ys, xs);
    }
    for (auto const& y : ys) {
      if (xs.find(y) != xs.end()) {
        return false;
      }
    }
    return true;
  }

  /**
   * \brief Returns the intersection two ordered sets.
   */
  template<
    typename Set,
    typename std::enable_if<set_traits<Set>::is_ordered, int>::type = 0>
  auto set_intersection(Set const& xs, Set const& ys) -> Set {
    auto inter = Set{};
    if (xs.empty() || ys.empty()) {
      return inter;
    }
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();

    while (xs_it != xs_end && ys_it != ys_end) {
      if (*xs_it < *ys_it) {
        ++xs_it;
      } else {
        if (!(*ys_it < *xs_it)) {
          inter.emplace_hint(inter.end(), *xs_it++);
        }
        ++ys_it;
      }
    }
    return inter;
  }

  /**
    \brief Returns the intersection two unordered sets.
   */
  template<
    typename Set,
    typename std::enable_if<!set_traits<Set>::is_ordered, int>::type = 0>
  auto set_intersection(Set const& xs, Set const& ys) -> Set {
    if (xs.size() < ys.size()) {
      return set_intersection(ys, xs);
    }
    auto inter = Set{};
    if (xs.empty() || ys.empty()) {
      return inter;
    }
    for (auto const& y : ys) {
      if (xs.find(y) != xs.end()) {
        inter.insert(y);
      }
    }
    return inter;
  }

  /**
   * \brief Size of the set difference.
   */
  template<
    typename Set,
    typename std::enable_if<set_traits<Set>::is_ordered, int>::type = 0>
  auto set_difference_size(Set const& xs, Set const& ys) -> size_t {
    if (xs.empty()) {
      return 0;
    }
    if (ys.empty()) {
      return xs.size();
    }
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();
    auto count = xs.size();

    while (xs_it != xs_end && ys_it != ys_end) {
      auto const x_lt_y = *xs_it < *ys_it;
      auto const y_lt_x = *ys_it < *xs_it;
      if (x_lt_y && !y_lt_x) {
        ++xs_it;
      } else if (!x_lt_y) {
        ++ys_it;
        if (!y_lt_x) {
          --count;
          ++xs_it;
        }
      }
    }
    return count;
  }

  /**
   * \brief Size of the set difference.
   */
  template<
    typename Set,
    typename std::enable_if<!set_traits<Set>::is_ordered, int>::type = 0>
  auto set_difference_size(Set const& xs, Set const& ys) -> size_t {
    auto count = xs.size();
    for (auto const& y : ys) {
      count -= (xs.find(y) != xs.end());
    }
    return count;
  }

  /**
   * \brief Set difference.
   */
  template<
    typename Set,
    typename std::enable_if<set_traits<Set>::is_ordered, int>::type = 0>
  auto set_difference(Set const& xs, Set const& ys) -> Set {
    auto diff = Set{};
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();

    while (xs_it != xs_end && ys_it != ys_end) {
      auto const x_lt_y = *xs_it < *ys_it;
      auto const y_lt_x = *ys_it < *xs_it;
      if (x_lt_y && !y_lt_x) {
        diff.emplace_hint(diff.end(), *xs_it++);
      } else if (!x_lt_y) {
        ++ys_it;
        if (!y_lt_x) {
          ++xs_it;
        }
      }
    }
    while (xs_it != xs_end) {
      diff.emplace_hint(diff.end(), *xs_it++);
    }
    return diff;
  }

  /**
   * \brief Set difference.
   */
  template<
    typename Set,
    typename std::enable_if<!set_traits<Set>::is_ordered, int>::type = 0>
  auto set_difference(Set const& xs, Set const& ys) -> Set {
    auto diff = xs;
    for (auto const& y : ys) {
      diff.erase(y);
    }
    return diff;
  }

  /**
   * \brief Tanimoto similarity on two vectors.
   */
  template<typename T>
  auto tanimoto(vector<T> const& xs, vector<T> const& ys) -> double {
    if (xs.empty() || ys.empty()) {
      return 0;
    }
    auto and_count = 0, or_count = 0;
    auto const n = std::min(xs.size(), ys.size());
    for (auto i = 0u; i < n; ++i) {
      and_count += (xs[i] && ys[i]);
      or_count += (xs[i] || ys[i]);
    }
    return double(and_count) / or_count;
  }
  /**
   * \brief Computes the Tanimoto similarity index between two sets (size of the
   *        intersection divided by the size of the union of the sets).
   */
  template<typename Set>
  auto tanimoto(Set const& xs, Set const& ys) -> double {
    if (xs.empty() || xs.empty()) {
      return 0;
    }
    auto const i_size = set_intersection_size(xs, ys);
    return double(i_size) / (xs.size() + ys.size() - i_size);
  }

  template<typename T>
  auto tanimoto_distance(vector<T> const& xs, vector<T> const& ys) -> double {
    return 1.0 - tanimoto(xs, ys);
  }

  template<typename Set>
  auto tanimoto_distance(Set const& xs, Set const& ys) -> double {
    return 1.0 - tanimoto(xs, ys);
  }

  /**
   * \brief Returns a set with all the elements at the intersection of two sets plus a 50% chance
   *        of including any element found in only one set.
   */
  template<
    typename Set,
    typename std::enable_if<set_traits<Set>::is_ordered, int>::type = 0>
  auto set_intersection_split_union(Set const& xs, Set const& ys, std::mt19937_64& rng) -> Set {
    auto unif = std::uniform_real_distribution<double>(0, 1);
    auto s = Set{};
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();

    while (xs_it != xs_end && ys_it != ys_end) {
      if (*xs_it < *ys_it) {
        if (unif(rng) < 0.5) {
          s.emplace_hint(s.end(), *xs_it);
        }
        ++xs_it;
      } else {
        if (!(*ys_it < *xs_it)) {
          s.emplace_hint(s.end(), *xs_it++);
        } else if (unif(rng) < 0.5) {
          s.emplace_hint(s.end(), *ys_it);
        }
        ++ys_it;
      }
    }
    for (; xs_it != xs_end;  ++xs_it) {
      if (unif(rng) < 0.5) {
        s.emplace_hint(s.end(), *xs_it);
      }
    }
    for (; ys_it != ys_end; ++ys_it) {
      if (unif(rng) < 0.5) {
        s.emplace_hint(s.end(), *ys_it);
      }
    }
    return s;
  }

  /**
   * \brief Returns a map with all the elements at the intersection of two map plus a 50% chance
   *        of including any element found in only one set. For keys at the intersection, choose
   *        the mapped value randomly among the two sets.
   */
  template<
    typename Map,
    typename std::enable_if<map_traits<Map>::is_ordered, int>::type = 0>
  auto map_intersection_split_union(Map const& xs, Map const& ys, std::mt19937_64& rng) -> Map {
    auto unif = std::uniform_real_distribution<double>(0, 1);
    auto m = Map{};
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();

    while (xs_it != xs_end && ys_it != ys_end) {
      if (xs_it->first < ys_it->first) {
        if (unif(rng) < 0.5) {
          m.emplace_hint(m.end(), *xs_it);
        }
        ++xs_it;
      } else {
        if (!(ys_it->first < xs_it->first)) {
          if (unif(rng) < 0.5) {
            m.emplace_hint(m.end(), *xs_it);
          } else {
            m.emplace_hint(m.end(), *ys_it);
          }
          ++xs_it;
        } else if (unif(rng) < 0.5) {
          m.emplace_hint(m.end(), *ys_it);
        }
        ++ys_it;
      }
    }
    for (; xs_it != xs_end;  ++xs_it) {
      if (unif(rng) < 0.5) {
        m.emplace_hint(m.end(), *xs_it);
      }
    }
    for (; ys_it != ys_end; ++ys_it) {
      if (unif(rng) < 0.5) {
        m.emplace_hint(m.end(), *ys_it);
      }
    }
    return m;
  }

} /* end namespace cj */

#endif
