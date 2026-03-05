#pragma once

#include <gtest/gtest.h>

#include <type_traits>

#include "signature.hpp"

using VectorBool = signature::VectorBool;

TEST(VectorBoolTest, BitPacking) {
  VectorBool vector;

  for (int i = 0; i < 100; ++i) {
    vector.push_back(i % 2 == 0);
  }
  EXPECT_EQ(vector.size(), 100UL);

  for (int i = 0; i < 100; ++i) {
    EXPECT_EQ(vector[i], i % 2 == 0);
  }
}

TEST(VectorBoolTest, FlipMethod) {
  VectorBool vector{true, false, true, false};
  vector.flip();

  EXPECT_EQ(vector.size(), 4UL);
  EXPECT_FALSE(vector[0]);
  EXPECT_TRUE(vector[1]);
  EXPECT_FALSE(vector[2]);
  EXPECT_TRUE(vector[3]);
}

TEST(VectorBoolTest, ReferenceProxy) {
  VectorBool vector{true, false};

  auto ref1 = vector[0];
  auto ref2 = vector[1];

  EXPECT_FALSE((std::is_same_v<decltype(ref1), bool>));

  ref1 = false;
  ref2 = true;

  EXPECT_FALSE(vector[0]);
  EXPECT_TRUE(vector[1]);
}

TEST(VectorBoolTest, ReferenceAssignment) {
  VectorBool vector{true, false, true};

  vector[0] = vector[1];
  EXPECT_FALSE(vector[0]);
  EXPECT_FALSE(vector[1]);

  vector[2] = true;
  EXPECT_TRUE(vector[2]);
}

TEST(VectorBoolTest, PushBackMany) {
  VectorBool vector;

  for (int i = 0; i < 1000; ++i) {
    vector.push_back((i % 3) == 0);
  }

  EXPECT_EQ(vector.size(), 1000UL);
  for (int i = 0; i < 1000; ++i) {
    EXPECT_EQ(vector[i], (i % 3) == 0);
  }
}

TEST(VectorBoolTest, BoolReferenceConversion) {
  VectorBool vector{true, false};

  bool b1 = vector[0];
  bool b2 = vector[1];

  EXPECT_TRUE(b1);
  EXPECT_FALSE(b2);

  EXPECT_TRUE(static_cast<bool>(vector[0]));
  EXPECT_FALSE(static_cast<bool>(vector[1]));

  EXPECT_FALSE(!static_cast<bool>(vector[0]));
  EXPECT_TRUE(!static_cast<bool>(vector[1]));
}

