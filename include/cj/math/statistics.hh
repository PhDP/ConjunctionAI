/**
 * \file   statistics.hh
 * \brief  Computes the mean and other statistics.
 */
#ifndef CJ_MATH_STATISTICS_HH_
#define CJ_MATH_STATISTICS_HH_

#include "cj/common.hh"

namespace cj {

  template<typename It>
  auto maximum(It fst, It lst) -> It {
    auto m = fst++;
    for (; fst != lst; ++fst) {
      if (*m < *fst) {
        m = fst;
      }
    }
    return m;
  }

  template<typename It>
  auto minimum(It fst, It lst) -> It {
    auto m = fst++;
    for (; fst != lst; ++fst) {
      if (*fst < *m) {
        m = fst;
      }
    }
    return m;
  }

  template<typename Container>
  auto idx_of_maximum(Container const& c) -> size_t {
    auto idx = size_t{0};
    for (auto i = size_t{1}; i < c.size(); ++i) {
      if (c[idx] < c[i]) {
        idx = i;
      }
    }
    return idx;
  }

  template<typename Container>
  auto idx_of_minimum(Container const& c) -> size_t {
    auto idx = size_t{0};
    for (auto i = size_t{1}; i < c.size(); ++i) {
      if (c[i] < c[idx]) {
        idx = i;
      }
    }
    return idx;
  }

  /**
   * \brief Sums the size() of the containers within the iterator range.
   *
   * \param a     Iterator to the first element of the range.
   * \param b     As per C++ tradition: points to the element after the last.
   * \param acc   Accumulator (defaults to 0).
   * \return      Sum of the sizes of the containers within the [a, b) range.
   */
  template<typename It, typename Acc>
  auto sum_sizes(It a, It b, Acc acc = Acc(0)) -> Acc {
    for (; a != b; ++a) {
      acc += a->size();
    }
    return acc;
  }

  template<typename It, typename Acc>
  auto sum_key_sizes(It a, It b, Acc acc = Acc(0)) -> Acc {
    for (; a != b; ++a) {
      acc += a->first.size();
    }
    return acc;
  }

  template<typename It, typename Acc>
  auto sum_val_sizes(It a, It b, Acc acc = Acc(0)) -> Acc {
    for (; a != b; ++a) {
      acc += a->first.size();
    }
    return acc;
  }

  /**
   * \brief Computes the mean of a container of floats.
   */
  template<
    typename It,
    typename = std::enable_if_t<
      std::is_floating_point<typename std::iterator_traits<It>::value_type>::value>>
  auto mean(It a, It b) -> typename std::iterator_traits<It>::value_type {
    auto ans = typename std::iterator_traits<It>::value_type(0);
    auto count = 1u;
    for (; a != b; ++a) {
      ans += (*a - ans) / count++;
    }
    return ans;
  }

  /**
   * \brief Computes the mean of a container of integer, returns the result as a double.
   */
  template<
    typename It,
    typename = std::enable_if_t<
      std::is_integral<typename std::iterator_traits<It>::value_type>::value>>
  auto mean(It a, It b) -> double {
    auto ans = double(0);
    auto count = 1u;
    for (; a != b; ++a) {
      ans += (*a - ans) / count++;
    }
    return ans;
  }

  /**
   * \brief Computes the mean of a container of floats.
   */
  template<
    typename It,
    typename = std::enable_if_t<
      std::is_floating_point<typename std::iterator_traits<It>::value_type>::value>>
  auto fast_mean(It a, It b) -> typename std::iterator_traits<It>::value_type {
    auto const n = std::distance(a, b);
    return std::accumulate(a, b, typename std::iterator_traits<It>::value_type(0)) / n;
  }

  /**
   * \brief Computes the mean of a container of integer, returns the result as a double.
   */
  template<
    typename It,
    typename = std::enable_if_t<
      std::is_integral<typename std::iterator_traits<It>::value_type>::value>>
  auto fast_mean(It a, It b) -> double {
    auto const n = std::distance(a, b);
    return std::accumulate(a, b, double(0.0)) / n;
  }

} /* end namespace cj */

#endif
