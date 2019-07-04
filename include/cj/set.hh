/**
 * # Summary
 *
 * # Type traits
 *
 * **map_trait**:
 * **set_trait**:
 * **is_ordered_set**:
 *
 * # Functions
 *
 * Each function have specialization for ordered and unordered sets:
 *
 * set_union(SET const&, SET const&) -> SET;
 * set_intersection(SET const&, SET const&) -> SET;
 * set_difference(SET const&, SET const&) -> SET;
 * set_union_size(SET const&, SET const&) -> size_t;
 * set_intersection_size(SET const&, SET const&) -> size_t;
 * set_empty_intersection(SET const&, SET const&) -> size_t;
 * tanimoto() -> size_t;
 * tanomoto_distance() -> size_t;
 *
 */
#ifndef CJ_SET_HH_
#define CJ_SET_HH_

#include "cj/details/common.hh"

namespace cj {

  template<typename Container>
  struct map_trait;

  template<typename Key, typename Value>
  struct map_trait<flat_map<Key, Value>> {
    using set_type = flat_set<Key>;
  };

  template<typename Key, typename Value>
  struct map_trait<ordered_map<Key, Value>> {
    using set_type = ordered_set<Key>;
  };

  template<typename Key, typename Value>
  struct map_trait<unordered_map<Key, Value>> {
    using set_type = unordered_set<Key>;
  };

  template<typename T>
  struct is_ordered_set {
    const static bool value = false;
  };

  template<typename T>
  struct is_ordered_set<flat_set<T> {
    const static bool value = true;
  };

  template<typename T>
  struct is_ordered_set<ordered_set<T> {
    const static bool value = true;
  };

  /**
    \brief Returns the union of two flat_sets.
   */
  template<typename Set> // is_ordered
  auto union_size(Set const& xs, Set const& ys) -> size_t {
    size_t count = 0;
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
    \brief Returns the union of two unordered sets.
   */
  template<typename Set> // is unordered
  auto union_size(Set const& xs, Set const& ys) -> size_t {
    if (xs.size() < ys.size()) {
      return union_size(ys, xs);
    }
    size_t count = 0;
    for (auto const& y : ys) {
      count += (xs.find(y) == xs.end());
    }
    return xs.size() + count;
  }

  /**
    \brief Returns the union of two flat_sets.
   */
  template<typename T>
  auto set_union(boost::container::flat_set<T> const& xs,
                 boost::container::flat_set<T> const& ys)
                 noexcept -> boost::container::flat_set<T> {
    auto u = boost::container::flat_set<T>{};
    u.reserve(xs.size() + ys.size());
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
    while (xs_it != xs_end) u.emplace_hint(u.end(), *xs_it++);
    while (ys_it != ys_end) u.emplace_hint(u.end(), *ys_it++);
    return u;
  }

  /**
    \brief Returns the union of two sets.
   */
  template<typename T>
  auto set_union(std::set<T> const& xs,
                 std::set<T> const& ys)
                 noexcept -> std::set<T> {
    auto u = std::set<T>{};
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
    while (xs_it != xs_end) u.emplace_hint(u.end(), *xs_it++);
    while (ys_it != ys_end) u.emplace_hint(u.end(), *ys_it++);
    return u;
  }

  /**
    \brief Returns the union of two unordered sets.
   */
  template<typename T>
  auto set_union(std::unordered_set<T> const& xs,
                 std::unordered_set<T> const& ys)
                 noexcept -> std::unordered_set<T> {
    if (xs.size() < ys.size())
      return set_union(ys, xs);

    auto u = xs;
    u.insert(ys.begin(), ys.end());
    return u;
  }

  template<typename T>
  auto set_intersection_size(ordered_set<T> const& xs, ordered_set<T> const& ys) noexcept -> size_t {
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();
    size_t count = 0;

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

  template<typename T>
  auto set_intersection_size(std::set<T> const& xs, std::set<T> const& ys) noexcept -> size_t {
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();
    size_t count = 0;

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

  template<typename T>
  auto set_intersection_size(std::unordered_set<T> const& xs,
                             std::unordered_set<T> const& ys)
                             noexcept -> size_t {
    if (xs.size() < ys.size())
      return set_intersection_size(ys, xs);

    size_t count = 0;
    for (auto const& y : ys) count += (xs.find(y) != xs.end());
    return count;
  }
  template<typename Key>
  auto set_intersection(ordered_set<Key> const& xs, ordered_set<Key> const& ys)
                        noexcept -> ordered_set<Key> {
    auto inter = ordered_set<Key>{};
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

  template<typename T>
  auto set_intersection(std::unordered_set<T> const& xs,
                        std::unordered_set<T> const& ys)
                        noexcept -> std::unordered_set<T> {
    if (xs.size() < ys.size())
      return set_intersection(ys, xs);

    auto inter = std::unordered_set<T>{};

    for (auto const& y : ys) {
      if (xs.find(y) != xs.end())
        inter.insert(y);
    }

    return inter;
  }

  template<typename T>
  auto set_difference_size(boost::container::flat_set<T> const& xs,
                           boost::container::flat_set<T> const& ys)
                           noexcept -> size_t {
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();
    size_t count = xs.size();

    while (xs_it != xs_end && ys_it != ys_end) {
      bool const x_lt_y = *xs_it < *ys_it;
      bool const y_lt_x = *ys_it < *xs_it;
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
    // Insert remaining:
    return count;
  }

  template<typename T>
  auto set_difference_size(std::set<T> const& xs,
                           std::set<T> const& ys)
                           noexcept -> size_t {
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();
    size_t count = xs.size();

    while (xs_it != xs_end && ys_it != ys_end) {
      bool const x_lt_y = *xs_it < *ys_it;
      bool const y_lt_x = *ys_it < *xs_it;
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
    // Insert remaining:
    return count;
  }

  template<typename T>
  auto set_difference_size(std::unordered_set<T> const& xs,
                           std::unordered_set<T> const& ys)
                           noexcept -> size_t {
    size_t count = xs.size();
    for (auto const& y : ys) count -= (xs.find(y) != xs.end());
    return count;
  }


  template<typename T>
  auto set_difference(boost::container::flat_set<T> const& xs,
                      boost::container::flat_set<T> const& ys)
                      noexcept -> boost::container::flat_set<T> {
    auto diff = boost::container::flat_set<T>{};
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();

    while (xs_it != xs_end && ys_it != ys_end) {
      bool const x_lt_y = *xs_it < *ys_it;
      bool const y_lt_x = *ys_it < *xs_it;
      if (x_lt_y && !y_lt_x) {
        diff.emplace_hint(diff.end(), *xs_it++);
      } else if (!x_lt_y) {
        ++ys_it;
        if (!y_lt_x)
          ++xs_it;
      }
    }
    // Insert remaining:
    while (xs_it != xs_end) diff.emplace_hint(diff.end(), *xs_it++);
    return diff;
  }

  template<typename T>
  auto set_difference(std::set<T> const& xs,
                      std::set<T> const& ys)
                      noexcept -> std::set<T> {
    auto diff = std::set<T>{};
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();

    while (xs_it != xs_end && ys_it != ys_end) {
      bool const x_lt_y = *xs_it < *ys_it;
      bool const y_lt_x = *ys_it < *xs_it;
      if (x_lt_y && !y_lt_x) {
        diff.emplace_hint(diff.end(), *xs_it++);
      } else if (!x_lt_y) {
        ++ys_it;
        if (!y_lt_x)
          ++xs_it;
      }
    }
    // Insert remaining:
    while (xs_it != xs_end) diff.emplace_hint(diff.end(), *xs_it++);
    return diff;
  }

  template<typename T>
  auto set_difference(std::unordered_set<T> const& xs,
                      std::unordered_set<T> const& ys)
                      noexcept -> std::unordered_set<T> {
    auto diff = xs;
    for (auto const& y : ys) diff.erase(y);
    return diff;
  }

  template<typename T>
  auto empty_set_intersection(boost::container::flat_set<T> const& xs,
                              boost::container::flat_set<T> const& ys)
                              noexcept -> bool {
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();

    while (xs_it != xs_end && ys_it != ys_end) {
      if (*xs_it < *ys_it) {
        ++xs_it;
      } else {
        if (!(*ys_it < *xs_it)) return false;
        ++ys_it;
      }
    }
    return true;
  }

  template<typename T>
  auto empty_set_intersection(std::set<T> const& xs,
                              std::set<T> const& ys)
                              noexcept -> bool {
    auto inter = std::set<T>{};
    auto const xs_end = xs.end(), ys_end = ys.end();
    auto xs_it = xs.begin(), ys_it = ys.begin();

    while (xs_it != xs_end && ys_it != ys_end) {
      if (*xs_it < *ys_it) {
        ++xs_it;
      } else {
        if (!(*ys_it < *xs_it)) return false;
        ++ys_it;
      }
    }
    return true;
  }

  template<typename T>
  auto empty_set_intersection(std::unordered_set<T> const& xs,
                              std::unordered_set<T> const& ys)
                              noexcept -> bool {
    if (xs.size() < ys.size())
      return set_intersection(ys, xs);

    for (auto const& y : ys) {
      if (xs.find(y) != xs.end())
        return false;
    }

    return true;
  }

  /**
    \brief Tanimoto similarity on two vectors.
   */
  template<typename T, typename Real = double>
  auto tanimoto(vector<T> const& xs, vector<T> const& ys) -> Real {
    if (xs.empty() || ys.empty())
      return 0;

    size_t and_count = 0, or_count = 0;
    size_t const n = std::min(xs.size(), ys.size());
    for (auto i = 0u; i < n; ++i) {
      and_count += (xs[i] && ys[i]);
      or_count += (xs[i] || ys[i]);
    }
    return (Real)and_count / or_count;
  }

  /**
   * \brief Computes the Tanimoto similarity index between two sets (size of the
   *        intersection divided by the size of the union of the sets).
   */
  template<typename T, typename Real = double>
  auto tanimoto(ordered_set<T> const& xs, ordered_set<T> const& ys) noexcept -> Real {
    if (xs.empty() || ys.empty())
      return 0.0;
    size_t const i = set_intersection_size(xs, ys);
    return (Real)i / (xs.size() + ys.size() - i);
  }

  /**
   * \brief Computes the Tanimoto similarity index between two sets (size of the
   *        intersection divided by the size of the union of the sets).
   */
  template<typename T, typename Real = double>
  auto tanimoto(unordered_set<T> const& xs, unordered_set<T> const& ys) noexcept -> Real {
    if (xs.size() > ys.size())
      return tanimoto(ys, xs);
    if (xs.empty())
      return 0;

    size_t const i = std::accumulate(xs.begin(), xs.end(), 0, [=](T const& x, size_t count) {
      return count + (ys.find(x) != ys.end());
    });

    return (Real)i / (xs.size() + ys.size() - i);
  }

  template<typename T, typename Real = double>
  auto tanimoto_distance(vector<T> const& xs, vector<T> const& ys) noexcept -> Real {
    return Real(1) - tanimoto<T, Real>(xs, ys);
  }

  template<typename T, typename Real = double>
  auto tanimoto_distance(ordered_set<T> const& xs, ordered_set<T> const& ys) noexcept -> Real {
    return Real(1) - tanimoto<T, Real>(xs, ys);
  }

  template<typename T, typename Real = double>
  auto tanimoto_distance(unordered_set<T> const& xs, unordered_set<T> const& ys) noexcept -> Real {
    return Real(1) - tanimoto<T, Real>(xs, ys);
  }

} /* end namespace cj */

#endif
