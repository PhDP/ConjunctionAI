/**
 * \brief
 */
#ifndef CJ_FUZZY_PARTITION_HH_
#define CJ_FUZZY_PARTITION_HH_

#include "cj/common.hh"

namespace cj {

  /**
   * \brief Builds a vector of generic labels for fuzzy sets (["is very low",
   * "is low", "is average", "is high", "is very high"] for n = 5).
   *
   * \param n   Number of labels to create. Will return an empty vector for n < 2.
   * \return    A vector of labels.
   */
  auto make_labels(size_t n) -> vector<string>;

  /**
   * \brief Builds a piecewise function that starts flat, has a straight slope, and continues flat.
   *
   * \param x_begin   Where the slope begins on the x axis.
   * \param x_end     Where the slope ends on the x axis.
   * \param y_before  y value from -infinity to x_begin.
   * \param y_after   y value from x_end to infinity.
   * \return          A function f: double -> double representing the slope.
   */
  auto make_slope(double x_begin, double x_end, double y_before = 0, double y_after = 1) -> std::function<double(double)>;

  /**
   * \brief Builds a piecewise function that starts flat, has two straight slopes to an apex (i.e.: a triangle), and continues flat.
   *
   * \param x_begin   Where the slope begins on the x axis.
   * \param x_apex    Where the of the triangle is on the x axis.
   * \param x_end     Where the slope ends on the x axis.
   * \param y_before  y value from -infinity to x_begin.
   * \param y_apex    y value at the top of the triangle.
   * \param y_after   y value from x_end to infinity.
   * \return          A function f: double -> double representing the triangles.
   */
  auto make_triangle(double x_begin, double x_apex, double x_end, double y_before = 0, double y_apex = 1, double y_after = 0) -> std::function<double(double)>;

  /**
   * \brief Creates 'n' equal-sized triangles from 'a' to 'b'.
   *
   * \param n        Number of triangles.
   * \param x_begin  Left bound (smallest value on the x-axis).
   * \param x_end    Right bound (largest value on the x-axis).
   * \param floor    Smallest truth value.
   * \param height   Highest truth value (peak of the triangles).
   * \return         Vector of triangles.
   */
  auto make_triangles(size_t n, double x_begin = 0, double x_end = 1, double floor = 0, double height = 1) noexcept -> vector<std::function<double(double)>>;

} /* end namespace cj */

#endif
