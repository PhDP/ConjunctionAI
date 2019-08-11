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
//    auto const prob_rule = 0.7 - 0.4 / (1.0 + std::exp(-double(c.complexity()) / 5 + 5));
    auto const prob_rule = 0.6 - 0.4 / (1.0 + std::exp(-double(c.complexity()) / 2 + 5));
    auto const n = c.get_raw_interpretation_ptr()->num_input(); // Number of input variables

//    std::cout << "Mutation (prob rule: " << prob_rule << "):\n";

    // Add rule:
    if (c.size() < 3 || unif(rng) < prob_rule) {
//      std::cout << "  Add rule \"";
      // Number of input variables in the new rule:
      auto const n_input_vars = 1 + unif(rng) * (n - 1);
      auto rule = rule_type{};
      for (auto i = 0u; i < n_input_vars; ++i) {
        auto const input_id = uint32_t(unif(rng) * n);
        rule.first[input_id] = uint32_t(unif(rng) * c.get_raw_interpretation_ptr()->num_partitions(input_id));
      }
      rule.second = uint32_t(unif(rng) * c.get_raw_interpretation_ptr()->num_categories());
//      cj::show_rule<Truth, double, uint32_t>(std::cout, rule, c.get_raw_interpretation_ptr());
//      std::cout << "\"\n";
      c.add_rule(rule);
    } else { // Try modifying existing rule:
      auto rule = c.pop_random_rule(rng);
//      std::cout << "  Modify rule \"";
//      cj::show_rule<Truth, double, uint32_t>(std::cout, rule, c.get_raw_interpretation_ptr());
//      std::cout << "\"\n";
      if (rule == rule0 || rule == rule1) { // Do not touch initial rules
//        std::cout << "  No modifications, put it back!\n";
        c.add_rule(rule);
      } else if (unif(rng) > (1 - prob_rule)) { // Otherwise just left the rule out.
        if (unif(rng) < (1 / double(n))) { // Mutate category
//          std::cout << "  Mutate output\n";
          rule.second = uint32_t(unif(rng) * c.get_raw_interpretation_ptr()->num_categories());
        } else {
          // 1/3 of removing
          // 1/3 of changing
          // 1/3 of adding

          auto const input_id = uint32_t(unif(rng) * n);
//          std::cout << "  Mutate input " << input_id << " (" << c.get_raw_interpretation_ptr()->input_name(input_id) << "): ";
          if (rule.first.find(input_id) != rule.first.end() && unif(rng) < 0.5) {
            rule.first.erase(input_id);
//            std::cout << "    Remove it!\n";
          } else {
//            std::cout << "    Change it!\n";
            rule.first[input_id] = uint32_t(unif(rng) * c.get_raw_interpretation_ptr()->num_partitions(input_id));
          }
        }
        c.add_rule(rule);
      } else {
//        std::cout << "  Remove rule entirely!\n";
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

  auto tsses = cj::vector<double>{};
  tsses.reserve(trials);
  for (auto const& b : bests) {
    tsses.push_back(b.evaluate_all(testing).tss(1));
  }
  auto const evolved_tss = cj::fast_mean(tsses.begin(), tsses.end());

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
    << '\n'
    << "Tss(initial): " << initial_tss << '\n'
    << "Tss(evolved): " << evolved_tss << '\n'
    << "Improvement: " << (evolved_tss - initial_tss) << '\n';

  out.close();
}

auto main(int argc, char *argv[]) -> int {
  auto logic_name = cj::get_arg<cj::string>(argc, argv, "logic", cj::string{"Łukasiewicz"});
  auto const seed = cj::get_arg<size_t>(argc, argv, "seed", std::time(0));
  auto const trials = cj::get_arg<uint32_t>(argc, argv, "trials", 20);
  auto const nsets = cj::get_arg<uint32_t>(argc, argv, "nsets", 5); // How many fuzzy sets for each variables.
  auto const pop_size = std::max(cj::get_arg<uint32_t>(argc, argv, "populations", 20), uint32_t{8});
  auto const t_max = std::max(cj::get_arg<uint32_t>(argc, argv, "steps", 10), uint32_t{100});
  auto const alpha = cj::get_arg<double>(argc, argv, "alpha", 0.0005);
  auto const ptest = 0.1;

  auto main_rng = std::mt19937_64(seed);

  // Make sure the logic's name is valid:
  if (logic_name != "Łukasiewicz") {
    if (logic_name == "Godel" || logic_name == "Gödel" || logic_name != "Gödel-Dummett") {
      logic_name = "Gödel-Dummett";
    } else if (logic_name == "Product") {
    } else {
      if (logic_name != "Lukasiewicz") {
        std::cout << "WARNING: Invalid logic name, defaulting to \"Łukasiewicz\".\n";
      }
      logic_name = "Łukasiewicz";
    }
  }

  std::cout
    << std::boolalpha
    << "Parameters:\n"
    << "  Seed: " << seed << '\n'
    << "  Trials: " << trials << '\n'
    << "  Logic: " << logic_name << '\n'
    << "  Fuzzy sets per input variables: " << nsets << '\n'
    << "  Population size: " << pop_size << '\n'
    << "  Complexity penalty (alpha): " << alpha << '\n'
    << "  Proportion held for testing: " << ptest << '\n';

  auto data_ref = cj::data_matrix<double, uint32_t>::from_file("../data/poll_plant/poll.csv");
  if (!data_ref) {
    std::cout << "ERROR: Failed to load data. You must execute this program in a folder with 'data/poll_plant/poll.csv'\n";
    return 0;
  }
  auto data = *data_ref;

  std::cout << "\nInput variables:\n";
  for (auto h = 0u; h < data.input_names().size(); ++h) {
    std::cout << "  " << h << ": " << data.input_name(h) << '\n';
  }
  std::cout << "\nOutput variable: " << data.output_name() << '\n';

  std::cout << "\nEntries in the data-set: " << data.nrows() << '\n';

  auto test = data.split_frame(ptest, main_rng);

  std::cout
    << "Training data size: " << data.nrows() << '\n'
    << "Testing data size: " << test.nrows() << '\n';

  if (logic_name == "Łukasiewicz") {
    parallel_trials<cj::lukasiewicz<double>>("Łukasiewicz", trials, 16, seed, nsets, pop_size, t_max, alpha, data, test, "Luka");
  }

//  auto fitness = [=alpha](auto c, auto d) {
//    return c.evaluate_all(d).tss(1) - alpha * c.complexity();
//  };

//  auto main_rng = std::mt19937_64(seed);
//  auto seed_gen = std::uniform_int_distribution<size_t>{};
//
//  std::cout << "Seeds:\n";
//  cj::vector<std::mt19937_64> main_rngs;
//  for (auto t = 0u; t < trials; ++t) {
//    auto const seed = seed_gen(main_rng);
//    std::cout << "  " << seed << '\n';
//    main_rngs.push_back(std::mt19937_64(seed));
//  }
//  std::cout << std::endl;
//
//  if (verbosity == 0) {
//    std::ios::sync_with_stdio(false);
//  }
//
//  auto futures = cj::vector<std::future<std::pair<double, cj::fuzzy_kb>>>{};
//
//  for (auto t = 0u; t < trials; ++t) {
//    futures.push_back(std::async([&](std::mt19937_64 rng) {
//      // Builds fuzzy set!
//      auto unif = std::uniform_real_distribution<double>(0.0, 1.0);
//      auto fs = std::make_unique<cj::fuzzy_sets>(cj::fuzzy_sets{"non-interaction", "interaction"});
//
//      fs->add_triangular_sets(data.header(0), 2);
//      for (auto i = 1u; i < 7; ++i) {
//        fs->add_triangular_sets(data.header(i), nsets);
//      }
//
//      auto const fitness = cj::fitness(logic, alpha, true);
//
//      using rule_type = typename cj::fuzzy_kb::rule_type;
//      auto const fixed = cj::unordered_set<rule_type>{rule_type{{{0, 0}}, false}, rule_type{{{0, 1}}, true}};
//
//      auto const kb0 = cj::fuzzy_kb{fs.get(), {rule_type{{{0, 0}}, false}, rule_type{{{0, 1}}, true}}};
//
//      auto const test_tss0 = cj::evaluate(kb0, test, logic).tss();
//      double const fitness0 = fitness(kb0, data);
//
//      // Knowledge base, random number generator, and fitness for each population:
//      auto kbs = cj::vector<cj::fuzzy_kb>{};
//      auto rngs = cj::vector<std::mt19937_64>{};
//      auto fitnesses = cj::vector<double>{};
//
//      for (auto p = 0u; p < pop_size; ++p) {
//        kbs.push_back(kb0);
//        rngs.push_back(std::mt19937_64(rng()));
//        fitnesses.push_back(fitness0);
//      }
//
//      for (auto c = 0u; c < crossovers; ++c) {
//        for (auto p = 0u; p < pop_size; ++p) {
//          for (auto m = 0u; m < mutations; ++m) {
//            fitnesses[p] = cj::mutate(kbs[p], data, fitnesses[p], fitness, rngs[p], fixed);
//          }
//        }
//        if (elites < pop_size) {
//          auto fitness_multimap = cj::flat_multimap<double, cj::fuzzy_kb>{};
//          auto i = 0u;
//          for (; i < pop_size; ++i) {
//            fitness_multimap.insert(std::pair<double, cj::fuzzy_kb>{fitnesses[i], kbs[i]});
//          }
//          i = 0u;
//          auto it = fitness_multimap.rbegin();
//          for (; i < elites; ++i, ++it) {
//            kbs[i] = it->second;
//            fitnesses[i] = it->first;
//          }
//          for (; i < pop_size; ++i) {
//            auto const mom = uint32_t(unif(rng) * pop_size);
//            auto const dad = uint32_t(unif(rng) * pop_size);
//            if (mom == dad) {
//              kbs[i] = kbs[mom];
//              fitnesses[i] = fitnesses[mom];
//            } else {
//              auto mom_it = fitness_multimap.rbegin(), dad_it = fitness_multimap.rbegin();
//              std::advance(mom_it, mom);
//              std::advance(dad_it, dad);
//              kbs[i] = cj::mate(mom_it->second, dad_it->second, rng);
//              fitnesses[i] = fitness(kbs[i], data);
//            }
//          }
//        }
//      }
//
//      auto best_fitness_idx = 0;
//      auto best_fitness = fitnesses[0];
//      for (auto i = 1u; i < pop_size; ++i) {
//        if (fitnesses[i] > best_fitness) {
//          best_fitness = fitnesses[i];
//          best_fitness_idx = i;
//        }
//      }
//      return std::pair<double, cj::fuzzy_kb>{test_tss0, kbs[best_fitness_idx]};
//    }, main_rngs[t]));
//  }
//
//  auto best_kbs = cj::vector<std::pair<double, cj::fuzzy_kb>>{};
//  for(auto &e : futures) {
//    best_kbs.push_back(e.get());
//  }
//
//  auto most_frequent = uint32_t{0};
//
//  {
//    auto frequencies = cj::unordered_map<cj::fuzzy_kb, uint32_t>{};
//    for (auto const& k : best_kbs) {
//      if (frequencies.find(k.second) != frequencies.end()) {
//        frequencies[k.second] = frequencies[k.second] + 1;
//      } else {
//        frequencies[k.second] = 1;
//      }
//      if (frequencies[k.second] > most_frequent) {
//        most_frequent = frequencies[k.second];
//      }
//    }
//  }
//
//  auto deltas = cj::vector<double>{};
//  for (auto t = 0u; t < trials; ++t) {
//    auto const new_tss = cj::evaluate(best_kbs[t].second, test, logic).tss();
//    deltas.push_back(new_tss - best_kbs[t].first);
//    std::cout << "\n# Trials " << t << '\n';
//    std::cout << "\n## Best knowledge base\n\n" << best_kbs[t].second;
//    std::cout << "\nTSS change: " << best_kbs[t].first << " -> " << new_tss << " (improvement: " << deltas[t] << ")\n";
//  }
//  std::cout << "\nMean improvement: " << cj::mean(deltas.begin(), deltas.end()) << '\n';
//  std::cout << "\nFrequency of the most common solution: " << (most_frequent / double(trials)) << '\n';
//
  return 0;
}
