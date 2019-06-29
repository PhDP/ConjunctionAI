/**
 * \file   information.hh
 * \brief  Core functions for information theory.
 */
#ifndef CJ_MATH_INFORMATION_HH_
#define CJ_MATH_INFORMATION_HH_

#include "cj/common.hh"

namespace cj {

  /**
   * \brief Checks if the values in the vector sum to 1 and all
   * individual probabilities $p$ obey the rule $0 \leq p \leq 1$.
   */
  template<typename T>
  auto probabilities(colvec<T> const& ps, T max_error = T(0.001)) -> bool {
    auto sum = T{0};
    for (auto i = size_t{0}; i < ps.size(); ++i) {
      if (ps(i) < T{0} || ps(i) > T{1}) {
        return false;
      }
      sum += ps(i);
    }
    return within_eq(T(1.0) - max_error, sum, T(1.0) + max_error);
  }

  /**
   * \brief Checks wether the values within a matrix sums to 1.0.
   */
  template<typename T>
  auto joint_probabilities(matrix<T> const& pxy, T max_error = T(0.001)) -> bool {
    auto sum = T{0};
    for (auto i = size_t{0}; i < pxy.size(); ++i) {
      if (pxy(i) < T{0} || pxy(i) > T{1}) {
        return false;
      }
      sum += pxy(i);
    }
    return within_eq(T(1.0) - max_error, sum, T(1.0) + max_error);
  }

  /**
   * \brief Converts nats to bits (all functions in manti works in bits).
   */
  template<typename T>
  constexpr auto nats_to_bits(T nats) -> T {
    return nats / ln2<T>;
  }

  /**
   * \brief Converts bits to nats (all functions in manti works in bits).
   */
  template<typename T>
  constexpr auto bits_to_nats(T bits) -> T {
    return bits * ln2<T>;
  }

  /**
   * \brief Returns the index of the highest value.
   */
  template<typename T>
  auto mode_idx(colvec<T> const& px) -> size_t {
    size_t idx = 0;
    for (auto i = 1u; i < px.size(); ++i) {
      if (px[i] > px[idx]) {
        idx = i;
      }
    }
    return idx;
  }

  /**
   * \brief Entropy of a vector.
   */
  template<typename T>
  auto entropy(colvec<T> const& px) -> T {
    T h = 0.0;
    for (auto const x : px) {
      if (x != T(0.0)) {
        h -= x * std::log2(x);
      }
    }
    return h;
  }

  template<typename T>
  auto joint_entropy(matrix<T> const& pxy) -> T {
    T h = 0.0;
    for (auto const x : pxy) {
      if (x != T(0.0)) {
        h -= x * std::log2(x);
      }
    }
    return h;
  }

  template<typename T>
  auto relative_entropy(colvec<T> const& xs, colvec<T> const& ys) -> T {
    T kl = 0.0;
    auto const n = std::min(xs.rows(), ys.rows());
    for (auto i = 0u; i < n; ++i) {
      if (xs[i] != T(0.0)) {
        kl += xs[i] * std::log2(xs[i] / ys[i]);
      }
    }
    return kl;
  }

  template<typename T>
  auto cross_entropy(colvec<T> const& xs, colvec<T> const& ys) -> T {
    T h = 0.0;
    auto const n = std::min(xs.rows(), ys.rows());
    for (auto i = 0u; i < n; ++i) {
      if (ys[i] != T(0.0)) {
        h -= xs[i] * std::log2(ys[i]);
      }
    }
    return h;
  }

  template<typename T>
  auto mutual_information(matrix<T> const& pxy, colvec<T> const& px, colvec<T> const& py) -> T {
    T i = 0.0;
    for (auto x = 0u; x < px.n_elem; ++x) {
      for (auto y = 0u; y < py.n_elem; ++y) {
        if (pxy(x, y) != T(0.0)) {
          i += pxy(x, y) * std::log2(pxy(x, y) / (px[x] * py[y]));
        }
      }
    }
    return i;
  }

} /* end namespace cj */

#endif
