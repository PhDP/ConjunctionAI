<img src='http://phdp.github.io/images/cj-200.png' alt='Conjunction logo' align='right'/>

> **ConjunctionAI is an AI library to. ConjunctionAI achieves this by exploring what lies at the
intersection of logic, fuzziness, and probability theory.**

ConjunctionAI is a C++17 library with a Python package.

## Code organization

All files directly in *include/cj* represent different mathematical objects or logics (except
*common.hh*). Each of these files begin with a detailed summary and list of classes/functions,
followed by, of course, the code for these classes/functions.

Math objects:

| Object          | TLDR;                                                     | Where? |
| --------------- | --------------------------------------------------------  | ------ |
| Set             | Standard sets & many type traits used across all sets     | set.hh |
| Fuzzy set       | Discrete fuzzy sets                                       | fuzzy_set.hh |
| Fuzzy partition | Function to generate continuous fuzzy sets to partition an input space | fuzzy_partition.hh |
| Graph           | Extremely generic type traits and functions for standard and fuzzy graphs | graph.hh |
| Truth           | Defines truth classes for many-valued logics              | truth.hh |

Logics (here, zeroth-order logic is a synonym of propositional logic):

| Language                    | Logic         | Epistemology                              | Where?                     |
| --------------------------- | ------------- | ----------------------------------------- | -------------------------- |
| Fuzzy rule-based classifier | Zeroth-order  | Many-valued                               | fuzzy_classifier.hh        |
| First-order logic           | First-order   | Bivalent or Many-Valued                   | fol.hh                     |
| Hybrid Markov logic         | First-order   | Probabilistic + Many-valued-ish :star2:   | hybrid_markov_logic.hh     |
| System F                    | Higher-order  | Bivalent                                  | system_f.hh                |

:star2: Hybrid Markov logic supports soft equalities, soft inequalities and similarly 'soft'
predicates.

## Building
[![Build status](https://travis-ci.org/PhDP/ConjunctionAI.svg?branch=master)](https://travis-ci.org/PhDP/ConjunctionAI)

The hard dependencies outside a C++17-compliant compiler are:

* [cmake](https://cmake.org/) for building the library, tests, benchmarks, examples.
* [boost](https://www.boost.org/) for lots of things (flat containers, math functions...).
* [eigen3](http://eigen.tuxfamily.org/) for linear algebra.
* [ceres solver](http://ceres-solver.org/) for non-linear optimization (which is also using eigen).

For convenience, some libraries are included directly in the repository or using git submodule:

* [google test](https://github.com/google/googletest/) is a git submodule used for tests.
* [google benchmark](https://github.com/google/benchmark/) is a git submodule used for benchmarks.
* [pybind11](https://github.com/pybind/pybind11/) is a git submodule used to build the Python3 package.

Use the '--recursive' option with git to fetch the submodules, e.g.:

    $ git clone --recursive https://github.com/PhDP/ConjunctionAI.git

    $ sudo apt install cmake libboost-all-dev libeigen3-dev libceres-dev

    $ dnf install cmake boost-devel eigen3-devel ceres-solver-devel

Using [brew](https://brew.sh/), install the following packages: 'boost',
'cmake', 'ceres', 'eigen'.

Recent versions of Visual Studio support the C++17 standard and all main dependencies (boost,
ceres, eigen) should work on Windows, however it is currently untested.

    $ mkdir build && cd $_
    $ export CXX=g++-8
    $ cmake ..
    $ make
    $ sudo make install

## License

The code is released under the permissive [Apache v2](http://www.apache.org/licenses/LICENSE-2.0)
license, [along with an exception to ensure GPLv2 compatibility](https://lwn.net/Articles/701155/) see
**LICENSE.md**.
