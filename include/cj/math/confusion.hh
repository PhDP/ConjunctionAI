/**
 * \brief Multidimentional confusion matrix for testing classifiers.
 */
#ifndef CJ_CONFUSION_HH_
#define CJ_CONFUSION_HH_

#include "cj/common.hh"

namespace cj {

  /**
   * \brief A multidimentional confusion matrix.
   *
   *               Observed_0   Observed_1   Observed_2
   * Predicted_0   (0, 0)       (0, 1)       (0, 2)
   * Predicted_1   (1, 0)       (1, 1)       (1, 2)
   * Predicted_2   (2, 0)       (2, 1)       (2, 2)
   *
   * \tparam Count    Type used for counting {true, false} x {positives, negatives}.
   * \tparam Float    Type used for real numbers (used in accuracy, tss).
   */
  template<typename Count = size_t, typename Float = double>
  class confusion {
   public:
    using count_type = Count;
    using float_type = Float;
    using matrix_type = Eigen::Matrix<count_type, Eigen::Dynamic, Eigen::Dynamic>;

    /**
     * \brief Creates a confusion matrix with 'n' dimensions (number of classes).
     */
    confusion(size_t n) : m_c{matrix_type::Constant(n, n, count_type{0})}, m_dim{n}, m_count{0} { }

    /**
     * \brief Whether there are no counts in the confusion matrix.
     */
    auto empty() const -> count_type {
      return m_count == 0;
    }

    /**
     * \brief Sum of all the counts in the matrix. O(1).
     */
    auto count() const -> count_type {
      return m_count;
    }

    /**
     * \brief Sum of all the true positives (the diagional).
     */
    auto sum_diagonal() const -> count_type;

    /**
     * \brief Sum of the observed class (column).
     */
    auto sum_observed(size_t observed) const -> count_type;

    /**
     * \brief Sum of the predicted class (row).
     */
    auto sum_predicted(size_t predicted) const -> count_type;

    /**
     * \brief Dimension of the matrix (i.e.: the number of classes).
     */
    auto dim() const -> size_t {
      return m_dim;
    }

    /**
     * \brief Add a count for a given predicted/observed class.
     */
    auto add_count(size_t predicted, size_t observed, count_type add = count_type{1}) -> void;

    /**
     * \brief Substract a count for a given predicted/observed class. Will set to zero if the number
     * of counts is below the number to be substracted.
     */
    auto sub_count(size_t predicted, size_t observed, count_type sub = count_type{1}) -> void;

    /**
     * \brief Returns the number of true positives for class 'c'.
     */
    auto true_positives(size_t c) const -> count_type {
      return m_c(c, c);
    };

    /**
     * \brief Returns the number of false positives for class 'c'.
     */
    auto false_positives(size_t c) const -> count_type {
      return sum_predicted(c) - m_c(c, c);
    }

    /**
     * \brief Returns the number of true negatives for class 'c'.
     */
    auto true_negatives(size_t c) const -> count_type {
      return m_count - (false_positives(c) + false_negatives(c) + true_positives(c));
    }

    /**
     * \brief Returns the number of false negatives for class 'c'.
     */
    auto false_negatives(size_t c) const -> count_type {
      return sum_observed(c) - m_c(c, c);
    }

    /**
     * \brief Overall accuracy (number of correct predictions / total count).
     */
    auto accuracy() const -> float_type {
      return float_type(sum_diagonal()) / m_count;
    }

    /**
     * \brief Accuracy for a specific class.
     */
    auto accuracy(size_t c) const -> float_type {
      return (true_positives(c) + true_negatives(c)) / float_type(m_count);
    }

    /**
     * \brief True skill statistic for a specific class.
     */
    auto tss(size_t c) const -> float_type;

    /**
     * \brief Frequency of a particular class.
     */
    auto frequency(size_t predicted, size_t observed) const -> float_type {
      return float_type(m_c(predicted, observed)) / m_count;
    }

    /**
     * \brief Returns the count.
     */
    auto operator()(size_t predicted, size_t observed) const -> count_type {
      return m_c(predicted, observed);
    }

   private:
    matrix_type m_c;
    size_t m_dim;
    count_type m_count;
  };

  // Definitions:

  template<typename Count, typename Float>
  auto confusion<Count, Float>::sum_diagonal() const -> Count {
    auto sum = count_type{0};
    for (auto i = size_t{0}; i < m_dim; ++i) {
      sum += m_c(i, i);
    }
    return sum;
  }

  template<typename Count, typename Float>
  auto confusion<Count, Float>::sum_observed(size_t observed) const -> count_type {
    auto sum = count_type{0};
    for (auto i = size_t{0}; i < m_dim; ++i) {
      sum += m_c(i, observed);
    }
    return sum;
  }

  template<typename Count, typename Float>
  auto confusion<Count, Float>::sum_predicted(size_t predicted) const -> count_type {
    auto sum = count_type{0};
    for (auto i = size_t{0}; i < m_dim; ++i) {
      sum += m_c(predicted, i);
    }
    return sum;
  }

  template<typename Count, typename Float>
  auto confusion<Count, Float>::add_count(size_t predicted, size_t observed, count_type add) -> void {
    m_c(predicted, observed) += add;
    m_count += add;
  }

  template<typename Count, typename Float>
  auto confusion<Count, Float>::sub_count(size_t predicted, size_t observed, count_type sub) -> void {
    if (m_c(predicted, observed) < sub) {
      m_count -= m_c(predicted, observed);
      m_c(predicted, observed) = 0;
    } else {
      m_c(predicted, observed) -= sub;
      m_count -= sub;
    }
  }

  template<typename Count, typename Float>
  auto confusion<Count, Float>::tss(size_t c) const -> float_type {
    auto const tp = true_positives(c);
    auto const tn = true_negatives(c);
    auto const fp = false_positives(c);
    auto const fn = false_negatives(c);
    return (float_type(tp * tn) - float_type(fp * fn)) / float_type((tp + fn) * (fp + tn));
  }

} /* end namespace cj */

#endif
