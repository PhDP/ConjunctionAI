/**
 * Summary
 * =======
 * A class for a multidimentional confusion matrix. Note: the term 'category' is used
 * instead of 'class' because the latter??? is a reserved word in C++.
 */
#ifndef CJ_CONFUSION_HH_
#define CJ_CONFUSION_HH_

#include "cj/common.hh"

namespace cj {

  /**
   *
   *               Observed_0   Observed_1   Observed_2
   * Predicted_0   (0, 0)       (0, 1)       (0, 2)
   * Predicted_1   (1, 0)       (1, 1)       (1, 2)
   * Predicted_2   (2, 0)       (2, 1)       (2, 2)
   */
  template<typename Count, typename Float = double>
  class confusion {
   public:
    using count_type = Count;
    using float_type = Float;
    using matrix_type = Eigen::Matrix<count_type, Eigen::Dynamic, Eigen::Dynamic>;

    confusion(size_t dim) : m_c{matrix_type::Constant(dim, dim, count_type{0})}, m_dim{dim}, m_count{0} { }

    auto empty() const -> count_type { return m_count == 0; }

    auto count() const -> count_type { return m_count; }

    auto dim() const -> size_t { return m_dim; }

    auto add_count(size_t predicted, size_t observed, count_type add = count_type{1}) -> void;

    auto sub_count(size_t predicted, size_t observed, count_type sub = count_type{1}) -> void;

    auto true_positives(size_t c) const -> count_type { return m_c(c, c); };

    auto false_positives(size_t c) const -> count_type;

    auto true_negatives(size_t c) const -> count_type;

    auto false_negatives(size_t c) const -> count_type;

    auto accuracy() const -> float_type;

    auto accuracy(size_t c) const -> float_type;

    auto tss(size_t c) const -> float_type;

    auto operator()(size_t predicted, size_t observed) const -> count_type { return m_c(predicted, observed); }

   private:
    matrix_type m_c;
    size_t m_dim;
    count_type m_count;
  };

  template<typename Count, typename Float>
  auto confusion<Count, Float>::add_count(size_t actual, size_t expected, Count add) -> void {
    m_c(actual, expected) += add;
    m_count += add;
  }

  template<typename Count, typename Float>
  auto confusion<Count, Float>::sub_count(size_t actual, size_t expected, Count sub) -> void {
    if (m_c(actual, expected) < sub) {
      m_count -= m_c(actual, expected);
      m_c(actual, expected) = 0;
    } else {
      m_c(actual, expected) -= sub;
      m_count -= sub;
    }
  }

  template<typename Count, typename Float>
  auto confusion<Count, Float>::false_positives(size_t c) const -> Count {
    auto sum = Count{0};
    for (auto i = size_t{0}; i < m_dim; ++i) {
      sum += m_c(c, i);
    }
    return sum - m_c(c, c);
  }

  template<typename Count, typename Float>
  auto confusion<Count, Float>::false_negatives(size_t c) const -> Count {
    auto sum = Count{0};
    for (auto i = size_t{0}; i < m_dim; ++i) {
      sum += m_c(i, c);
    }
    return sum - m_c(c, c);
  }

  template<typename Count, typename Float>
  auto confusion<Count, Float>::true_negatives(size_t c) const -> Count {
    return m_count - (false_positives(c) + false_negatives(c) + true_positives(c));
  }

  template<typename Count, typename Float>
  auto confusion<Count, Float>::accuracy() const -> Float {
    auto diag = Count{0};
    for (auto i = size_t{0}; i < m_dim; ++i) {
      diag += m_c(i, i);
    }
    return ((Float)diag) / m_count;
  }

  template<typename Count, typename Float>
  auto confusion<Count, Float>::accuracy(size_t c) const -> Float {
    return (true_positives(c) + true_negatives(c)) / Float(m_count);
  }

  template<typename Count, typename Float>
  auto confusion<Count, Float>::tss(size_t c) const -> Float {
    auto const tp = true_positives(c);
    auto const tn = true_negatives(c);
    auto const fp = false_positives(c);
    auto const fn = false_negatives(c);
    return (Float(tp * tn) - Float(fp * fn)) / Float((tp + fn) * (fp + tn));
  }

} /* end namespace cj */

#endif
