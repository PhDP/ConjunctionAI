/**
 * # Summary
 *
 * A simple row-based class for supervised learning. It stores data as a vector of rows (without
 * missing values), with each row being represented by a vector along with its output (of
 * potentially a different type).
 *
 */
#ifndef CJ_DATA_DATA_MATRIX_HH_
#define CJ_DATA_DATA_MATRIX_HH_

#include <iostream>
#include <sstream>
#include "cj/common.hh"
#include "cj/utils/string.hh"
#include "cj/math/random.hh"

namespace cj {

  /**
   * \brief A data_matrix is a vector of rows, each also represented as vectors of data, along with a vector of names (the headers).
   */
  template<typename Input, typename Output>
  class data_matrix {
   public:
    using input_type = Input;
    using output_type = Output;
    using row_type = pair<vector<input_type>, output_type>;
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
     * \brief Returns the number of columns (counting the output).
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
     * \brief Returns the number of rows.
     */
    auto size() const noexcept -> size_t {
      return m_dm.size();
    }

    /**
     * \brief Whether the data_matrix has no entries (i.e. size() == 0).
     */
    auto empty() const noexcept -> bool {
      return m_dm.empty();
    }

    /**
     * \brief Adds a row of data (if it is the right size). Returns whether the row could be added.
     */
    auto add_row(row_type const& new_row) noexcept -> bool;

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

    auto output_name() const noexcept -> string const& {
      return m_headers.back();
    }

    /**
     * \brief Reserves memory for a number of rows.
     */
    auto reserve(size_t n) -> void {
      m_dm.reserve(n);
    }

    /**
     * \brief Returns an empty vector if the name of the col is invalid.
     */
    auto extract_column(string const& col) const noexcept -> vector<input_type>;

    auto get_output(size_t row) const -> output_type {
      return m_dm.at(row).second;
    }

    /**
     * \brief Split x% of the current data_matrix into a new data_matrix.
     */
    auto split_frame(double prop, std::mt19937_64 &rng) -> data_matrix<input_type, output_type>;

    /**
     * \brief Split x% of the current data_matrix into a new data_matrix.
     */
    auto split_frame(double prop, size_t seed) -> data_matrix<input_type, output_type> {
      auto rng = std::mt19937_64(seed);
      return split_frame(prop, rng);
    }

    /**
     * \brief Outputs the data_matrix with one row per line.
     */
    auto one_per_line(std::ostream& os, string const& sep = ",") const noexcept -> std::ostream&;

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
     * \brief Returns a reference to a row.
     */
    auto operator()(size_t row) const -> row_type const& {
      return m_dm.at(row);
    }

    /**
     * \brief Returns a value for a given row and column.
     */
    auto operator()(size_t row, size_t col) const -> input_type {
      return m_dm.at(row).first.at(col);
    }

    /**
     * \brief Returns a value for a given row and column using the header's name for the column.
     */
    auto operator()(size_t row, string const& col) const -> input_type {
      return m_dm.at(row).first.at(m_str_to_idx.at(col));
    }

    static auto from_str(string const& txt, char delim = ',') -> std::optional<data_matrix<input_type, output_type>>;

    /**
     * \brief Constructs a data_matrix from a file.
     */
    static auto from_file(char const* filename, char delim = ',') -> std::optional<data_matrix<input_type, output_type>>;

   private:
    vector<string> m_headers;
    unordered_map<string, size_t> m_str_to_idx;
    data_matrix_type m_dm;
  };

  // Definitions:

  template<typename Input, typename Output>
  auto data_matrix<Input, Output>::add_row(typename data_matrix<Input, Output>::row_type const& new_row) noexcept -> bool {
    if (new_row.first.size() + 1 == m_headers.size()) {
      m_dm.push_back(new_row);
      return true;
    }
    return false;
  }

  template<typename Input, typename Output>
  auto data_matrix<Input, Output>::extract_column(string const& col) const noexcept -> vector<Input> {
    auto c = vector<Input>{};
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

  template<typename Input, typename Output>
  auto data_matrix<Input, Output>::split_frame(double prop, std::mt19937_64 &rng) -> data_matrix<Input, Output> {
    auto new_df = data_matrix<Input, Output>(m_headers);
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

  template<typename Input, typename Output>
  auto data_matrix<Input, Output>::one_per_line(std::ostream& os, string const& sep) const noexcept -> std::ostream& {
    os << intersperse(m_headers.begin(), m_headers.end(), sep) << '\n';
    for (auto const& row : m_dm) {
      os << intersperse(row.begin(), row.end(), sep) << '\n';
    }
    return os;
  }

  template<typename Input, typename Output>
  auto data_matrix<Input, Output>::from_str(string const& txt, char delim) -> std::optional<data_matrix<Input, Output>> {
    auto lines = split(txt, '\n');
    auto const headers = split(lines[0], delim);
    auto const ncols = headers.size();
    if (ncols == 0) {
      return std::nullopt;
    }
    auto dm = data_matrix<Input, Output>{headers};
    dm.reserve(lines.size() - 1);
    for (auto i = 1u; i < lines.size(); ++i) {
      auto new_row = typename data_matrix<Input, Output>::row_type{};
      new_row.first.reserve(ncols - 1);
      auto columns = split(lines[i], delim);
      new_row.second = boost::lexical_cast<Output>(columns.back());
      if (columns.size() != ncols) {
        return std::nullopt;
      }
      columns.pop_back();
      for (auto const& c : columns) {
        new_row.first.push_back(boost::lexical_cast<Input>(c));
      }
      dm.add_row(new_row);
    }
    return dm;
  }

  template<typename Input, typename Output>
  auto data_matrix<Input, Output>::from_file(char const* filename, char delim) -> std::optional<data_matrix<Input, Output>> {
    auto content = read_file(filename);
    return !content? std::nullopt : data_matrix<Input, Output>::from_str(*content, delim);
  }

  template<typename Input, typename Output>
  auto operator<<(std::ostream& os, data_matrix<Input, Output> const& df) -> std::ostream& {
    return df.one_per_line(os);
  }

} /* end namespace cj */

#endif
