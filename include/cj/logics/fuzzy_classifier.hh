/**
 * # Summary
 *
 * Class for a simple fuzzy rule-based classifier.
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
#include "cj/math/fuzzy_partition.hh"
#include "cj/math/truth.hh"
#include "cj/stat/confusion.hh"
#include "cj/stat/statistics.hh"
#include "cj/data/data_matrix.hh"

namespace cj {

  /**
   * \brief Fuzzy rule-based classifier.
   */
  template<typename Truth, typename Input, typename Id = uint32_t>
  class fuzzy_classifier {
   public:
    class interpretation;

    using id_type = Id;
    using truth_type = Truth;
    using input_type = Input;
    using interpretation_type = std::shared_ptr<interpretation>;
    using interpretation_ptr = interpretation const*;
    using antecedent_type = flat_map<id_type, id_type>; // Maps an input ID to a fuzzyset ID.
    using rule_type = pair<antecedent_type, id_type>; // Pairs a rule with its output (for now: a binary value).
    using rules_type = flat_map<antecedent_type, id_type>; // Maps rules to output.
    using const_iterator = typename rules_type::const_iterator;
    using operator_type = std::function<truth_type(truth_type const&, truth_type const&)>;

    /**
     * \brief Builds a fuzzy knowledge base with a pointer to fuzzy_set and (optionally) a set of intitial rules.
     */
    fuzzy_classifier(interpretation_type const& fs, rules_type const& initial_rules = {});

    /**
     * \brief Whether the knowledge base has no rules.
     */
    auto empty() const -> bool { return m_rules.empty(); }

    /**
     * \brief Returns the number of rules in the knowledge base.
     */
    auto size() const -> size_t { return m_rules.size(); }

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
    auto has_rule(antecedent_type const& a, id_type c) const -> bool;

    /**
     * \brief Whether a given rule is present.
     */
    auto has_rule(rule_type const& r) const -> bool;

    /**
     * \brief Adds a rule (unless it is already there).
     */
    auto add_rule(antecedent_type const& a, id_type c) -> void;

    /**
     * \brief Adds a rule (unless it is already there).
     */
    auto add_rule(rule_type const& r) -> void;

    /**
     * \brief Removes a rule (unless it is already there).
     */
    auto rmv_rule(antecedent_type const& a, id_type c) -> void;

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
    auto evaluate(vector<input_type> const& row) -> size_t {
      auto truth_by_classes = vector<truth_type>(m_i->num_classes(), truth_trait<truth_type>::zero);
      for (auto const& rule : m_rules) {
        auto truth = truth_trait<truth_type>::unit;
        for (auto const& v : rule.first) {
          truth = truth && m_i->operator()(v.first, v.second, row.at(v.first));
        }
        truth_by_classes[rule.second] = truth_by_classes[rule.second] || truth;
      }
      return maximum_idx(truth_by_classes);
    }

    auto evaluate_all(data_matrix<input_type, id_type> const& dm) -> confusion<size_t, double> {
      auto results = confusion<size_t, double>{m_i->num_classes()};
      for (auto const& row : dm) {
        auto predicted = evaluate(row.first);
        results.add_count(predicted, row.second);
      }
      return results;
    }

    /**
     * \brief Reference to the fuzzy sets used to define this knowledge base.
     */
    auto get_interpretation() const -> interpretation_ptr { return m_i.get(); }

    auto begin() const -> const_iterator { return m_rules.begin(); }

    auto end() const -> const_iterator { return m_rules.end(); }

    auto operator==(fuzzy_classifier<Truth, Input, Id> const& other) const -> bool {
      if (m_rules.size() != other.size() || m_i != other.m_i) {
        return false;
      }
      //
      return true;
    }

    auto operator!=(fuzzy_classifier<Truth, Input, Id> const& other) const -> bool {
      return !(*this == other);
    }

    class interpretation {
     public:
      /**
       * \brief Builds an interpretation with a vector of categories.
       */
      interpretation(vector<string> classes);

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

      auto num_classes() const -> size_t;

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

   private:
    rules_type m_rules;
    interpretation_type m_i;
  };

  template<typename Truth, typename Input, typename Id>
  auto show_rule(std::ostream& os, typename fuzzy_classifier<Truth, Input, Id>::rule_type const& p, typename fuzzy_classifier<Truth, Input, Id>::interpretation_ptr f) -> void;

  template<typename Truth, typename Input, typename Id>
  auto operator<<(std::ostream& os, typename fuzzy_classifier<Truth, Input, Id>::interpretation const& i) -> std::ostream&;

  template<typename Truth, typename Input, typename Id>
  auto operator<<(std::ostream& os, fuzzy_classifier<Truth, Input, Id> const& k) -> std::ostream&;

} /* end namespace cj */

namespace std {

  template<typename Truth, typename Input, typename ID>
  struct hash<cj::fuzzy_classifier<Truth, Input, ID>> {
    auto operator()(cj::fuzzy_classifier<Truth, Input, ID> const& kb) const -> size_t {
      auto seed = size_t{51274857491742893LU};
      for (auto const& r : kb) {
        cj::std_hash_combine(seed, r);
      }
      return seed;
    }
  };

} /* end namespace std */

#endif
