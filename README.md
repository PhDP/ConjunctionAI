<img src='http://phdp.github.io/images/cj-200.png' alt='Conjunction logo' align='right'/>

Conjunction is an experimental C++17 template library for testing ideas in fuzzy/probabilistic
logics.

## Building
[![Build status](https://travis-ci.org/PhDP/Conjunction.svg?branch=master)](https://travis-ci.org/PhDP/Conjunction)

The hard dependencies outside a C++17-compliant compiler are:

* [cmake](https://cmake.org/) for building the library, tests, benchmarks, examples.
* [boost](https://www.boost.org/) for lots of things (flat containers, math functions...).
* [eigen3](http://eigen.tuxfamily.org/) for linear algebra.
* [ceres solver](http://ceres-solver.org/) for non-linear optimization (which is also using eigen).

For convenience, some libraries are included directly in the repository or using git submodule:

* [nlohmann's json](https://github.com/nlohmann/json) 3.6.1 is directly in the include directory.
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

## Style guide

The project mostly follows [LLVM's style guide](https://llvm.org/docs/CodingStandards.html) with the
following notes:

* auto/using all the things!
* Indent with two spaces, including namespaces. Half-indent 'private/public/protected' qualifiers.

## License

The code is released under the permissive [Apache v2](http://www.apache.org/licenses/LICENSE-2.0)
license, [along with an exception to ensure GPLv2 compatibility](https://lwn.net/Articles/701155/) see
**LICENSE.md**.
