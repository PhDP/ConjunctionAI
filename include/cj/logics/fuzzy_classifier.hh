/**
 * # Summary
 *
 * Class for a fuzzy rule-based classifier.
 */
#ifndef CJ_FUZZY_CLASSIFIER_HH_
#define CJ_FUZZY_CLASSIFIER_HH_

#include "cj/common.hh"
#include "cj/math/fuzzy_partition.hh"
#include "cj/math/truth.hh"
#include "cj/math/confusion.hh"
#include "cj/math/statistics.hh"
#include "cj/data/data_matrix.hh"

namespace cj {

  /**
   * \brief Fuzzy rule-based classifier: predict a category given a set of inputs using a set
   * of fuzzy rules of the form 'IF antecedents THEN category'.
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
    fuzzy_classifier(interpretation_type const& i, rules_type const& initial_rules = {})
      : m_i(i), m_rules{initial_rules} {
    }

    /**
     * \brief Whether the knowledge base has no rules.
     */
    auto empty() const -> bool {
      return m_rules.empty();
    }

    /**
     * \brief Returns the number of rules in the knowledge base.
     */
    auto size() const -> size_t {
      return m_rules.size();
    }

    /**
     * \brief Complexity as the sum of the size of all rule (number of input variables in the
     * antecedents + 1).
     */
    auto complexity() const -> size_t {
      return sum_key_sizes(m_rules.begin(), m_rules.end(), m_rules.size());
    }

    /**
     * \brief Whether a rule with a given antecedent is present.
     */
    auto has_antecedent(antecedent_type const& a) const -> bool {
      return m_rules.find(a) != m_rules.end();
    }

    /**
     * \brief Whether a given rule is present.
     */
    auto has_rule(antecedent_type const& a, id_type c) const -> bool {
      auto it = m_rules.find(a);
      return it == m_rules.end()? false : it->second == c;
    }

    /**
     * \brief Whether a given rule is present.
     */
    auto has_rule(rule_type const& r) const -> bool {
      return has_rule(r.first, r.second);
    }

    /**
     * \brief Adds a rule (unless it is already there).
     */
    auto add_rule(antecedent_type const& a, id_type c) -> void {
      m_rules[a] = c;
    }

    /**
     * \brief Adds a rule (unless it is already there).
     */
    auto add_rule(rule_type const& r) -> void {
      m_rules.insert(r);
    }

    /**
     * \brief Removes a rule (unless it is already there).
     */
    auto rmv_rule(antecedent_type const& a) -> void {
      m_rules.erase(a);
    }

    /**
     * \brief Removes a rule (if present).
     */
    auto rmv_rule(rule_type const& r) -> void {
      m_rules.erase(r.first);
    }

    /**
     * \brief Returns a random rule.
     */
    auto get_random_rule(std::mt19937_64& rng) const -> rule_type;

    /**
     * \brief Returns a random rule and remove it from the knowledge base.
     */
    auto pop_random_rule(std::mt19937_64& rng) -> rule_type;

    /**
     * \brief Reference to the fuzzy sets used to define this knowledge base.
     */
    auto get_interpretation() const -> interpretation_type {
      return m_i;
    }

    /**
     * \brief Reference to the fuzzy sets used to define this knowledge base.
     */
    auto get_interpretation_ptr() const -> interpretation_type {
      return m_i.get();
    }

    /**
     * \brief Generates a prediction (category ID) given a set of input values.
     */
    auto evaluate(vector<input_type> const& row) -> id_type;

    /**
     * \brief Returns the confusion matrix for a database of (input, category) pairs.
     */
    auto evaluate_all(data_matrix<input_type, id_type> const& dm) -> confusion<size_t, double>;

//    auto evolve(data_matrix<input_type, id_type> const& training, std::function<void(std::mt19937_64&)> const& mutate, std::mt19937_64& rng) -> void;

    static auto make_interpretation(vector<string> categories) -> interpretation_type  {
      return std::make_shared<interpretation>(interpretation{std::move(categories)});
    }

    auto begin() const -> const_iterator {
      return m_rules.begin();
    }

    auto end() const -> const_iterator {
      return m_rules.end();
    }

    auto operator==(fuzzy_classifier<Truth, Input, Id> const& other) const -> bool {
      return m_rules == other.m_rules && m_i == other.m_i;
    }

    auto operator!=(fuzzy_classifier<Truth, Input, Id> const& other) const -> bool {
      return m_rules != other.m_rules || m_i != other.m_i;
    }

    class interpretation {
     public:
      /**
       * \brief Builds an interpretation with a vector of categories.
       */
      interpretation(vector<string> categories) : m_categories{std::move(categories)} {};

      /**
       * \brief Adds an input variable with equally-sized triangular fuzzy sets.
       * \param name    Name of the input variable.
       * \param nsets   Number of fuzzy sets (triangles).
       * \param a       Input value where the fuzzy sets begin.
       * \param b       Input value where the fuzzy sets end.
       */
      auto add_triangular_sets(string const& name, size_t nsets, input_type a, input_type b) -> void {
        m_names.push_back(name);
        m_partitions.push_back(make_triangles(nsets, a, b));
        m_labels.push_back(make_labels(nsets));
      }

      /**
       * \brief Number of input variables.
       */
      auto num_input() const -> size_t {
        return m_input_names.size();
      }

      /**
       * \brief Number of partitions associated with input 'n'.
       */
      auto num_partitions(id_type n) const -> size_t {
        return m_labels.at(n).size();
      }

      /**
       * \brief Number of categories (possible outputs).
       */
      auto num_categories() const -> size_t {
        return m_categories.size();
      }

      /**
       * \brief Name of the nth input variable.
       */
      auto input_name(id_type n) const -> string const& {
        return m_input_names.at(n);
      }

      /**
       * \brief Name of the nth input variable.
       */
      auto category_name(id_type n) const -> string const& {
        return m_categories.at(n);
      }

      /**
       * \brief Vector of labels for the fuzzy sets associated with the nth input variable.
       */
      auto label(id_type n) const -> vector<string> const& {
        return m_labels.at(n);
      }

      /**
       * \brief Name of the label 's' of the fuzzy sets associated with the nth input variable.
       */
      auto label(id_type n, id_type s) const -> string const& {
        return m_labels.at(n).at(s);
      }

      auto partition_name(id_type n) const -> vector<string> const& {
        return m_partition_names.at(n);
      }

      auto partition_name(id_type n, id_type p) const -> string const& {
        return m_partition_names.at(n).at(p);
      }

      /**
       * \brief Returns a reference to the fuzzy sets associated with the nth input variable.
       */
      auto get(id_type n) const -> vector<std::function<truth_type(input_type)>> const& {
        return m_partitions.at(n);
      }

      /**
       * \brief Returns a reference to the fuzzy sets 's' associated with the nth input variable.
       */
      auto get(id_type n, id_type s) const -> std::function<truth_type(input_type)> const& {
        return m_partitions.at(n).at(s);
      }

      /**
       * \brief Call the fuzzy sets 's' associated with the nth input variable with value 'x'.
       */
      auto get(id_type n, id_type s, input_type x) const -> truth_type {
        return m_partitions.at(n).at(s)(x);
      }

     private:
      vector<string> m_input_names; // Names of the variables (for the antecedants) given their id.
      vector<vector<string>> m_labels; // Name of the linguistic variables for each input variable.
      vector<vector<std::function<truth_type(input_type)>>> m_partitions; // Partition for each variable.
      vector<vector<string>> m_partition_names; // Name of the partitions (e.g.: Triangle blah blah, Gaussian, ...).
      vector<string> m_categories; // Name of the categories (output).
    };

   private:
    rules_type m_rules;
    interpretation_type m_i;
  };

  template<typename Truth, typename Input, typename Id>
  auto fuzzy_classifier<Truth, Input, Id>::get_random_rule(std::mt19937_64& rng) const -> rule_type {
    if (empty()) {
      return rule_type{};
    }
    if (size() == 1) {
      return *m_rules.begin();
    }
    auto unif = std::uniform_int_distribution<size_t>(0, size() - 1);
    auto const by = unif(rng);
    auto it = m_rules.begin();
    std::advance(it, by);
    return *it;
  }

  template<typename Truth, typename Input, typename Id>
  auto fuzzy_classifier<Truth, Input, Id>::pop_random_rule(std::mt19937_64& rng) -> rule_type {
    if (empty()) {
      return rule_type{};
    }
    auto it = m_rules.begin();
    if (size() == 1) {
      auto p = rule_type{*it};
      m_rules.erase(it);
      return p;
    }
    auto unif = std::uniform_int_distribution<size_t>(0, size() - 1);
    auto const by = unif(rng);
    std::advance(it, by);
    auto p = rule_type{*it};
    m_rules.erase(it);
    return p;
  }

  template<typename Truth, typename Input, typename Id>
  auto fuzzy_classifier<Truth, Input, Id>::evaluate(vector<input_type> const& row) -> id_type {
    auto truth_by_classes = vector<truth_type>(m_i->num_classes(), truth_trait<truth_type>::zero);
    for (auto const& rule : m_rules) {
      auto truth = truth_trait<truth_type>::unit;
      for (auto const& v : rule.first) {
        truth = truth && m_i.get(v.first, v.second, row.at(v.first));
      }
      truth_by_classes[rule.second] = truth_by_classes[rule.second] || truth;
    }
    return idx_of_maximum(truth_by_classes);
  }

  template<typename Truth, typename Input, typename Id>
  auto fuzzy_classifier<Truth, Input, Id>::evaluate_all(data_matrix<input_type, id_type> const& dm) -> confusion<size_t, double> {
    auto results = confusion<size_t, double>{m_i->num_classes()};
    for (auto const& row : dm) {
      auto predicted = evaluate(row.first);
      results.add_count(predicted, row.second);
    }
    return results;
  }

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
      cj::std_hash_combine(seed, kb.get_interpretation());
      return seed;
    }
  };

} /* end namespace std */

#endif
