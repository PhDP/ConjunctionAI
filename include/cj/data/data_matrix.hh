/**
 * \file   data_matrix.hh
 * \brief  A simple row-based class to store data as a matrix of a single type (without missing values).
 */
#ifndef CJ_DATA_DATA_MATRIX_HH_
#define CJ_DATA_DATA_MATRIX_HH_

#include <iostream>
#include <sstream>
#include <boost/lexical_cast.hpp>
#include "cj/common.hh"
#include "cj/utils/string.hh"
#include "cj/math/random.hh"

namespace cj {

  /**
   * \brief A data_matrix is a vector of rows, each also represented as vectors of data, along with a vector of names (the headers).
   */
  template<typename Value>
  class data_matrix {
   public:
    using value_type = Value;
    using row_type = vector<value_type>;
    using data_matrix_type = vector<row_type>;
    using const_iterator = typename data_matrix_type::const_iterator;

    /**
     * \brief Constructs an empty data_matrix with a set of headers.
     */
    data_matrix(vector<string> const& headers)
      : m_headers(headers) {
      unordered_map<string, size_t> m_str_to_idx;
    }

    /**
     * \brief Returns the number of columns.
     */
    auto ncols() const noexcept -> size_t {
      return m_headers.size();
    }

    /**
     * \brief Returns the number of rows.
     */
    auto nrows() const noexcept -> size_t {
      return m_dm.size();
    }

    /**
     * \brief Returns the total number of entries (number of columns * number of rows).
     */
    auto size() const noexcept -> size_t {
      return ncols() * nrows();
    }

    /**
     * \brief Whether the data_matrix has no entries (i.e. size() == 0).
     */
    auto empty() const noexcept -> bool {
      return m_dm.empty();
    }

    /**
     * \brief Add a row of data (if it is the right size). Returns whether the row could be added.
     */
    auto add_row(row_type const& new_row) noexcept -> bool {
      if (new_row.size() == m_headers.size()) {
        m_dm.push_back(new_row);
        return true;
      }
      return false;
    }

    /**
     * \brief Returns a reference to the data_matrix's headers.
     */
    auto headers() const noexcept -> vector<string> const& {
      return m_headers;
    }

    /**
     * \brief Returns a reference to the nth header.
     */
    auto header(size_t n) const noexcept -> string const& {
      return m_headers.at(n);
    }

    auto reserve(size_t n) -> void {
      m_dm.reserve(n);
    }

    /**
     * \brief Returns an empty vector if the name of the col is invalid.
     */
    auto extract_column(string const& col) const noexcept -> vector<value_type> {
      auto c = vector<value_type>{};
      auto const it = m_str_to_idx.find(col);
      if (it == m_str_to_idx.end()) {
        return c;
      }
      size_t const idx = *it;
      c.reserve(m_dm.size());
      for (auto const& row : m_dm) {
        c.push_back(row.at(idx));
      }
      return c;
    }

    /**
     * \brief Returns a reference to a row.
     */
    auto operator()(size_t row) const -> row_type const& {
      return m_dm.at(row);
    }

    /**
     * \brief Returns a value for a given row and column.
     */
    auto operator()(size_t row, size_t col) const -> value_type {
      return m_dm.at(row).at(col);
    }

    /**
     * \brief Returns a value for a given row and column using the header's name for the column.
     */
    auto operator()(size_t row, string const& col) const -> value_type {
      return m_dm.at(row).at(m_str_to_idx.at(col));
    }

    /**
     * \brief Split x% of the current data_matrix into a new data_matrix.
     */
    auto split_frame(double prop, std::mt19937_64 &rng) -> data_matrix<value_type> {
      auto new_df = data_matrix<value_type>(m_headers);
      size_t const n = prop * nrows();
      auto const indexes = unique_integers<size_t>(n, size_t(0), nrows(), rng);
      for (auto const i : indexes) {
        new_df.add_row(m_dm[i]);
      }
      for (auto it = indexes.rbegin(); it != indexes.rend(); ++it) {
        m_dm.erase(m_dm.begin() + *it);
      }
      return new_df;
    }

    /**
     * \brief Split x% of the current data_matrix into a new data_matrix.
     */
    auto split_frame(double prop, size_t seed) -> data_matrix<value_type> {
      auto rng = std::mt19937_64(seed);
      return split_frame(prop, rng);
    }

    /**
     * \brief Iterator to the beginning of the rows.
     */
    auto begin() const noexcept -> const_iterator {
      return m_dm.begin();
    }

    /**
     * \brief Iterator to the end of the rows.
     */
    auto end() const noexcept -> const_iterator {
      return m_dm.end();
    }

    /**
     * \brief Outputs the data_matrix with one row per line.
     */
    auto one_per_line(std::ostream& os, string const& sep = ",") const noexcept -> std::ostream& {
      os << intersperse(m_headers.begin(), m_headers.end(), sep) << '\n';
      for (auto const& row : m_dm) {
        os << intersperse(row.begin(), row.end(), sep) << '\n';
      }
      return os;
    }

   private:
    vector<string> m_headers;
    unordered_map<string, size_t> m_str_to_idx;
    data_matrix_type m_dm;
  };

  template<typename V>
  auto operator<<(std::ostream& os, data_matrix<V> const& df) -> std::ostream& {
    return df.one_per_line(os);
  }

  /**
   * \brief Constructs a data_matrix from a file.
   */
  template<typename Value>
  auto data_matrix_from_file(char const* filename, char delim = ',') -> std::optional<data_matrix<Value>> {
    auto content = read_file(filename);
    if (!content) {
      return std::nullopt;
    }
    auto lines = split(*content, '\n');
    auto const headers = split(lines[0], delim);
    auto const ncols = headers.size();
    if (ncols == 0) {
      return std::nullopt;
    }
    auto dm = data_matrix<Value>{headers};
    dm.reserve(lines.size() - 1);
    for (auto i = 1u; i < lines.size(); ++i) {
      auto new_row = typename data_matrix<Value>::row_type{};
      new_row.reserve(ncols);
      auto const columns = split(lines[i], delim);
      if (columns.size() != ncols) {
        return std::nullopt;
      }
      for (auto const& c : columns) {
        new_row.push_back(boost::lexical_cast<Value>(c));
      }
      dm.add_row(new_row);
    }
    return dm;
  }

} /* end namespace cj */

#endif
