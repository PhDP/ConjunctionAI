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
  auto fmt(std::ostream& os, clause<T, S> const& c, string const& sym = " <- ") -> std::ostream& {
    os
      << intersperse(c.begin_head(), c.end_head())
      << " <- "
      << intersperse(c.begin_body(), c.end_body());
    return os;
  }

  template<typename T, template<typename...> typename S>
  auto operator<<(std::ostream& os, clause<T, S> const& c) -> std::ostream& {
    return fmt(os, c);
  }

  /**
   * \brief Different kinds of clauses (disjunctive normal form, conjunctive normal form).
   */
  enum class clause_kind {
    dnf,
    cnf
  };

  /**
   * \brief A clausal knowledge base for hard and weighted (probabilistic) clauses.
   *
   * This object has both a set of hard clauses (infinite weight) and weighted clauses. Clauses
   * have to be unique, and thus functions will make sure that the same clause isn't found in both
   * sets.
   *
   * The clausal knowledge base only accept clauses with a positive weight (a clause with a weight
   * of zero has, well, no weight). Clauses with a weight being positive infinity will be moved
   * to hard clauses. Clauses with NAN will not be added. If a clause with a negative weight is
   * added, the clauses will be negated and the weight made positive assuming the equality
   * (clause, -weight) == (!clause, weight).
   */
  template<
    typename A,
    typename WeightType = double,
    template<typename...> typename ClauseSet = flat_set,
    template<typename...> typename Set = unordered_set,
    template<typename, typename...> typename Map = unordered_map>
  class clausal_kb {
   public:
    using clause_type = clause<A, ClauseSet>;
    using weight_type = WeightType;
    using hard_kb_type = Set<clause_type>;
    using prob_kb_type = Map<clause_type, weight_type>;

    /**
     * \brief Creates an empty clausal knowledge base.
     */
    clausal_kb() noexcept { }

    /**
     * \brief Returns true if the knowledge base has no clauses.
     */
    auto empty() const -> bool {
      return m_hard.empty() && m_prob.empty();
    }

    /**
     * \brief Returns the number of hard and weighted clauses.
     */
    auto size() const -> size_t {
      return m_hard.size() + m_prob.size();
    }

    /**
     * \brief Returns the number of hard clauses.
     */
    auto size_hard() const -> size_t {
      return m_hard.size();
    }

    /**
     * \brief Returns the number of weighted clauses.
     */
    auto size_prob() const -> size_t {
      return m_prob.size();
    }

    /**
     * \brief Returns the kind of the clauses.
     */
    auto kind() const -> clause_kind {
      return m_kind;
    }

    /**
     * \brief Checks for the presence of a given clause in either hard or weighted clauses.
     */
    auto has(clause_type const& c) const -> bool {
      return m_hard.find(c) != m_hard.end() || m_prob.find(c) != m_prob.end();
    }

    /**
     * \brief Checks whether a given hard clause is present.
     */
    auto has_hard(clause_type const& c) const -> bool {
      return m_hard.find(c) != m_hard.end();
    }

    /**
     * \brief Checks whether a given weighted clause is present.
     */
    auto has_prob(clause_type const& c) const -> bool {
      return m_prob.find(c) != m_prob.end();
    }

    /**
     * \brief Adds a hard clause, returns true if it could be added.
     */
    auto tell(clause_type const& c) -> bool;

    /**
     * \brief Adds a weighted clause, returns true if it could be added.
     */
    auto tell(clause_type const& c, weight_type weight) -> bool;

    /**
     * \brief Removes a clause (whether hard or weighted), returns true if it could be removed.
     */
    auto untell(clause_type const& c) -> bool {
      return untell_hard(c) || untell_prob(c);
    }

    /**
     * \brief Removes a hard clause, returns true if it could be removed.
     */
    auto untell_hard(clause_type const& c) -> bool;

    /**
     * \brief Removes a weighted clause, returns true if it could be removed.
     */
    auto untell_prob(clause_type const& c) -> bool;

    /**
     * \brief Update the weight of a clause. If the weight is positive infinity, move to hard clauses.
     */
    auto update(clause_type const& c, weight_type weight) -> bool;

    /**
     * \brief Moves a weighted clause to hard clauses, returns true if it could be moved.
     */
    auto to_hard(clause_type const& c) -> bool;

    /**
     * \brief Moves a hard clause to weighted clauses, returns true if it could be moved.
     */
    auto to_prob(clause_type const& c, weight_type weight) -> bool;

    /**
     * \brief Returns the weight of a given clause, with 0.0 standing for absent clauses and
     *        positive infinity for hard clauses.
     */
    auto get_weight(clause_type const& c) const -> weight_type;

    template<template<typename...> typename S>
    auto evaluate(S<A> const& db) const -> bool;

    template<typename Truth, template<typename, typename...> typename M>
    auto evaluate(M<A, Truth> const& db) const -> Truth;

   private:
    hard_kb_type m_hard;
    prob_kb_type m_prob;
    clause_kind m_kind;
  };

  template<typename A, typename W, template<typename...> typename C, template<typename...> typename S, template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::tell(clause_type const& c) -> bool {
    if (has(c)) {
      return false;
    }
    m_hard.insert(c);
    return true;
  }

  template<typename A, typename W, template<typename...> typename C, template<typename...> typename S, template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::tell(clause_type const& c, weight_type weight) -> bool {
    if (has(c)) {
      return false;
    }
    m_hard[c] = weight;
    return true;
  }

  template<typename A, typename W, template<typename...> typename C, template<typename...> typename S, template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::update(clause_type const& c, weight_type weight) -> bool {
    auto const it_prob = m_prob.find(c);
    if (it_prob == m_prob.end()) {
      return false;
    }
    if (weight <= W(0.0) || std::isnan(weight)) {
      return false;
    }
    if (weight == std::numeric_limits<W>::infinity()) {
      m_prob.erase(it_prob);
      m_hard.insert(c);
    } else {
      it_prob->second = weight;
    }
    return true;
  }

  template<typename A, typename W, template<typename...> typename C, template<typename...> typename S, template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::untell_hard(clause_type const& c) -> bool {
    auto const it_hard = m_hard.find(c);
    if (it_hard == m_hard.end()) {
      return false;
    }
    m_hard.erase(it_hard);
    return true;
  }

  template<typename A, typename W, template<typename...> typename C, template<typename...> typename S, template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::untell_prob(clause_type const& c) -> bool {
    auto const it_prob = m_prob.find(c);
    if (it_prob == m_prob.end()) {
      return false;
    }
    m_prob.erase(it_prob);
    return true;
  }

  template<typename A, typename W, template<typename...> typename C, template<typename...> typename S, template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::to_hard(clause_type const& c) -> bool {
    auto const it_prob = m_prob.find(c);
    if (it_prob == m_prob.end()) {
      return false;
    }
    m_prob.erase(it_prob);
    m_hard.insert(c);
    return true;
  }

  template<typename A, typename W, template<typename...> typename C, template<typename...> typename S, template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::to_prob(clause_type const& c, weight_type weight) -> bool {
    auto const it_hard = m_hard.find(c);
    if (it_hard == m_hard.end()) {
      return false;
    }
    m_hard.erase(it_hard);
    m_prob[c] = weight;
    return true;
  }

  template<typename A, typename W, template<typename...> typename C, template<typename...> typename S, template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::get_weight(clause_type const& c) const -> weight_type {
    auto const it_hard = m_hard.find(c);
    if (it_hard != m_hard.end()) {
      return std::numeric_limits<W>::infinity();
    }
    auto const it_prob = m_prob.find(c);
    return it_prob == m_prob.end()? W(0) : it_prob->second;
  }

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
