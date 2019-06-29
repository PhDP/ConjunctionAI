<img src='http://phdp.github.io/images/cj.svg.png' alt='Conjunction logo' align='right'/>

> **ConjunctionAI is an AI library to explore what lies at the intersection of logic, fuzziness, and
probability theory.**

ConjunctionAI is a C++17 library with a Python package (maybe an R package someday?).

## Languages

(here, zeroth-order logic is a synonym of propositional logic)

| Language                           | Logic         | Epistemology                              | Where?                        |
| ---------------------------------- |:-------------:| :---------------------------------------: | ----------------------------- |
| Fuzzy rule-based classifier        | Zeroth-order  | Many-valued                               | cj/fuzzy_classifier.hh        |
| First-order logic                  | First-order   | Bivalent or Many-Valued                   | cj/fol.hh                     |
| Hybrid Markov logic                | First-order   | Probabilistic + Many-valued-ish :star2:   | cj/hybrid_markov_logic.hh     |
| System F                           | Higher-order  | Bivalent                                  | cj/system_f.hh                |

:star2: Hybrid Markov logic supports soft equalities, soft inequalities and similarly 'soft'
predicates.

## Dependencies
[![Build status](https://travis-ci.org/PhDP/GryffonAI.svg?branch=master)](https://travis-ci.org/PhDP/GryffonAI)

The hard dependencies outside a C++17-compliant compiler are:

* [cmake](https://cmake.org/) for building the library, tests, benchmarks, examples.
* [boost](https://www.boost.org/) for lots of things (flat containers, math functions...).
* [eigen3](http://eigen.tuxfamily.org) for linear algebra.
* [ceres solver](http://ceres-solver.org/) for non-linear optimization (which is also using eigen).

For convenience, some libraries are included directly in the repository or using git submodule:

* [catch2](https://github.com/catchorg/Catch2/) is included directly in the 'test' folder.
* [google benchmark](https://github.com/google/benchmark) is a git submodule used for benchmarks (duh!).
* [pybind11](https://github.com/pybind/pybind11) is a git submodule used to build the Python3 package.

Use the '--recursive' option with git to fetch the two submodules, e.g.:

    $ git clone --recursive https://github.com/PhDP/GryffonAI.git

## Installing dependencies

### Linux: Ubuntu

    $ sudo apt install cmake libboost-all-dev libeigen3-dev libceres-dev

### Linux: Fedora

    $ dnf install cmake boost-devel eigen3-devel ceres-solver-devel

### Mac OSX

Using [brew](https://brew.sh/), install the following packages: 'boost',
'cmake', 'ceres', 'eigen'.

### Microsoft Windows

Recent versions of Visual Studio support the C++17 standard and all main dependencies (boost,
ceres, eigen) should work on Windows, however it is currently untested.

## Building

On UNIX-like systems, cmake honors the CXX environmetal variable, so you can set your compiler
before building, e.g.:

    $ mkdir build && cd $_
    $ export CXX=g++-8
    $ cmake ..
    $ make
    $ sudo make install

## License

The code is released under the permissive [Apache v2](http://www.apache.org/licenses/LICENSE-2.0)
license, [along with an exception to ensure GPLv2 compatibility](https://lwn.net/Articles/701155/) see
**LICENSE.md**.
