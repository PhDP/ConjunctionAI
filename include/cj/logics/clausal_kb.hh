/**
 * \brief
 */
#ifndef CJ_CLAUSAL_KB_HH_
#define CJ_CLAUSAL_KB_HH_

#include "cj/common.hh"
#include "cj/utils/string.hh"
#include "cj/math/set.hh"
#include "cj/logics/clause.hh"

namespace cj {

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
   *
   * \tparam A          Atom type in the clauses.
   * \tparam Weight     Weight (normally double or float) for weighted clauses.
   * \tparam ClauseSet  Set used for the clauses' literals.
   * \tparam Set        Set used to store hard clauses.
   * \tparam Map        Map used to store weighted clauses.
   */
  template<
    typename A,
    typename Weight = double,
    template<typename...> typename ClauseSet = flat_set,
    template<typename...> typename Set = unordered_set,
    template<typename, typename...> typename Map = unordered_map>
  class clausal_kb {
   public:
    using clause_type = clause<A, ClauseSet>;
    using weight_type = Weight;
    using hard_kb_type = Set<clause_type>;
    using prob_kb_type = Map<clause_type, weight_type>;
    using hard_const_iterator = typename hard_kb_type::const_iterator;
    using prob_const_iterator = typename prob_kb_type::const_iterator;

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

    /**
     * \brief Returns the weight of a given clause, with 0.0 standing for absent clauses and
     *        positive infinity for hard clauses.
     */
    auto operator[](clause_type const& c) const -> weight_type {
      return get_weight(c);
    }

    /**
     * \brief Sum of the weighted of the violated clauses given a set of ground truth. Violating
     *        a single hard clause results in an infinite cost.
     */
    template<template<typename...> typename S>
    auto cost(S<A> const& db) const -> weight_type;

    template<template<typename...> typename S>
    auto evaluate_hard(S<A> const& db) const -> bool;

    template<typename Truth, template<typename, typename...> typename M>
    auto evaluate_hard(M<A, Truth> const& db) const -> Truth;

    auto begin_hard() const -> hard_const_iterator {
      return m_hard.begin();
    }

    auto end_hard() const -> hard_const_iterator {
      return m_hard.end();
    }

    auto begin_prob() const -> prob_const_iterator {
      return m_prob.begin();
    }

    auto end_prob() const -> prob_const_iterator {
      return m_prob.end();
    }

   private:
    hard_kb_type m_hard;
    prob_kb_type m_prob;
    clause_kind m_kind;
  };

  template<
    typename A, typename W, template<typename...> typename C, template<typename...> typename S,
    template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::tell(clause_type const& c) -> bool {
    if (has(c)) {
      return false;
    }
    m_hard.insert(c);
    return true;
  }

  template<
    typename A, typename W, template<typename...> typename C, template<typename...> typename S,
    template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::tell(clause_type const& c, weight_type weight) -> bool {
    if (has(c)) {
      return false;
    }
    m_hard[c] = weight;
    return true;
  }

  template<
    typename A, typename W, template<typename...> typename C, template<typename...> typename S,
    template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::update(clause_type const& c, weight_type weight) -> bool {
    auto const it_prob = m_prob.find(c);
    if (it_prob == m_prob.end()) {
      return false;
    }
    if (weight == W(0)) {
      m_prob.erase(it_prob);
      return true;
    }
    if (weight < W(0) || std::isnan(weight)) {
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

  template<
    typename A, typename W, template<typename...> typename C, template<typename...> typename S,
    template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::untell_hard(clause_type const& c) -> bool {
    auto const it_hard = m_hard.find(c);
    if (it_hard == m_hard.end()) {
      return false;
    }
    m_hard.erase(it_hard);
    return true;
  }

  template<
    typename A, typename W, template<typename...> typename C, template<typename...> typename S,
    template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::untell_prob(clause_type const& c) -> bool {
    auto const it_prob = m_prob.find(c);
    if (it_prob == m_prob.end()) {
      return false;
    }
    m_prob.erase(it_prob);
    return true;
  }

  template<
    typename A, typename W, template<typename...> typename C, template<typename...> typename S,
    template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::to_hard(clause_type const& c) -> bool {
    auto const it_prob = m_prob.find(c);
    if (it_prob == m_prob.end()) {
      return false;
    }
    m_prob.erase(it_prob);
    m_hard.insert(c);
    return true;
  }

  template<
    typename A, typename W, template<typename...> typename C, template<typename...> typename S,
    template<typename, typename...> typename M>
  auto clausal_kb<A, W, C, S, M>::to_prob(clause_type const& c, weight_type weight) -> bool {
    auto const it_hard = m_hard.find(c);
    if (it_hard == m_hard.end()) {
      return false;
    }
    m_hard.erase(it_hard);
    m_prob[c] = weight;
    return true;
  }

  template<
    typename A, typename W, template<typename...> typename C, template<typename...> typename S,
    template<typename, typename...> typename M>
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

  template<
    typename A, typename W, template<typename...> typename C, template<typename...> typename S,
    template<typename, typename...> typename M>
  struct hash<cj::clausal_kb<A, W, C, S, M>> {
    auto operator()(cj::clausal_kb<A, W, C, S, M> const& k) const -> size_t {
      auto seed = size_t(977368807307959276);
      cj::std_hash_range(seed, k.begin_hard(), k.end_hard());
      cj::std_hash_pair_range(seed, k.begin_prob(), k.end_prob());
      cj::std_hash_combine(seed, k.kind());
      return seed;
    }
  };

} /* end namespace std */

#endif
