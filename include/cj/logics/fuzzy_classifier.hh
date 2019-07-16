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
#include "cj/utils/bounded_multimap.hh"
#include "cj/math/set.hh"

namespace cj {

  /**
   * \brief Fuzzy rule-based classifier: predict a category given a set of inputs using a set
   *        of fuzzy rules of the form 'IF antecedents THEN category'.
   */
  template<typename Truth, typename Input, typename Id = uint32_t>
  class fuzzy_classifier {
   public:
    class interpretation;

    using truth_type = Truth;
    using input_type = Input;
    using id_type = Id;
    using self_type = fuzzy_classifier<truth_type, input_type, id_type>;
    using interpretation_ptr = std::shared_ptr<interpretation>;
    using mutate_function = std::function<void(self_type&, std::mt19937_64&)>;
    using fitness_function = std::function<double(self_type const&, data_matrix<input_type, id_type> const&)>;
    using antecedent_type = flat_map<id_type, id_type>; // Maps an input ID to a fuzzyset ID.
    using rule_type = pair<antecedent_type, id_type>; // Pairs a rule with its output (for now: a binary value).
    using rules_type = flat_map<antecedent_type, id_type>; // Maps rules to output.
    using const_iterator = typename rules_type::const_iterator;
    using operator_type = std::function<truth_type(truth_type const&, truth_type const&)>;

    /**
     * \brief Builds a fuzzy knowledge base with a pointer to fuzzy_set and (optionally) a set of
     *        intitial rules.
     */
    fuzzy_classifier(interpretation_ptr const& i, rules_type const& initial_rules = {})
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
     *        antecedents + 1).
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
    auto get_interpretation_ptr() const -> interpretation_ptr {
      return m_i;
    }

    /**
     * \brief Reference to the fuzzy sets used to define this knowledge base.
     */
    auto get_raw_interpretation_ptr() const -> interpretation const* {
      return m_i.get();
    }

    /**
     * \brief Generates a prediction (category ID) given a set of input values.
     */
    auto evaluate(vector<input_type> const& row) const -> id_type;

    /**
     * \brief Returns the confusion matrix for a database of (input, category) pairs.
     */
    auto evaluate_all(data_matrix<input_type, id_type> const& dm) const -> confusion<size_t, double>;

    /**
     * \brief Returns iterator to the beginning of the rules.
     */
    auto begin() const -> const_iterator {
      return m_rules.begin();
    }

    /**
     * \brief Returns iterator to the end of the rules.
     */
    auto end() const -> const_iterator {
      return m_rules.end();
    }

    /**
     * \brief Returns a reference to the set of rules.
     */
    auto rules() const -> rules_type const& {
      return m_rules;
    }

    auto operator==(self_type const& other) const -> bool {
      return m_rules == other.m_rules && m_i == other.m_i;
    }

    auto operator!=(self_type const& other) const -> bool {
      return m_rules != other.m_rules || m_i != other.m_i;
    }

    /**
     * \brief Helper to builds a shared pointer to an interpretation given a vector of categories.
     */
    static auto make_interpretation(vector<string> categories) -> interpretation_ptr  {
      return std::make_shared<interpretation>(interpretation{std::move(categories)});
    }

    /**
     * \brief ...
     *
     * \param fs          Initial knowledge bases.
     * \param mut         Mutation function.
     * \param fit         Fitness function.
     * \param stop        ...
     * \param training    Training data.
     * \param pop_size    ...
     * \param elites      ...
     * \param seed        Seed for the random number generator.
     * \return            Multimap from fitness to knowledge base.
     */
    static auto evolve(self_type initial, mutate_function const& mut, fitness_function const& fit,
      std::function<bool(double)> const& stop,
      data_matrix<input_type, id_type> const& training, size_t const pop_size = 1000,
      size_t const elites = 50, size_t const t_max = 1000, size_t const seed = 42) -> pair<self_type, double>;

   private:
    rules_type m_rules;
    interpretation_ptr m_i;

   public:
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
      auto add_triangular_partition(string const& name, size_t nsets, input_type a, input_type b) -> void;

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

      /**
       * \brief Name given to the parition (the fuzzy sets) associated with an input.
       * E.g.: Triangular may be used for triangular partitions.
       */
      auto partition_name(id_type n) const -> string const& {
        return m_partition_names.at(n);
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

      /**
       * \brief Shows a summary of the input and output variables.
       */
      auto summary(std::ostream& os) -> std::ostream&;

     private:
      vector<string> m_input_names; // Names of the variables (for the antecedants) given their id.
      vector<vector<string>> m_labels; // Name of the linguistic variables for each input variable.
      vector<vector<std::function<truth_type(input_type)>>> m_partitions; // Partition for each variable.
      vector<string> m_partition_names; // Name of the partitions (e.g.: Triangle blah blah, Gaussian, ...).
      vector<string> m_categories; // Name of the categories (output).
    };
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
  auto fuzzy_classifier<Truth, Input, Id>::evaluate(vector<input_type> const& row) const -> id_type {
    auto truth_by_classes = vector<truth_type>(m_i->num_categories(), truth_type{0});
    for (auto const& rule : m_rules) {
      auto truth = truth_type{1};
      for (auto const& v : rule.first) {
        truth = truth && m_i->get(v.first, v.second, row.at(v.first));
      }
      truth_by_classes[rule.second] = truth_by_classes[rule.second] || truth;
    }
    return idx_of_maximum(truth_by_classes);
  }

  template<typename Truth, typename Input, typename Id>
  auto fuzzy_classifier<Truth, Input, Id>::evaluate_all(data_matrix<input_type, id_type> const& dm)
      const -> confusion<size_t, double> {
    auto results = confusion<size_t, double>{m_i->num_categories()};
    for (auto const& row : dm) {
      results.add_count(evaluate(row.first), row.second);
    }
    return results;
  }

  template<typename Truth, typename Input, typename Id>
  auto fuzzy_classifier<Truth, Input, Id>::evolve(self_type initial, mutate_function const& mut,
      fitness_function const& fit, std::function<bool(double)> const& stop,
      data_matrix<input_type, id_type> const& training, size_t const pop_size, size_t const elites,
      size_t const t_max, size_t const seed) -> pair<self_type, double> {
    assert(pop_size > 0);
    assert(pop_size > elites);
    assert(elites > 0);
    assert(t_max > 0);

    auto mutations = std::binomial_distribution<int>(100, 0.03);
    auto const inter = initial.get_interpretation_ptr();

    auto const non_elites = pop_size - elites;
    auto rng = std::mt19937_64(seed);

    // Creates the initial population of solutions:
    auto pop = vector<self_type>(pop_size, initial);
    auto fitnesses = bounded_multimap<double, size_t>(elites); // Maps the highest fitness to the population's index in 'pop'.
    fitnesses.try_insert(fit(initial, training), 0);

    for (auto t = size_t{0}; t < t_max; ++t) {
      // Mutates and store fitness:
      for (auto p = size_t{0}; p < pop_size; ++p) {
        auto const num_mutations = mutations(rng);
        for (auto m = size_t{0}; m < num_mutations; ++m) {
          mut(pop[p], rng);
        }
        fitnesses.try_insert(fit(pop[p], training), p);
      }
      if (stop(fitnesses.maximum_key())) {
        break;
      }
      auto const fittest = fitnesses.set_of_values();
      // Only mate the non-elites, keep the elites untouched:
      for (auto p = size_t{0}; p < pop_size; ++p) {
        if (fittest.find(p) == fittest.end()) {
          auto const parents = pick_unique_pair(fittest, rng);
          pop[p] = self_type {
            inter,
            map_intersection_split_union(pop[parents[0]].rules(), pop[parents[1]].rules(), rng)
          };
        }
      }
    }
    return {pop[fitnesses.maximum().second], fitnesses.maximum_key()};
  }

  template<typename Truth, typename Input, typename Id>
  auto fuzzy_classifier<Truth, Input, Id>::interpretation::add_triangular_partition(string const& name,
      size_t nsets, input_type a, input_type b) -> void {
    m_input_names.push_back(name);
    m_partitions.push_back(make_triangles(nsets, a, b, truth_type{0}, truth_type{1}));
    m_partition_names.push_back("Triangular_{" + boost::lexical_cast<string>(nsets) + "}("
      + boost::lexical_cast<string>(a) + ", " + boost::lexical_cast<string>(b) + ")");
    m_labels.push_back(make_labels(nsets));
  }

  template<typename Truth, typename Input, typename Id>
  auto show_rule(std::ostream& os, typename fuzzy_classifier<Truth, Input, Id>::rule_type const& p,
      typename fuzzy_classifier<Truth, Input, Id>::interpretation const* i) -> void {
    if (p.first.size()) {
      auto it = p.first.begin();
      os << "If " << i->input_name(it->first) << ' ' << i->label(it->first, it->second);
      ++it;
      for (; it != p.first.end(); ++it) {
        os << " and " << i->input_name(it->first) << ' ' << i->label(it->first, it->second);
      }
      os << " then " << i->category_name(p.second);
    }
  }

  template<typename Truth, typename Input, typename Id>
  auto fuzzy_classifier<Truth, Input, Id>::interpretation::summary(std::ostream& os)
      -> std::ostream& {
    os << "Input variables:\n";
    for (auto n = 0u; n < num_input(); ++n) {
      os << "  " << n << ": " << input_name(n) << " :: " << partition_name(n) << '\n';
    }
    os << "\nCategories (output):\n";
    for (auto n = 0u; n < num_categories(); ++n) {
      os << "  " << n << ": " << category_name(n) << '\n';
    }
    return os;
  }

  template<typename Truth, typename Input, typename Id>
  auto operator<<(std::ostream& os, fuzzy_classifier<Truth, Input, Id> const& c) -> std::ostream& {
    for (auto const& r : c) {
      show_rule<Truth, Input, Id>(os, r, c.get_raw_interpretation_ptr());
      os << '\n';
    }
    return os;
  }

} /* end namespace cj */

namespace std {

  template<typename Truth, typename Input, typename ID>
  struct hash<cj::fuzzy_classifier<Truth, Input, ID>> {
    auto operator()(cj::fuzzy_classifier<Truth, Input, ID> const& kb) const -> size_t {
      auto seed = size_t{51274857491742893LU};
      for (auto const& r : kb) {
        cj::std_hash_combine(seed, r);
      }
      cj::std_hash_combine(seed, kb.get_interpretation_ptr());
      return seed;
    }
  };

} /* end namespace std */

#endif
