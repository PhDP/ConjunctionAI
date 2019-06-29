/**
 * \file   confusion.hh
 * \brief  Confusion matrix.
 */
#ifndef CJ_CONFUSION_HH_
#define CJ_CONFUSION_HH_

#include "cj/common.hh"

namespace cj {

  /**
   * \brief A confusion matrix.
   */
  template<
    typename Count = uint64_t,
    typename Real = double>
  struct confusion {
    using count_type = Count;
    using real_type = Real;

    count_type tp; // True positives
    count_type tn; // True negatives
    count_type fp; // False positives
    count_type fn; // False negatives

    /**
     * \brief Builds a confusion matrix with true positives, true negatives, false
     * positives, and false negatives (default: 0).
     */
    constexpr confusion(count_type true_positives = 0, count_type true_negatives = 0, count_type false_positives = 0, count_type false_negatives = 0)
      noexcept : tp(true_positives), tn(true_negatives), fp(false_positives), fn(false_negatives) {
    }

    /**
     * \brief Total number of observations.
     */
    constexpr auto size() const noexcept -> count_type {
      return tp + tn + fp + fn;
    }

    /**
     * \brief Positive observations (P = true positives + false negatives).
     */
    constexpr auto positives() const noexcept -> count_type {
      return tp + fn;
    }

    /**
     * \brief Negative observations (P = true negatives + false positives).
     */
    constexpr auto negatives() const noexcept -> count_type {
      return tn + fp;
    }

    /**
     * \brief Whether the table has no entries.
     */
    constexpr auto empty() const noexcept -> bool {
      return tp == count_type(0) && tn == count_type(0) && fp == count_type(0) && fn == count_type(0);
    }

    /**
     * \brief Correct predictions divided by the total number of predictions.
     */
    constexpr auto accuracy() const noexcept -> real_type {
      return (tp + tn) / real_type(size());
    }

    /**
     * \brief True positive rate (also called sensitivity, recall, hit rate).
     */
    constexpr auto tpr() const noexcept -> real_type {
      return tp / real_type(tp + fn);
    }

    /**
     * \brief True negative rate (also called specificity).
     */
    constexpr auto tnr() const noexcept -> real_type {
      return tn / real_type(tn + fp);
    }

    /**
     * \brief Positive predictive value (or precision).
     */
    constexpr auto ppv() const noexcept -> real_type {
      return tp / real_type(tp + fp);
    }

    /**
     * \brief Negative predictive value (NPV).
     */
    constexpr auto npv() const noexcept -> real_type {
      return tn / real_type(tn + fn);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Aliases

    /**
     * \brief Alias for the true positive rate.
     */
    constexpr auto sensitivity() const noexcept -> real_type {
      return tpr();
    }

    /**
     * \brief Alias for the true positive rate.
     */
    constexpr auto recall() const noexcept -> real_type {
      return tpr();
    }

    /**
     * \brief Alias for the true positive rate.
     */
    constexpr auto hit_rate() const noexcept -> real_type {
      return tpr();
    }

    /**
     * \brief Alias for the true negative rate.
     */
    constexpr auto specificity() const noexcept -> real_type {
      return tnr();
    }

    /**
     * \brief Alias for the positive predictive value.
     */
    constexpr auto precision() const noexcept -> real_type {
      return ppv();
    }

    ///////////////////////////////////////////////////////////////////////////
    // Complex metrics

    /**
     * \brief True skill statistic (or Hanssen-Kuiper skill score). Ranges from -1 to 1.
     */
    constexpr auto tss() const noexcept -> real_type {
      return (real_type(tp * tn) - real_type(fp * fn)) / real_type((tp + fn) * (fp + tn));
    }

    /**
     * \brief Youden's J statistics.
     */
    constexpr auto youdens_j() const noexcept -> real_type {
      return tp / real_type(tp + fn) + tn / real_type(tn + fp) - real_type(1);
    }

    /**
     * \brief The F1 score: the harmonic average of precision and recall.
     */
    constexpr auto f1_score() const noexcept -> real_type {
      return (tp << 1) / real_type((tp << 1) + fp + fn);
    }

    /**
     * \brief Matthews correlation coefficient.
     */
    constexpr auto matthews_corr_coef() const noexcept -> real_type {
      return (real_type(tp * tn) - real_type(fp * fn)) / std::sqrt(real_type((tp + fp) * (tp + fn) * (tn + fp) * (tn + fn)));
    }

    /**
     * \brief Size of the matrix times its log(size).
     */
    constexpr auto uncertainty_coef() const noexcept -> real_type {
      return size() * std::log(size());
    }

    /**
     * \brief Alias for uncertainty coefficient.
     */
    auto proficiency() const noexcept -> real_type {
      return uncertainty_coef();
    }

    // Alias for youdens_j (test).
    constexpr auto informedness() const noexcept -> real_type {
      return tpr() + tnr() - real_type(1);
    }

    constexpr auto operator==(confusion<count_type, real_type> const& other) const noexcept -> bool {
      return tp == other.tp && tn == other.tn && fp == other.fp && fn == other.fn;
    }

    constexpr auto operator!=(confusion<count_type, real_type> const& other) const noexcept -> bool {
      return tp != other.tp || tn != other.tn || fp != other.fp || fn != other.fn;
    }

    constexpr auto operator+(confusion<count_type, real_type> const& other) const noexcept -> confusion<count_type, real_type> {
      return confusion<count_type, real_type>{tp + other.tp, tn + other.tn, fp + other.fp, fn + other.fn};
    }

    constexpr auto operator+=(confusion<count_type, real_type> const& other) noexcept -> confusion<count_type, real_type>& {
      tp += other.tp;
      tn += other.tn;
      fp += other.fp;
      fn += other.fn;
      return *this;
    }
  };

  template<typename C, typename R>
  auto operator<<(std::ostream& os, confusion<C, R> const& c) noexcept -> std::ostream& {
    os
      << "True positives: " << c.tp << "; "
      << "True negatives: " << c.tn << "; "
      << "False positives: " << c.fp << "; "
      << "False negatives: " << c.fn << '.';
    return os;
  }

} /* end namespace cj */

#endif
