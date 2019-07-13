/**
 * \file   string.hh
 * \brief  Various useful string functions.
 */
#ifndef CJ_UTILS_STRING_HH_
#define CJ_UTILS_STRING_HH_

#include <iostream>
#include <sstream>
#include <fstream>
#include <cctype>
#include "cj/common.hh"

namespace cj {

  /**
   * \brief Split a string given a deliminator, into a vector of strings.
   */
  inline auto split(string const& s, char delim) -> vector<string> {
    auto ss = std::stringstream(s);
    auto tks = vector<string>{};
    auto item = string{};
    while (std::getline(ss, item, delim)) {
      tks.push_back(item);
    }
    return tks;
  }

  /**
   * \brief Reads an entire file into a single string.
   */
  inline auto read_file(char const* filename) -> std::optional<string> {
    auto in = std::ifstream(filename, std::ios::in | std::ios::binary);
    if (in) {
      auto contents = string{};
      in.seekg(0, std::ios::end);
      contents.resize((size_t)in.tellg());
      in.seekg(0, std::ios::beg);
      in.read(&contents[0], contents.size());
      in.close();
      return contents;
    }
    return std::nullopt;
  }

  /**
   * \brief Whether a string 's' starts with 'beginning'.
   */
  inline auto begins_with(string const& s, string const& beginning) -> bool {
    return s.compare(0, beginning.length(), beginning) == 0;
  }

  /**
   * \brief Creates a lowercase copy of a string.
   */
  inline auto to_lower_cpy(string const& s) -> string {
    auto cpy = string{s};
    for (auto i = 0u; i < cpy.size(); ++i) {
      cpy[i] = std::tolower(cpy[i]);
    }
    return cpy;
  }

  /**
   * \brief Builds a string from a container, interspersing a string between all elements.
   *
   * \param first          First element of the container.
   * \param last           Last element of the container.
   * \param inter          [Default = ", "] What to insert between the elements.
   * \param before_each    [Default = ""] What to add before each element.
   * \param after_each     [Default = ""] What to add after each element.
   * \return               A string with 'inter' interspersed between all elements of the container.
   */
  template<typename Iterator>
  inline auto intersperse(Iterator fst, Iterator lst, string const& inter = ", ",
                          string const& before_each = "", string const& after_each = "")
                          noexcept -> string {
    if (fst == lst) {
      return "";
    }
    auto oss = std::ostringstream{};
    oss << before_each << *fst << after_each;
    while (++fst != lst) {
      oss << inter << before_each << *fst << after_each;
    }
    return oss.str();
  }

  /**
   * \brief Builds a string from a container, interspersing a string between all elements and between
   * pairs.
   *
   * \param fst            First element of the container.
   * \param lst            Last element of the container.
   * \param inter          [Default = ", "] What to insert between the elements.
   * \param pair_inter     [Default = ", "] What to insert between the first and second element of the pairs.
   * \param before_each    [Default = "("] What to add before each element.
   * \param after_each     [Default = ")"] What to add after each element.
   * \return               A string with 'inter' interspersed between all elements of the container.
   */
  template<typename Iterator>
  inline auto intersperse_pairs(Iterator fst, Iterator lst,  string const& inter = ", ",
                                string const& pair_inter = ", ", string const& before_each = "(",
                                string const& after_each = ")")
                                noexcept -> string {
    if (fst == lst) {
      return "";
    }
    auto oss = std::ostringstream{};
    oss << before_each << fst->first << pair_inter << fst->second << after_each;
    while (++fst != lst) {
      oss << inter << before_each << fst->first << pair_inter << fst->second << after_each;
    }
    return oss.str();
  }

  /**
   * \brief Builds a string from the keys of an associative container, interspersing a string between
   * all elements.
   *
   * \param fst            First element of the container.
   * \param lst            Last element of the container.
   * \param inter          [Default = ", "] What to insert between the elements.
   * \param before_each    [Default = "("] What to add before each element.
   * \param after_each     [Default = ")"] What to add after each element.
   * \return               A string with 'inter' interspersed between all elements of the container.
   */
  template<typename Iterator>
  inline auto intersperse_keys(Iterator fst, Iterator lst, string const& inter = ", ",
                               string const& before_each = "", string const& after_each = "")
                               noexcept -> string {
    if (fst == lst) {
      return "";
    }
    auto oss = std::ostringstream{};
    oss << before_each << fst->first << after_each;
    while (++fst != lst) {
      oss << inter << before_each << fst->first << after_each;
    }
    return oss.str();
  }

} /* end namespace cj */

#endif
