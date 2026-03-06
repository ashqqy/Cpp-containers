#pragma once

#include <gtest/gtest.h>

#include "signature.hpp"
#include "types_for_test.hpp"

template <typename T>
using VectorT = signature::VectorT<T>;

template <typename T>
class ThrowingMethods : public ::testing::Test {};

TYPED_TEST_SUITE(ThrowingMethods, TestTypes);

TYPED_TEST(ThrowingMethods, At_NonConst) {
  std::size_t size = 10;
  VectorT<TypeParam> vec(size);

  EXPECT_THROW(vec.at(size), std::out_of_range);
  EXPECT_THROW(vec.at(size * 2), std::out_of_range);
  EXPECT_NO_THROW(vec.at(size - 1));
  EXPECT_NO_THROW(vec.at(0));
}

TYPED_TEST(ThrowingMethods, At_Const) {
  std::size_t size = 10;
  const VectorT<TypeParam> vec(size);

  EXPECT_THROW(vec.at(size), std::out_of_range);
  EXPECT_THROW(vec.at(size * 2), std::out_of_range);
  EXPECT_NO_THROW(vec.at(size - 1));
  EXPECT_NO_THROW(vec.at(0));
}
