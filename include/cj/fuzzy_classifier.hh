/**
 * \file   fuzzy_classifier.hh
 * \brief  A simple fuzzy knowledge base for classification.
 *
 * This header defines two classes: 'frbc' is a set of fuzzy rules, with labels and variables
 * being represented by integers. 'frbc_intepretation' provides an interpretation for 'frbc',
 * it gives names to the integers and define the fuzzy partitions. Each 'frbc' object is
 * associated with an 'interpretation'. The goal of this design is to allow the quick
 * copy and mutation of fuzzy classifiers for evolutionary algorithms, while maintaining
 * a single shared object for their interpretation.
 */
#ifndef CJ_FUZZY_CLASSIFIER_HH_
#define CJ_FUZZY_CLASSIFIER_HH_

#include "cj/common.hh"
#include "cj/fuzzy_partition.hh"
#include "cj/math/confusion.hh"
#include "cj/data/datamatrix.hh"
#include "cj/truth.hh"

namespace cj {

  /**
   * \brief Stores fuzzy sets associated with a vector of input variables.
   * \tparam Truth
   * \tparam Input
   * \tparam Signed or unsigned integer to identify the input/output variables.
   */
  template<typename Truth, typename Input = double, typename ID = uint32_t>
  class interpretation {
   public:
    using truth_type = Truth;
    using id_type = ID;
    using input_type = Input;
    using operator_type = std::function<truth_type(truth_type const&, truth_type const&)>;

    /**
     * \brief Builds an empty fuzzy sets.
     */
    interpretation(vector<string> categories, and_op, also_op);

    /**
     * \brief Adds an input variable with equally-sized triangular fuzzy sets.
     * \param name    Name of the input variable.
     * \param nsets   Number of fuzzy sets (triangles).
     * \param a       Input value where the fuzzy sets begin.
     * \param b       Input value where the fuzzy sets end.
     */
    auto add_triangular_sets(string const& name, size_t nsets, input_type a, input_type b) -> void;

    /**
     * \brief Whether the object is empty.
     */
    auto empty() const -> bool;

    /**
     * \brief Number of input variables.
     */
    auto size() const -> size_t;

    /**
     * \brief Number of fuzzy sets associated with input variable n.
     */
    auto size(id_type n) const -> size_t;

    /**
     * \brief Returns the name of the true or false consequent.
     */
    auto consequent(bool n) const -> string const&;

    /**
     * \brief Name of the nth input variable.
     */
    auto var_name(id_type n) const -> string const&;

    /**
     * \brief Name of the sets associated with the nth input variable.
     */
    auto sets_name(id_type n) const -> string const&; // WHAT IS THIS?

    /**
     * \brief Vector of labels for the fuzzy sets associated with the nth input variable.
     */
    auto label(id_type n) const -> vector<string> const&;

    /**
     * \brief Name of the label 's' of the fuzzy sets associated with the nth input variable.
     */
    auto label(id_type n, id_type s) const -> string const&;

    /**
     * \brief Returns a reference to the fuzzy sets associated with the nth input variable.
     */
    auto operator()(id_type n) const -> vector<std::function<double(double)>> const&;

    /**
     * \brief Returns a reference to the fuzzy sets 's' associated with the nth input variable.
     */
    auto operator()(id_type n, id_type s) const -> std::function<double(double)> const&;

    /**
     * \brief Call the fuzzy sets 's' associated with the nth input variable with value 'x'.
     */
    auto operator()(id_type n, id_type s, double x) const -> double;

   private:
    vector<string> m_ant_names; // Names of the variables (for the antecedants) given their id.
    vector<vector<std::function<truth_type(input_type)>>> m_sets; // Partition for each variable.
    vector<string> m_sets_names; // ????
    vector<vector<string>> m_sets_labels; // Name of the linguistic variables for each input variable.
    vector<string> m_cons; // Name of the categories for the consequants.
  };

  template<typename Truth, typename Input, typename ID>
  auto operator<<(std::ostream& os, interpretation<Truth, Input, ID> const& i) -> std::ostream&;

  /**
   * \brief Fuzzy rule-based classifier.
   */
  template<typename Truth, typename ID = uint32_t>
  class fuzzy_classifier {
   public:
    using id_type = ID;
    using truth_type = Truth;
    using interpretation_type = std::shared_ptr<interpretation<truth_type>>;
    using interpretation_ptr = interpretation<truth_type> const*;
    using antecedent_type = flat_map<id_type, id_type>; // Maps an input ID to a fuzzyset ID.
    using rule_type = pair<antecedent_type, id_type>; // Pairs a rule with its output (for now: a binary value).
    using rules_type = flat_map<antecedent_type, id_type>; // Maps rules to output.
    using const_iterator = rules_type::const_iterator;

    /**
     * \brief Builds a fuzzy knowledge base with a pointer to fuzzy_set and (optionally) a set of intitial rules.
     */
    fuzzy_classifier(interpretation_type const& fs, rules_type const& initial_rules = {});

    /**
     * \brief Whether the knowledge base has no rules.
     */
    auto empty() const -> bool;

    /**
     * \brief Returns the number of rules in the knowledge base.
     */
    auto size() const -> size_t;

    /**
     * \brief Complexity as the sum of the size of all antecedents (the number of input variables) + 1.
     */
    auto complexity() const -> size_t;

    /**
     * \brief Whether a rule with a given antecedent is present.
     */
    auto has_antecedent(antecedent_type const& m) const -> bool;

    /**
     * \brief Whether a given rule is present.
     */
    auto has_rule(antecedant_type const& a, id_type c) const -> bool;

    /**
     * \brief Whether a given rule is present.
     */
    auto has_rule(rule_type const& r) const -> bool;

    /**
     * \brief Adds a rule (unless it is already there).
     */
    auto add_rule(antecedant_type const& a, id_type c) -> void;

    /**
     * \brief Adds a rule (unless it is already there).
     */
    auto add_rule(rule_type const& r) -> void;

    /**
     * \brief Adds a rule (unless it is already there).
     */
    auto rmv_rule(antecedant_type const& a, id_type c) -> void;

    /**
     * \brief Removes a rule (if present).
     */
    auto rmv_rule(rule_type const& r) -> void;

    /**
     * \brief Returns a random rule.
     */
    auto get_random_rule(std::mt19937_64 &rng) const -> rule_type;

    /**
     * \brief Returns a random rule and remove it from the knowledge base.
     */
    auto pop_random_rule(std::mt19937_64& rng) -> rule_type;

    /**
     * \brief Evaluate the fuzzy classifier's effectiveness on a datamatrix.
     */
    auto evaluate(datamatrix<truth_type> const& dm) -> confusion<int64_t, double>;

    /**
     * \brief Reference to the fuzzy sets used to define this knowledge base.
     */
    auto interpretation() const -> interpretation_ptr;

    auto begin() const -> const_iterator;

    auto end() const -> const_iterator;

    auto operator==(fuzzy_classifier<Truth, Id> const& other) const -> bool;

    auto operator!=(fuzzy_classifier<Truth, Id> const& other) const -> bool;

   private:
    rules_type m_rules;
    interpretation_type m_fs;
  };

  template<typename Truth, typename ID>
  auto show_rule(std::ostream& os, typename fuzzy_classifier<Truth, ID>::rule_type const& p, typename fuzzy_classifier<Truth, ID>::interpretation_ptr f) -> void;

  template<typename Truth, typename ID>
  auto operator<<(std::ostream& os, fuzzy_classifier<Truth, ID> const& k) -> std::ostream&;

} /* end namespace cj */

namespace std {

  template<>
  struct hash<typename cj::fuzzy_classifier::rule_type> {
    auto operator()(typename cj::fuzzy_classifier::rule_type const& r) const -> size_t {
      auto seed = size_t{37193719738991LU};
      for (auto const& i : r.first) {
        cj::std_hash_combine(seed, i.first);
        cj::std_hash_combine(seed, i.second);
      }
      cj::std_hash_combine(seed, r.second);
      return seed;
    }
  };

  template<>
  struct hash<cj::fuzzy_classifier> {
    auto operator()(cj::fuzzy_kb const& kb) const -> size_t {
      auto seed = size_t{51274857491742893LU};
      for (auto const& r : kb) {
        cj::std_hash_combine(seed, r);
      }
      return seed;
    }
  };

} /* end namespace std */

#endif
