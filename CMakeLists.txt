cmake_minimum_required(VERSION 3.8)
project(conjunction CXX)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "No build type provided, default to RELEASE.")
  set(CMAKE_BUILD_TYPE "RELEASE")
endif()

option(CJ_STATIC            "Builds a static library ('cj_static') instead of a shared one ('cj')." OFF)
option(CJ_CUDA              "Builds CUDA library." OFF)
option(CJ_TEST              "Builds tests." OFF)
option(CJ_BENCH             "Builds benchmarks." OFF)
option(CJ_EXAMPLES          "Builds examples." OFF)
option(CJ_PYTHON            "Builds Python package." OFF)
option(CJ_INSTALL_PYTHON    "Creates install target for Python package." OFF)
option(CJ_PI                "Builds a position-independant library." OFF)
option(CJ_STATIC_STD        "Links statically to the standard library." OFF)
option(CJ_SANITIZE          "Adds -fsanitize=address and -fsanitize=undefined flags." OFF)
option(CJ_NATIVE            "Adds -march=native flag." OFF)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) # Hmmm, not sure about that.

find_package(Threads REQUIRED)
find_package(Eigen REQUIRED)
find_package(Boost REQUIRED)
find_package(CUDA)
#find_package(BLAS)
#find_package(LAPACK)
#find_package(MKL) # FindMKL expects the environmental variable MKLROOT to point to the root of the library.
find_package(Math)

include_directories(. include ${Boost_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIR})

set(CJ_LIBS_TO_LINK ${CMAKE_THREAD_LIBS_INIT})# ${MATH_LIBRARIES})

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Select best backend for linear algebra:

#if(MKL_FOUND) # Add support for MKL
#  add_definitions(-DEIGEN_USE_MKL_ALL)
#  include_directories(MKL_INCLUDE_DIRS)
#  list(APPEND libs_to_link ${MKL_LIBRARIES})
#else()
  if(LAPACK_FOUND)
    add_definitions(-DEIGEN_USE_BLAS -DEIGEN_USE_LAPACKE)
    list(APPEND libs_to_link ${LAPACK_LIBRARIES})
  elseif(BLAS_FOUND)
    add_definitions(-DEIGEN_USE_BLAS)
    list(APPEND libs_to_link ${BLAS_LIBRARIES})
  endif()
#endif()

# Optional flags:

if(CJ_STATIC_STD)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++ -static-libgcc")
endif()

if(CJ_SANITIZE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined")
endif()

if(CJ_NATIVE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

# Add subdirectories:

if(CJ_BENCH)
  add_subdirectory(bench)
endif()

if(CJ_TEST)
  enable_testing()
  add_subdirectory(test)
endif()

if(CJ_EXAMPLES)
  add_subdirectory(examples)
endif()

if(CJ_TEST OR CJ_EXAMPLES)
  file(COPY data DESTINATION .)
endif()

if(CJ_PYTHON)
  find_package(Python)
  if(CJ_STATIC AND NOT CJ_PI)
    message(ERROR "Must have the CJ_PI option ON to build the python module with a static library.")
  else()
    add_subdirectory(python)
  endif()
endif()

# Summary:

message(STATUS "")
message(STATUS "CONJUNCTION BUILD SUMMARY")
message(STATUS "  CMAKE_GENERATOR                  : ${CMAKE_GENERATOR}")
message(STATUS "  Build type                       : ${CMAKE_BUILD_TYPE}")
message(STATUS "  Compiler ID                      : ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "  Tests                            : ${CJ_TEST}")
message(STATUS "  Examples                         : ${CJ_EXAMPLES}")
message(STATUS "  Benchmarks                       : ${CJ_BENCH}")
message(STATUS "  Python module                    : ${CJ_PYTHON}")
message(STATUS "  Position-independant library     : ${CJ_PI}")
message(STATUS "  Link statically to stdlibc++     : ${CJ_STATIC_STD}")
message(STATUS "  Sanitize flags                   : ${CJ_SANITIZE}")
message(STATUS "  Eigen3's path                    : ${EIGEN_INCLUDE_DIR}")
message(STATUS "  Libs to link                     : ${libs_to_link}")
message(STATUS "  C++ flags                        : ${CMAKE_CXX_FLAGS}")
if(CMAKE_BUILD_TYPE MATCHES RELEASE)
  message(STATUS "  C++'s release flags              : ${CMAKE_CXX_FLAGS_RELEASE}")
elseif(CMAKE_BUILD_TYPE MATCHES DEBUG)
  message(STATUS "  C++'s debug flags                : ${CMAKE_CXX_FLAGS_DEBUG}")
endif()
message(STATUS "")
