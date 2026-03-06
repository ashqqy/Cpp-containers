#include <gtest/gtest.h>

#include "test_vector_int.hpp"
#include "test_vector_bool.hpp"
#include "test_vector_t.hpp"
#include "test_throwing_methods.hpp"

int main(int argc, char** argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
