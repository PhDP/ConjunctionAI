/**
 * \file   random.hh
 * \brief  Helper for random numbers.
 */
#ifndef CJ_MATH_RANDOM_HH_
#define CJ_MATH_RANDOM_HH_

#include "cj/common.hh"

namespace cj {

  /**
   * \brief Generates n unique integers within a range (not including 'end'). If the range is smaller
   * than n, return the entire range.
   */
  template<typename Integer, typename Rng, template<typename...> class SetType = flat_set>
  auto unique_integers(size_t n, Integer begin, Integer end, Rng& rng) noexcept -> SetType<Integer> {
    auto u = SetType<Integer>{};
    if (end < begin + 1) { // WRONGGGGGGGGGGGGGGGGGGGG!!!!!
      return u;
    }
    auto dist = std::uniform_int_distribution<Integer>(begin, end - 1);
    auto const range = end - begin;
    if (n > range) {
      n = range;
    }
    while (u.size() < n) {
      u.insert(dist(rng));
    }
    return u;
  }

  template<typename Container>
  auto pick_unique_pair(Container const& c, std::mt19937_64& rng) -> std::array<typename Container::value_type, 2> {
    auto idx = unique_integers(2, size_t{0}, c.size());
    return {get(c, get(idx, 0)), get(c, get(idx, 1))};
  }

} /* end namespace cj */

#endif
