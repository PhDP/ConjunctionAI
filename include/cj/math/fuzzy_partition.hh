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
  inline auto make_labels(size_t n) -> vector<string> {
    switch (n) {
    case 0:
    case 1:
      return vector<string>{};
    case 2:
      return vector<string>{"is low", "is high"};
    case 3:
      return vector<string>{"is low", "is average", "is high"};
    case 4:
      return vector<string>{"is very low", "is low", "is high", "is very high"};
    case 5:
      return vector<string>{"is very low", "is low", "is average", "is high", "is very high"};
    case 6:
      return vector<string>{"is very low", "is low", "is low-average", "is average-high", "is high", "is very high"};
    case 7:
      return vector<string>{"is very low", "is low", "is low-average", "is average", "is average-high", "is high", "is very high"};
    }
    auto ls = vector<string>{};
    auto const odd = n & 1;
    auto const half = n >> 1;
    for (auto i = 0u; i < half; ++i) {
      ls.push_back("is low" + boost::lexical_cast<string>(i));
    }
    if (odd) {
      ls.push_back("is average");
    }
    for (auto i = 0u; i < half; ++i) {
      ls.push_back("is high" + boost::lexical_cast<string>(i));
    }
    return ls;
  }

  /**
   * \brief Builds a piecewise function that starts flat, has a straight slope, and continues flat.
   *
   * \tparam Input    ...
   * \tparam Truth    ...
   * \param begin     Where the slope begins on the x axis.
   * \param end       Where the slope ends on the x axis.
   * \param before    Truth value for the input range (-infinity, begin).
   * \param after     Truth value for the input range (end, infinity).
   * \return          A function f: Input -> Truth representing the slope.
   */
  template<typename Input, typename Truth>
  auto make_slope(Input begin, Input end, Truth before, Truth after) -> std::function<Truth(Input)> {
    auto const length = end - begin;
    return [=](double x) {
      if (x < begin) {
        return before;
      }
      if (x < end) {
        return Truth{before.value * (1 - (x - begin) / length) + after.value * (1 - (end - x) / length)};
      }
      return after;
    };
  }

  /**
   * \brief Builds a piecewise function that starts flat, has two straight slopes to an apex (i.e.: a triangle), and continues flat.
   *
   * \param begin     Where the slope begins on the x axis.
   * \param apex      Where the of the triangle is on the x axis.
   * \param end       Where the slope ends on the x axis.
   * \param before    y value from -infinity to x_begin.
   * \param apex      y value at the top of the triangle.
   * \param after     y value from x_end to infinity.
   * \return          A function f: double -> double representing the triangles.
   */
  template<typename Input, typename Truth>
  auto make_triangle(Input begin, Input i_apex, Input end, Truth before, Truth t_apex, Truth after) -> std::function<Truth(Input)> {
    auto const ls_length = i_apex - begin;
    auto const rs_length = end - i_apex;
    return [=](Input x) {
      if (x < begin) {
        return before;
      }
      if (x < i_apex) {
        return Truth{before.value * (1 - (x - begin) / ls_length) + t_apex.value * (1 - (i_apex - x) / ls_length)};
      }
      if (x < end) {
        return Truth{t_apex.value * (1 - (x - i_apex) / rs_length) + after.value * (1 - (end - x) / rs_length)};
      }
      return after;
    };
  }

  /**
   * \brief Creates 'n' equal-sized triangles from 'a' to 'b'.
   *
   * \param n        Number of triangles.
   * \param begin    Left bound (smallest value on the x-axis).
   * \param end      Right bound (largest value on the x-axis).
   * \param floor    Smallest truth value.
   * \param ceil     Highest truth value (peak of the triangles).
   * \return         Vector of triangles.
   */
  template<typename Input, typename Truth>
  auto make_triangles(size_t n, Input begin, Input end, Truth floor, Truth ceil) noexcept -> vector<std::function<Truth(Input)>> {
    auto triangles = vector<std::function<Truth(Input)>>{};
    if (n < 2) {
      return triangles;
    }
    double const step = (end - begin) / (n - 1);
    triangles.push_back(make_slope(begin, begin + step, ceil, floor));
    for (auto i = 0u; i < n - 2; ++i) {
      triangles.push_back(make_triangle(begin + i * step, begin + (i + 1) * step, begin + (i + 2) * step, floor, ceil, floor));
    }
    triangles.push_back(make_slope(end - step, end, floor, ceil));
    return triangles;
  }

} /* end namespace cj */

#endif
