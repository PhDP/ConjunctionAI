/**
 * \brief
 */
#ifndef CJ_CLAUSE_HH_
#define CJ_CLAUSE_HH_

#include "cj/common.hh"
#include "cj/utils/string.hh"
#include "cj/math/set.hh"

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
    template<typename...> typename Set = flat_set>
  class clause {
   public:
    using self_type = clause<A, Set>;
    using literal_type = A;
    using literals_type = Set<literal_type>;
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
      return m_body.empty() && m_head.empty();
    }

    /**
     * \brief Total number of literals.
     */
    auto size() const noexcept -> size_t {
      return m_body.size() + m_head.size();
    }

    /**
     * \brief Number of literals in the head.
     */
    auto size_head() const noexcept -> size_t {
      return m_head.size();
    }

    /**
     * \brief Number of literals in the body.
     */
    auto size_body() const noexcept -> size_t {
      return m_body.size();
    }

    /**
     * \brief How many times the atom 'a' is found in the clause.
     */
    auto count(literal_type const& a) const noexcept -> size_t {
      return m_body.count(a) + m_head.count(a);
    }

    /**
     * \brief How many times the literal is found in the head of the clause.
     */
    auto count_head(literal_type const& a) const noexcept -> size_t {
      return m_head.count(a);
    }

    /**
     * \brief How many times the literal is found in the body of the clause.
     */
    auto count_body(literal_type const& a) const noexcept -> size_t {
      return m_body.count(a);
    }

    /**
     * \brief Whether the literal can be found in either body or head.
     */
    auto has(literal_type const& a) const noexcept -> bool {
      return has_head(a) || has_body(a);
    }

    /**
     * \brief Whether the literal can be found in the head of the clause.
     */
    auto has_head(literal_type const& a) const noexcept -> bool {
      return m_head.find(a) != m_head.end();
    }

    /**
     * \brief How many times the literal is found in the body of the clause.
     */
    auto has_body(literal_type const& a) const noexcept -> bool {
      return m_body.find(a) != m_body.end();
    }

    /**
     * \brief A fact has one positive literal and no literals in the body.
     */
    auto is_fact() const noexcept -> bool {
      return size_head() == 1 && size_body() == 0;
    }

    /**
     * \brief A rule has one positive literal and at least one negative literal.
     */
    auto is_rule() const noexcept -> bool {
      return size_head() == 1 && size_body() > 0;
    }

    /**
     * \brief A query has no positive literals and at least one negative literal.
     */
    auto is_query() const noexcept -> bool {
      return size_head() == 0 && size_body() > 0;
    }

    /**
     * \brief A horn clause has 0 or 1 positive literal.
     */
    auto is_horn() const noexcept -> bool {
      return size_head() < 2;
    }

    /**
     * \brief A definite clause has exactly one positive literal.
     */
    auto is_definite() const noexcept -> bool {
      return size_head() == 1;
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
     * \brief Removes a positive literal. In the case of multisets: will only remove one literal.
     */
    auto rmv_from_head(literal_type const& l) noexcept -> void;

    /**
     * \brief Removes a negative literal. In the case of multisets: will only remove one literal.
     */
    auto rmv_from_body(literal_type const& l) noexcept -> void;

    /**
     * \brief Flips a literal from negative to positive or vice-verse. If the literal is found in
     *        both the body and the head, or if it is found in neither: do nothing and return false.
     */
    auto flip(literal_type const& l) noexcept -> bool;

    /**
     * \brief Checks if all elements of the head and body are the same.
     */
    auto operator==(self_type const& other) const noexcept -> bool {
      return m_head == other.m_head && m_body == other.m_body;
    }

    /**
     * \brief Checks if some elements of the head and body are different.
     */
    auto operator!=(self_type const& other) const noexcept -> bool {
      return m_head != other.m_head || m_body != other.m_body;
    }

    /**
     * \brief Applies < to the head and, if false and head == body, return the result of < on the body.
     */
    auto operator<(self_type const& other) const noexcept -> bool {
      return m_head < other.m_head? true : (m_head == other.m_head) && (m_body < other.m_body);
    }

    /**
     * \brief Iterator to the beginning of the head.
     */
    auto begin_head() const noexcept -> const_iterator {
      return m_head.begin();
    }

    /**
     * \brief Iterator to the end of the head.
     */
    auto end_head() const noexcept -> const_iterator {
      return m_head.end();
    }

    /**
     * \brief Iterator to the beginning of the body.
     */
    auto begin_body() const noexcept -> const_iterator {
      return m_body.begin();
    }

    /**
     * \brief Iterator to the end of the body.
     */
    auto end_body() const noexcept -> const_iterator {
      return m_body.end();
    }

   private:
    literals_type m_head;
    literals_type m_body;
  };

  template<typename A, template<typename...> typename S>
  auto clause<A, S>::rmv_from_head(literal_type const& l) noexcept -> void {
    auto const it = m_head.find(l);
    if (it != m_head.end()) {
      m_head.erase(it);
    }
  }

  template<typename A, template<typename...> typename S>
  auto clause<A, S>::rmv_from_body(literal_type const& l) noexcept -> void {
    auto const it = m_body.find(l);
    if (it != m_body.end()) {
      m_body.erase(it);
    }
  }

  template<typename A, template<typename...> typename S>
  auto clause<A, S>::flip(literal_type const& l) noexcept -> bool {
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

  template<typename T, template<typename...> typename S>
  auto show_clause(std::ostream& os, clause<T, S> const& c, string const& sym = " <- ") -> std::ostream& {
    os
      << intersperse(c.begin_head(), c.end_head())
      << " <- "
      << intersperse(c.begin_body(), c.end_body());
    return os;
  }

  template<typename T, template<typename...> typename S>
  auto operator<<(std::ostream& os, clause<T, S> const& c) -> std::ostream& {
    return show_clause(os, c);
  }

  /**
   * \brief Different kinds of clauses (disjunctive normal form, conjunctive normal form).
   */
  enum class clause_kind {
    dnf,
    cnf
  };

  template<
    typename A,
    typename WeightType = double,
    template<typename...> typename Set = flat_set,
    template<typename, typename...> typename Map = unordered_map>
  class clausal_kb {
   public:
    using clause_type = clause<A, Set>;
    using weight_type = WeightType;
    using prob_kb_type = Map<clause_type, weight_type>;
    using hard_kb_type = typename map_traits<prob_kb_type>::set_type;

    clausal_kb() noexcept { }

    auto empty() const -> bool {
      return m_hard.empty() && m_prob.empty();
    }

    auto size() const -> size_t {
      return m_hard.size() + m_prob.size();
    }

    auto kind() const -> clause_kind {
      return m_kind;
    }

//    auto tell(formula const& f, weight_type weight = std::numeric_limits<double>::infinity()) -> void;

    auto tell(clause_type const& c);

    auto tell(clause_type const& c, weight_type weight);

   private:
    hard_kb_type m_hard;
    prob_kb_type m_prob;
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
