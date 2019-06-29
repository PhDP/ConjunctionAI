#include "gtest/gtest.h"
#include "cj/data/data_matrix.hh"

TEST(CJDataMatrix, CreatesDataMatrix) {
  auto const d = cj::data_matrix_from_file<double>("../data/poll_plant/poll.csv");

  EXPECT_EQ(8, d->ncols());
  EXPECT_EQ(1496, d->nrows());
  EXPECT_EQ(std::string{"PlantFlowerWidth"}, d->header(3));
  EXPECT_EQ(std::string{"Interaction"}, d->header(7));
//  REQUIRE(0.08692307692307692 == Approx(d->operator()(4, 3)));
}


