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

template<typename Truth>
auto make_interpretation(size_t nsets, cj::data_matrix<double, uint32_t> const& dm)
                         -> typename cj::fuzzy_classifier<Truth, double>::interpretation_ptr {
  auto i = cj::fuzzy_classifier<Truth, double>::make_interpretation({"Non-interaction", "Interaction"});
  i->add_triangular_sets(dm.input_name(0), 2, 0.0, 1.0);
  for (auto h = 1u; h < dm.input_names().size(); ++h) {
    i->add_triangular_partition(dm.input_name(h), nsets, 0.0, 1.0);
  }
  return i;
}

template<typename Truth>
auto trial(size_t seed) -> double {
  auto rng = std::mt19937_64(seed);
  return 0.0;
}

auto main(int argc, char *argv[]) -> int {
  auto logic_name = cj::get_arg<cj::string>(argc, argv, "logic", cj::string{"Łukasiewicz"});
  auto const seed = cj::get_arg<size_t>(argc, argv, "seed", std::time(0));
  auto const trials = cj::get_arg<uint32_t>(argc, argv, "trials", 20);
  auto const nsets = cj::get_arg<uint32_t>(argc, argv, "nsets", 5); // How many fuzzy sets for each variables.
  auto const pop_size = std::max(cj::get_arg<uint32_t>(argc, argv, "populations", 200), uint32_t{8});
  auto const elites = std::min(cj::get_arg<uint32_t>(argc, argv, "elites", pop_size / 10), pop_size / 2);
  auto const nonelites = pop_size - elites;
  auto const alpha = cj::get_arg<double>(argc, argv, "alpha", 0.0005);
  auto const ptest = 0.1;

  auto main_rng = std::mt19937_64(seed);

  // Make sure the logic's name is valid:
  if (logic_name != "Łukasiewicz") {
    if (logic_name == "Godel" || logic_name == "Gödel" || logic_name != "Gödel-Dummett") {
      logic_name = "Gödel-Dummett";
    } else if (logic_name == "Product") {
    } else {
      std::cout << "WARNING: Invalid logic name, defaulting to \"Łukasiewicz\".\n";
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
    << "  Elites: " << elites << '\n'
    << "  Non-elites: " << nonelites << '\n'
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
