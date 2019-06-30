/**
 * \file   truth.hh
 * \brief  Implement three fuzzy logics: Lukasiwicz, Godel-Dummett, Product, along with truth_trait for bool and the three fuzzy logics.
 */
#ifndef CJ_TRUTH_HH_
#define CJ_TRUTH_HH_

#include <iostream>
#include "cj/details/common.hh"

namespace cj {

  /**
   * \brief Must implement !, &&, &, ||, |, along with implication, equivalence. It requires
   * three fields: static const T unit, static const T zero, static const size_t fuzziness.
   */
  template<typename T>
  struct truth_trait;

  template<>
  struct truth_trait<bool> {
    static const size_t fuzziness = 0;
    static const bool zero = false;
    static const bool unit = true;
  };

  inline auto implication(bool lhs, bool rhs) -> bool {
    return !lhs || rhs;
  }

  inline auto equivalence(bool lhs, bool rhs) -> bool {
    return lhs == rhs;
  }

  template<typename Float = double>
  struct lukasiewicz {
    Float value;
    explicit lukasiewicz(Float v) : value{v} {}
  };

  template<typename Float>
  struct truth_trait<lukasiewicz<Float>> {
    static const size_t fuzziness = 1;
    static const lukasiewicz<Float> zero = lukasiewicz<Float>{0};
    static const lukasiewicz<Float> unit = lukasiewicz<Float>{1};
  };

  template<typename F>
  inline auto operator!(lukasiewicz<F> l) -> lukasiewicz<F> {
    return lukasiewicz{F(1) - l.value};
  }

  template<typename F>
  inline auto operator&&(lukasiewicz<F> lhs, lukasiewicz<F> rhs) -> lukasiewicz<F> {
    return lukasiewicz{std::max(F(0), lhs.value + rhs.value - F(1))};
  }

  template<typename F>
  inline auto operator&(lukasiewicz<F> lhs, lukasiewicz<F> rhs) -> lukasiewicz<F> {
    return lukasiewicz{std::min(lhs.value, rhs.value)};
  }

  template<typename F>
  inline auto operator||(lukasiewicz<F> lhs, lukasiewicz<F> rhs) -> lukasiewicz<F> {
    return lukasiewicz{std::min(F(1), lhs.value + rhs.value)};
  }

  template<typename F>
  inline auto operator|(lukasiewicz<F> lhs, lukasiewicz<F> rhs) -> lukasiewicz<F> {
    return lukasiewicz{std::max(lhs.value, rhs.value)};
  }

  template<typename F>
  inline auto implication(lukasiewicz<F> lhs, lukasiewicz<F> rhs) -> lukasiewicz<F> {
    return lukasiewicz{std::min(F(1), F(1) - lhs.value + rhs.value)};
  }

  template<typename F>
  inline auto equivalence(lukasiewicz<F> lhs, lukasiewicz<F> rhs) -> lukasiewicz<F> {
    return lukasiewicz{F(1) - std::abs(lhs.value - rhs.value)};
  }

  template<typename F>
  inline auto operator==(lukasiewicz<F> lhs, lukasiewicz<F> rhs) -> bool {
    return lhs.value == rhs.value;
  }

  template<typename F>
  inline auto operator!=(lukasiewicz<F> lhs, lukasiewicz<F> rhs) -> bool {
    return lhs.value != rhs.value;
  }

  template<typename F>
  inline auto operator<(lukasiewicz<F> lhs, lukasiewicz<F> rhs) -> bool {
    return lhs.value < rhs.value;
  }

  template<typename F>
  auto operator<<(std::ostream& os, lukasiewicz<F> l) -> std::ostream& {
    os << l.value;
  }

  template<typename Float = double>
  struct godel {
    Float value;
    explicit godel(Float v) : value{v} {}
  };

  template<typename Float>
  struct truth_trait<godel<Float>> {
    static const size_t fuzziness = 1;
    static const godel<Float> zero = godel<Float>{0};
    static const godel<Float> unit = godel<Float>{1};
  };

  template<typename F>
  inline auto operator!(godel<F> l) -> godel<F> {
    return l.value == F(0)? godel{F(1)} : godel{F(0)};
  }

  template<typename F>
  inline auto operator&&(godel<F> lhs, godel<F> rhs) -> godel<F> {
    return godel{std::min(lhs.value, rhs.value)};
  }

  template<typename F>
  inline auto operator&(godel<F> lhs, godel<F> rhs) -> godel<F> {
    return godel{std::min(lhs.value, rhs.value)};
  }

  template<typename F>
  inline auto operator||(godel<F> lhs, godel<F> rhs) -> godel<F> {
    return godel{std::max(lhs.value, rhs.value)};
  }

  template<typename F>
  inline auto operator|(godel<F> lhs, godel<F> rhs) -> godel<F> {
    return godel{std::max(lhs.value, rhs.value)};
  }

  template<typename F>
  inline auto implication(godel<F> lhs, godel<F> rhs) -> godel<F> {
    return lhs.value > rhs.value? godel{rhs.value} : godel{F(1)};
  }

  template<typename F>
  inline auto equivalence(godel<F> lhs, godel<F> rhs) -> godel<F> {
    return implication(lhs, rhs) && implication(rhs, lhs);
  }

  template<typename F>
  inline auto operator==(godel<F> lhs, godel<F> rhs) -> bool {
    return lhs.value == rhs.value;
  }

  template<typename F>
  inline auto operator!=(godel<F> lhs, godel<F> rhs) -> bool {
    return lhs.value != rhs.value;
  }

  template<typename F>
  inline auto operator<(godel<F> lhs, godel<F> rhs) -> bool {
    return lhs.value < rhs.value;
  }

  template<typename F>
  auto operator<<(std::ostream& os, godel<F> l) -> std::ostream& {
    os << l.value;
  }

  template<typename Float = double>
  struct product {
    Float value;
    explicit product(Float v) : value{v} {}
  };

  template<typename Float>
  struct truth_trait<product<Float>> {
    static const size_t fuzziness = 1;
    static const product<Float> zero = product<Float>{0};
    static const product<Float> unit = product<Float>{1};
  };

  template<typename F>
  inline auto operator!(product<F> l) -> product<F> {
    return l.value == F(0)? product{F(1)} : product{F(0)};
  }

  template<typename F>
  inline auto operator&&(product<F> lhs, product<F> rhs) -> product<F> {
    return product{lhs.value * rhs.value};
  }

  template<typename F>
  inline auto operator&(product<F> lhs, product<F> rhs) -> product<F> {
    return product{std::min(lhs.value, rhs.value)};
  }

  template<typename F>
  inline auto operator||(product<F> lhs, product<F> rhs) -> product<F> {
    return product{lhs.value + rhs.value - lhs.value * rhs.value};
  }

  template<typename F>
  inline auto operator|(product<F> lhs, product<F> rhs) -> product<F> {
    return product{std::max(lhs.value, rhs.value)};
  }

  template<typename F>
  inline auto implication(product<F> lhs, product<F> rhs) -> product<F> {
    return lhs.value > rhs.value? product{rhs.value / lhs.value} : product{F(1)};
  }

  template<typename F>
  inline auto equivalence(product<F> lhs, product<F> rhs) -> product<F> {
    return implication(lhs, rhs) && implication(rhs, lhs);
  }

  template<typename F>
  inline auto operator==(product<F> lhs, product<F> rhs) -> bool {
    return lhs.value == rhs.value;
  }

  template<typename F>
  inline auto operator!=(product<F> lhs, product<F> rhs) -> bool {
    return lhs.value != rhs.value;
  }

  template<typename F>
  inline auto operator<(product<F> lhs, product<F> rhs) -> bool {
    return lhs.value < rhs.value;
  }

  template<typename F>
  auto operator<<(std::ostream& os, product<F> l) -> std::ostream& {
    os << l.value;
  }

} /* end namespace cj */

namespace std {

  template<typename Float>
  struct hash<cj::lukasiewicz<Float>> {
    auto operator()(cj::lukasiewicz<Float> const& f) const -> size_t {
      return hash<Float>{l.value};
    }
  };

  template<typename Float>
  struct hash<cj::godel<Float>> {
    auto operator()(cj::godel<Float> const& f) const -> size_t {
      return hash<Float>{l.value};
    }
  };

  template<typename Float>
  struct hash<cj::product<Float>> {
    auto operator()(cj::product<Float> const& f) const -> size_t {
      return hash<Float>{l.value};
    }
  };

} /* end namespace std */

#endif
