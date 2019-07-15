#include "gtest/gtest.h"
#include "cj/data/data_matrix.hh"

TEST(CJDataMatrix, CreatesDataMatrix) {
  auto const d = cj::data_matrix<double, size_t>::from_file("../data/poll_plant/poll.csv");

  EXPECT_EQ(7, d->ncols());
  EXPECT_EQ(1496, d->nrows());
  EXPECT_EQ(std::string{"PlantFlowerWidth"}, d->input_name(3));
  EXPECT_EQ(std::string{"Interaction"}, d->output_name());
  EXPECT_DOUBLE_EQ(0.4292307692307692, d->operator()(4, 3));
  EXPECT_DOUBLE_EQ(1, d->get_output(0));
}
