add_subdirectory(gtest)

include_directories(./gtest/googletest/include)

set(test_src run_all.cc
  logics/fuzzy_classifier_spec.cc
  data/data_matrix_spec.cc
  math/truth_spec.cc
  math/confusion_spec.cc
  math/set_spec.cc
  utils/top_n_map_spec.cc
  utils/top_n_set_spec.cc
)

add_executable(test_cj ${test_src})

target_link_libraries(test_cj
  gtest
  ${CJ_LIBS_TO_LINK}
  ${CMAKE_THREAD_LIBS_INIT}
  ${MATH_LIBS}
)

add_test(NAME tests COMMAND test_cj)
