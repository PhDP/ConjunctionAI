add_subdirectory(gtest)

include_directories(./gtest/googletest/include)

set(test_src run_all.cc
  fuzzy_classifier_spec.cc
  truth_spec.cc
  data/data_matrix_spec.cc
  stat/confusion_spec.cc
)

add_executable(test_cj ${test_src})

target_link_libraries(test_cj
  gtest
  ${CJ_LIBS_TO_LINK}
  ${CMAKE_THREAD_LIBS_INIT}
  ${MATH_LIBS}
)

add_test(NAME tests COMMAND test_cj)
