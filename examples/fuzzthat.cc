#include <iostream>
#include <random>
#include <tuple>
#include <functional>
#include <iterator>
#include <utility>
#include <thread>
#include <future>
#include <ctime>
#include "cj/logics/fuzzy_classifier.hh"
#include "cj/utils/cl_reader.hh"
#include "cj/math/statistics.hh"

template<typename Truth>
auto make_interpretation(size_t nsets, cj::data_matrix<double, uint32_t> const& dm)
                         -> typename cj::fuzzy_classifier<Truth, double>::interpretation_ptr {
  auto i = cj::fuzzy_classifier<Truth, double>::make_interpretation({"Non-interaction", "Interaction"});
  i->add_triangular_partition(dm.input_name(0), 2, 0.0, 1.0);
  for (auto h = 1u; h < dm.input_names().size(); ++h) {
    i->add_triangular_partition(dm.input_name(h), nsets, 0.0, 1.0);
  }
  return i;
}

template<typename Truth>
auto trial(size_t const seed, size_t const nsets, size_t const pop_size, size_t const t_max, double const alpha, cj::data_matrix<double, uint32_t> const& dm)
          -> cj::fuzzy_classifier<Truth, double, uint32_t> {
  using classifier = cj::fuzzy_classifier<Truth, double, uint32_t>;
  using rule_type = typename classifier::rule_type;

  auto rng = std::mt19937_64(seed);
  auto i = make_interpretation<Truth>(nsets, dm);
  auto const rule0 = rule_type{{{0, 0}}, 0};
  auto const rule1 = rule_type{{{0, 1}}, 1};
  auto const initial_rule = classifier(i, {rule0, rule1});

  auto const mutate = [&rule0, &rule1](classifier& c, std::mt19937_64& rng) {
    auto unif = std::uniform_real_distribution<double>(0.0, 1.0);
    auto const prob_rule = 0.6 - 0.4 / (1.0 + std::exp(-double(c.complexity()) / 2 + 5));
    auto const n = c.get_raw_interpretation_ptr()->num_input(); // Number of input variables

    // Add rule:
    if (c.size() < 3 || unif(rng) < prob_rule) {
      // Number of input variables in the new rule:
      auto const n_input_vars = 1 + unif(rng) * (n - 1);
      auto rule = rule_type{};
      for (auto i = 0u; i < n_input_vars; ++i) {
        auto const input_id = uint32_t(unif(rng) * n);
        rule.first[input_id] = uint32_t(unif(rng) * c.get_raw_interpretation_ptr()->num_partitions(input_id));
      }
      rule.second = uint32_t(unif(rng) * c.get_raw_interpretation_ptr()->num_categories());
      c.add_rule(rule);
    } else { // Try modifying existing rule:
      auto rule = c.pop_random_rule(rng);
      if (rule == rule0 || rule == rule1) { // Do not touch initial rules
        c.add_rule(rule);
      } else if (unif(rng) > (1 - prob_rule)) { // Otherwise just leave the rule out.
        if (unif(rng) < (1 / double(n))) { // Mutate category
          rule.second = uint32_t(unif(rng) * c.get_raw_interpretation_ptr()->num_categories());
        } else {
          // 1/3 of removing
          // 1/3 of changing
          // 1/3 of adding

          auto const input_id = uint32_t(unif(rng) * n);
          if (rule.first.find(input_id) != rule.first.end() && unif(rng) < 0.5) {
            rule.first.erase(input_id);
          } else {
            rule.first[input_id] = uint32_t(unif(rng) * c.get_raw_interpretation_ptr()->num_partitions(input_id));
          }
        }
        c.add_rule(rule);
      }
    }
  };

  auto const fitness = [alpha](classifier const& c, cj::data_matrix<double, uint32_t> const& d) {
    return c.evaluate_all(d).tss(1) - alpha * c.complexity();
  };

  auto const stop = [](double fit) { return fit >= 1.0; };

  return classifier::evolve(initial_rule, mutate, fitness, stop, dm, pop_size, pop_size / 4, t_max, seed);
}

template<typename Truth>
auto parallel_trials(cj::string const& tnorm, size_t const trials, size_t const threads,
                     size_t const seed, size_t const nsets, size_t const pop_size,
                     size_t const t_max, double alpha, cj::data_matrix<double, uint32_t> const& dm,
                     cj::data_matrix<double, uint32_t> const& testing,
                     char const* filename_prefix) -> void {
  using classifier = cj::fuzzy_classifier<Truth, double, uint32_t>;
  auto futures = cj::vector<std::future<classifier>>{};
  futures.reserve(trials);

  auto rng = std::mt19937_64(seed);
  auto seed_gen = std::uniform_int_distribution<size_t>{};

  auto seeds = cj::vector<size_t>{};
  seeds.reserve(trials);
  for (auto t = 0; t < trials; ++t) {
    seeds.push_back(seed_gen(rng));
  }

  for (auto t = 0u; t < trials; ++t) {
    futures.push_back(std::async([&](size_t const s) {
      return trial<Truth>(s, nsets, pop_size, t_max, alpha, dm);
    }, seeds[t]));
  }

  auto bests = cj::vector<classifier>{};
  bests.reserve(trials);
  for (auto& f : futures) {
    bests.push_back(f.get());
  }

  // Collect info on best knowedge bases:
  auto tsses = cj::vector<double>{};
  auto complexities = cj::vector<size_t>{};
  auto nrules = cj::vector<size_t>{};
  tsses.reserve(trials);
  complexities.reserve(trials);
  nrules.reserve(trials);
  for (auto const& b : bests) {
    tsses.push_back(b.evaluate_all(testing).tss(1));
    complexities.push_back(b.complexity());
    nrules.push_back(b.size());
  }
  auto const evolved_tss = cj::fast_mean(tsses.begin(), tsses.end());
  auto const mean_complexity = cj::fast_mean(complexities.begin(), complexities.end());
  auto const mean_nrules = cj::fast_mean(nrules.begin(), nrules.end());

  auto i = make_interpretation<Truth>(nsets, dm);
  auto const initial_c = classifier(i, {{{{0, 0}}, 0}, {{{0, 1}}, 1}});
  auto const initial_tss = initial_c.evaluate_all(testing).tss(1);

  // Print results to file:
  auto const filename = cj::string{filename_prefix} + cj::string{"-"}
                        + boost::lexical_cast<cj::string>(seed) + cj::string{".txt"};

  auto out = std::ofstream(filename);

  out
    << "Seed: " << seed << '\n'
    << "Tnorm: " << tnorm << '\n'
    << "Trials: " << trials << '\n'
    << "Sets / input variables: " << nsets << '\n'
    << "Population size: " << pop_size << '\n'
    << "T(max): " << t_max << '\n'
    << "Alpha: " << alpha << '\n'
    << "Mean complexity: " << mean_complexity << '\n'
    << "Mean number of rules: " << mean_nrules << '\n'
    << '\n'
    << "Tss(initial): " << initial_tss << '\n'
    << "Tss(evolved): " << evolved_tss << '\n'
    << "Improvement: " << (evolved_tss - initial_tss) << '\n';

  out.close();
}

auto main(int argc, char *argv[]) -> int {
  auto logic_name = cj::get_arg<cj::string>(argc, argv, "logic", cj::string{"Łukasiewicz"});
  auto const seed = cj::get_arg<size_t>(argc, argv, "seed", std::time(0));
  auto const trials = cj::get_arg<uint32_t>(argc, argv, "trials", 100);
  auto const nsets = cj::get_arg<uint32_t>(argc, argv, "nsets", 5); // How many fuzzy sets for each variables.
  auto const pop_size = std::max(cj::get_arg<uint32_t>(argc, argv, "populations", 100), uint32_t{8});
  auto const t_max = std::max(cj::get_arg<uint32_t>(argc, argv, "steps", 10), uint32_t{100});
  auto const alpha = cj::get_arg<double>(argc, argv, "alpha", 0.0005);
  auto const ptest = 0.1;
  auto const threads = std::thread::hardware_concurrency() + 1;

  auto main_rng = std::mt19937_64(seed);

  // Make sure the logic's name is valid:
  if (logic_name != "Łukasiewicz") {
    if (logic_name == "Godel" || logic_name == "Gödel" || logic_name == "Gödel-Dummett") {
      logic_name = "Gödel-Dummett";
    } else if (logic_name == "Product") {
    } else {
      if (logic_name != "Lukasiewicz") {
        std::cout << "WARNING: Invalid logic name, defaulting to \"Łukasiewicz\".\n";
      }
      logic_name = "Łukasiewicz";
    }
  }

  std::cout << seed << '\n';

  auto data_ref = cj::data_matrix<double, uint32_t>::from_file("../data/poll_plant/poll.csv");
  if (!data_ref) {
    std::cout << "ERROR: Failed to load data. You must execute this program in a folder with 'data/poll_plant/poll.csv'\n";
    return 0;
  }
  auto data = *data_ref;

  auto test = data.split_frame(ptest, main_rng);

  if (logic_name == "Łukasiewicz") {
    parallel_trials<cj::lukasiewicz<double>>("Łukasiewicz", trials, threads, seed, nsets, pop_size, t_max, alpha, data, test, "Luka");
  } else if (logic_name == "Gödel-Dummett") {
    parallel_trials<cj::godel<double>>("Gödel-Dummett", trials, threads, seed, nsets, pop_size, t_max, alpha, data, test, "Godel");
  } else { // "Product"
    parallel_trials<cj::product<double>>("Product", trials, threads, seed, nsets, pop_size, t_max, alpha, data, test, "Prod");
  }

  return 0;
}
