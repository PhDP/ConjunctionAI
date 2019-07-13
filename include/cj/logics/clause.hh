/**
 * Summary
 * -------
 * Generic classes for clauses and clausal knowledge bases. Many algorithms in first-order logic
 * begin with converting the formulas into their disjunctive or conjunctive normal forms (DNF, CNF).
 *
 * Types
 * -------
 * **clause**
 * **clausal_kb**
 * **weighted_clausal_kb**
 * **clause_kind**
 *
 */
#ifndef CJ_CLAUSE_HH_
#define CJ_CLAUSE_HH_

#include "cj/details/common.hh"
#include "cj/utils/string.hh"

namespace cj {

  /**
   * \brief A clause as two sets of literals (positive & negative).
   *
   * By default, the class uses flat sets (sorted vectors) to store literals.
   * Use multisets if you want the same literal to be present more than once.
   *
   * \tparam A         The atoms of the sets.
   * \tparam SetType   The type of set used to store the literals.
   */
  template<
    typename A,
    template<typename...> typename SetType = flat_set>
  class clause {
   public:
    using literal_type = A;
    using literals_type = SetType<literal_type>;
    using iterator = typename literals_type::const_iterator;
    using const_iterator = typename literals_type::const_iterator;

    /**
     * \brief Builds a clause with initial sets of positive literals (the head) and negative literals (the body).
     */
    clause(literals_type const& head = literals_type{}, literals_type const& body = literals_type{})
      : m_head(head), m_body(body) {
    }

    /**
     * \brief Whether the clause is empty (has no literals).
     */
    auto empty() const noexcept -> bool {
      return m_body.empty() && m_head.size();
    }

    /**
     * \brief Total number of literals.
     */
    auto size() const noexcept -> size_t {
      return m_body.size() + m_head.size();
    }

    auto count(literal_type const& a) const noexcept -> size_t {
      return m_body.count(a) + m_head.count(a);
    }

    auto body_count(literal_type const& a) const noexcept -> size_t {
      return m_body.count(a);
    }

    auto head_count(literal_type const& a) const noexcept -> size_t {
      return m_head.count(a);
    }

    /**
     * \brief Number of literals in the head.
     */
    auto head_size() const noexcept -> size_t {
      return m_head.size();
    }

    /**
     * \brief Number of literals in the body.
     */
    auto body_size() const noexcept -> size_t {
      return m_body.size();
    }

    /**
     * \brief A fact has one positive literal and no literals in the body.
     */
    auto is_fact() const noexcept -> bool {
      return head_size() == 1 && body_size() == 0;
    }

    /**
     * \brief A rule has one positive literal and at least one negative literal.
     */
    auto is_rule() const noexcept -> bool {
      return head_size() == 1 && body_size > 0;
    }

    /**
     * \brief A query has no positive literals and at least one negative literal.
     */
    auto is_query() const noexcept -> bool {
      return head_size() == 0 && body_size > 0;
    }

    /**
     * \brief A horn clause has 0 or 1 positive literal.
     */
    auto is_horn() const noexcept -> bool {
      return head_size() < 2;
    }

    /**
     * \brief A definite clause has exactly one positive literal.
     */
    auto is_definite() const noexcept -> bool {
      return head_size() == 1;
    }

    /**
     * \brief Adds a positive literal to the clause.
     */
    auto add_to_head(literal_type const& l) noexcept -> void {
      m_head.insert(l);
    }

    /**
     * \brief Adds a negative literal to the clause.
     */
    auto add_to_body(literal_type const& l) noexcept -> void {
      m_body.insert(l);
    }

    /**
     * \brief Removes a positive literal.
     */
    auto rmv_from_head(literal_type const& l) noexcept -> void {
      m_head.erase(l);
    }

    /**
     * \brief Removes a negative literal.
     */
    auto rmv_from_body(literal_type const& l) noexcept -> void {
      m_body.erase(l);
    }

    /**
     * \brief Moves a literal from the head to the body.
     */
    auto move_head_to_body(literal_type const& l) noexcept -> bool {
      auto const it = m_head.find(l);
      if (it == m_head.end()) {
        return false;
      }
      m_head.erase(it);
      m_body.insert(l);
      return true;
    }

    /**
     * \brief Moves a literal from the body to the head.
     */
    auto move_body_to_head(literal_type const& l) noexcept -> bool {
      auto const it = m_body.find(l);
      if (it == m_body.end()) {
        return false;
      }
      m_body.erase(it);
      m_head.insert(l);
      return true;
    }

    /**
     * \brief Flips a literal from negative to positive or vice-verse. If the
     * literal is found in both the body and the head, or if it is found
     * in neither: do nothing and return false.
     */
    auto flip(literal_type const& l) noexcept -> bool {
      auto const it_body = m_body.find(l);
      auto const it_head = m_head.find(l);
      if ((it_body == m_body.end()) == (it_head == m_head.end())) {
        return false;
      }
      if (it_body != m_body.end()) {
        m_body.erase(it_body);
        m_head.insert(l);
      } else { // found_head is true
        m_head.erase(it_head);
        m_body.insert(l);
      }
      return true;
    }

    template<typename T_, template<typename...> class S_>
    auto operator==(clause<T_, S_> const& other) const noexcept -> bool {
      return m_head == other.m_head && m_body == other.m_body;
    }

    template<typename T_, template<typename...> class S_>
    auto operator!=(clause<T_, S_> const& other) const noexcept -> bool {
      return m_head != other.m_head || m_body != other.m_body;
    }

    template<typename T_, template<typename...> class S_>
    auto operator<(clause<T_, S_> const& other) const noexcept -> bool {
      if (m_head < other.m_head) {
        return true;
      }
      if (other.m_head < m_head) {
        return false;
      }
      return m_body < other.m_body;
    }

    auto head_begin() const noexcept -> const_iterator {
      return m_head.begin();
    }

    auto head_end() const noexcept -> const_iterator {
      return m_head.end();
    }

    auto body_begin() const noexcept -> const_iterator {
      return m_body.begin();
    }

    auto body_end() const noexcept -> const_iterator {
      return m_body.end();
    }

   private:
    literals_type m_head;
    literals_type m_body;
  };

  template<typename T, template<typename...> class S>
  auto operator<<(std::ostream& os, clause<T, S> const& c) -> std::ostream& {
    os
      << intersperse(c.head_begin(), c.head_end())
      << " := "
      << intersperse(c.body_begin(), c.body_end());
    return os;
  }

  enum class clause_kind {
    dnf,
    cnf
  };

  template<
    typename T,
    typename WeightType = double,
    template<typename...> typename SetType = flat_set,
    template<typename, typename...> typename MapType = flat_map>
  class clausal_kb {
   public:
    using clause_type = clause<T, SetType>;
    using weight_type = WeightType;
    using kb_type = MapType<clause_type, weight_type>;
    using const_iterator = typename kb_type::const_iterator;

    clausal_kb() noexcept;

    auto empty() const -> bool;

    auto size() const -> size_t;

    auto kind() const -> clause_kind { return m_kind; }

    auto tell(formula const& f, weight_type weight = std::numeric_limits<double>::infinity()) -> void;

    auto tell(clause_type const& f, weight_type weight = std::numeric_limits<double>::infinity()) -> void;

    auto show(std::ostream& os, symbols_map const& s) const -> std::ostream&;

    auto begin() const -> const_iterator;

    auto end() const -> const_iterator;

   private:
    kb_type m_kb;
    clause_kind m_kind;
  };

} /* end namespace cj */

namespace std {

  template<typename T, template<typename...> class S>
  struct hash<cj::clause<T, S>> {
    auto operator()(cj::clause<T, S> const& c) const -> size_t {
      size_t seed = 977368807307959276;
      cj::std_hash_range(seed, c.head_begin(), c.head_end());
      cj::std_hash_range(seed, c.body_begin(), c.body_end());
      cj::std_hash_combine(seed, c.kind());
      return seed;
    }
  };

} /* end namespace std */

#endif
