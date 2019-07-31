/**
 * \file   common.hh
 * \brief  Headers needed in all headers.
 */
#ifndef CJ_COMMON_HH_
#define CJ_COMMON_HH_

#include <algorithm>
#include <functional>
#include <complex>
#include <utility>
#include <string>
#include <random>
#include <iterator>
#include <type_traits>
#include <array>
#include <vector>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <initializer_list>
#include <memory>
#include <cmath>
#include <variant>
#include <optional>
#include <boost/container/flat_set.hpp>
#include <boost/container/flat_map.hpp>
#include <boost/lexical_cast.hpp>
#include <Eigen/Dense>
#include <Eigen/Sparse>

#define CONJUNCTION
#define CONJUNCTION_MAX     0
#define CONJUNCTION_MIN     0
#define CONJUNCTION_REV     0
#define CONJUNCTION_VERSION "0.0.0"

#if defined(__GNUC__) // Clang also defines __GNUC__
# define cj_likely(x)       (__builtin_expect((x), 1))
# define cj_unlikely(x)     (__builtin_expect((x), 0))
#else
# define cj_likely(x)       (x)
# define cj_unlikely(x)     (x)
#endif

namespace cj {

  // Aliases:

  using string = std::string;

  template<typename T>
  using vector = std::vector<T>;

  template<typename Key0, typename Key1>
  using pair = std::pair<Key0, Key1>;

  template<typename Key, typename Compare = std::less<Key>>
  using flat_set = boost::container::flat_set<Key, Compare>;

  template<typename Key, typename Value, typename Compare = std::less<Key>>
  using flat_map = boost::container::flat_map<Key, Value, Compare>;

  template<typename Key, typename Compare = std::less<Key>>
  using flat_multiset = boost::container::flat_multiset<Key, Compare>;

  template<typename Key, typename Value, typename Compare = std::less<Key>>
  using flat_multimap = boost::container::flat_multimap<Key, Value, Compare>;

  template<typename Key, typename Compare = std::less<Key>>
  using ordered_set = std::set<Key, Compare>;

  template<typename Key, typename Value, typename Compare = std::less<Key>>
  using ordered_map = std::map<Key, Value, Compare>;

  template<typename Key, typename Compare = std::less<Key>>
  using ordered_multiset = std::multiset<Key, Compare>;

  template<typename Key, typename Value, typename Compare = std::less<Key>>
  using ordered_multimap = std::multimap<Key, Value, Compare>;

  template<typename Key>
  using unordered_set = std::unordered_set<Key>;

  template<typename Key, typename Value>
  using unordered_map = std::unordered_map<Key, Value>;

  template<typename Key>
  using unordered_multiset = std::unordered_multiset<Key>;

  template<typename Key, typename Value>
  using unordered_multimap = std::unordered_multimap<Key, Value>;

  // Eigen's vectors and matrices. Aliases for matrices and vectors based on the 's', 'c', 'd', 'z'
  // convention established by BLAS:

  template<typename T>
  using array = Eigen::Array<T, Eigen::Dynamic, 1>;

  template<typename T>
  using sparse_matrix = Eigen::SparseMatrix<T>;

  template<typename T>
  using matrix = Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>;

  template<typename T>
  using colvec = Eigen::Matrix<T, Eigen::Dynamic, 1>;

  template<typename T>
  using rowvec = Eigen::Matrix<T, 1, Eigen::Dynamic>;

  // Aliases for matrices/vectors based on BLAS' convention: 's' = float, 'd' = double,
  // 'c' = complex<float>, 'z': complex<double>.

  using smatrix = matrix<float>;
  using scolvec = colvec<float>;
  using srowvec = rowvec<float>;

  using dmatrix = matrix<double>;
  using dcolvec = colvec<double>;
  using drowvec = rowvec<double>;

  using cmatrix = matrix<std::complex<float>>;
  using ccolvec = colvec<std::complex<float>>;
  using crowvec = rowvec<std::complex<float>>;

  using zmatrix = matrix<std::complex<double>>;
  using zcolvec = colvec<std::complex<double>>;
  using zrowvec = rowvec<std::complex<double>>;

  // Helpers to build eigen objects:

  /**
   * \brief Builds an array with an initializer list.
   */
  template<typename T>
  constexpr auto make_array(std::initializer_list<T> const& xs) noexcept -> array<T> {
    auto a = array<T>(xs.size());
    auto i = 0u;
    for (auto const& x : xs) {
      a(i++) = x;
    }
    return a;
  }

  /**
   * \brief Builds a column vector with an initializer list.
   */
  template<typename T>
  constexpr auto make_colvec(std::initializer_list<T> const& xs) noexcept -> colvec<T> {
    auto v = colvec<T>(xs.size());
    auto i = 0u;
    for (auto const& x : xs) {
      v(i++) = x;
    }
    return v;
  }

  /**
   * \brief Builds a row vector with an initializer list.
   */
  template<typename T>
  constexpr auto make_rowvec(std::initializer_list<T> const& xs) noexcept -> rowvec<T> {
    auto v = rowvec<T>(xs.size());
    auto i = 0u;
    for (auto const& x : xs) {
      v(i++) = x;
    }
    return v;
  }

  /**
   * \brief Builds a matrix with an initializer list.
   */
  template<typename T>
  constexpr auto make_matrix(std::initializer_list<std::initializer_list<T>> const& xs) noexcept -> matrix<T> {
    auto const nrows = xs.size();
    if (nrows == 0) {
      return matrix<T>(0, 0);
    }
    auto const ncols = xs.begin()->size();
    auto m = matrix<T>(nrows, ncols);
    auto r = 0u;
    for (auto const& row : xs) {
      auto c = 0u;
      for (auto const& value : row) {
        m(r, c++) = value;
      }
      ++r;
    }
    return m;
  }

  // Math constants:

  /**
   * \brief Natural logarithm of 2.
   */
  template<typename T>
  constexpr T ln2 = T(0.6931471805599453094172321214581765680755L);

  /**
   * \brief Square root of 2.
   */
  template<typename T>
  constexpr T sqrt2 = T(1.4142135623730950488016887242096980785696L);

  /**
   * \brief Pi. You may have heard about it.
   */
  template<typename T>
  constexpr T pi = T(3.1415926535897932384626433832795028841971L);

  /**
   * \brief Euler's number.
   */
  template<typename T>
  constexpr T euler = T(2.7182818284590452353602874713526624977572L);

  /**
   * \brief The golden ratio.
   */
  template<typename T>
  constexpr T golden = T(1.6180339887498948482045868343656381177203L);

  // Helpers:

  /**
   * \brief Get nth element using std::advance.
   */
  template<typename Container>
  inline auto get(Container const& c, size_t n) -> typename Container::value_type const& {
    auto it = c.begin();
    std::advance(it, n);
    return *it;
  }

  /**
   * \brief Whether a container contains a given element.
   */
  template<typename Container>
  inline auto contains(Container const& c, typename Container::value_type const& elem) -> bool {
    return c.find(elem) != c.end();
  }

  /**
   * \brief Checks if left <= x <= right.
   */
  template<typename T>
  constexpr auto within_eq(T left, T x, T right) noexcept -> bool {
    return left <= x && x <= right;
  }

  /**
   * \brief Checks if left < x < right.
   */
  template<typename T>
  constexpr auto within(T left, T x, T right) noexcept -> bool {
    return left < x && x < right;
  }

  /**
   * \brief Hashes T and combine with a seed. A version of boost's hash_combine using std::hash.
   */
  template<typename T>
  constexpr auto std_hash_combine(size_t& seed, T const& t) noexcept -> void {
    seed ^= std::hash<T>{}(t) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
  }

  /**
   * \brief Hashes a range and combine with a seed. A version of boost's hash_range using std::hash.
   */
  template<typename It>
  constexpr auto std_hash_range(size_t& seed, It fst, It lst) noexcept -> void {
    for (; fst != lst; ++fst) {
      using value_type = typename std::iterator_traits<It>::value_type;
      seed ^= std::hash<value_type>{}(*fst) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
    }
  }

  template<typename It>
  constexpr auto std_hash_pair_range(size_t& seed, It fst, It lst) noexcept -> void {
    for (; fst != lst; ++fst) {
      std_hash_range(seed, fst->first);
      std_hash_range(seed, fst->second);
    }
  }

} /* end namespace cj */

#endif
