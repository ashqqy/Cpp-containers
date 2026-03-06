#pragma once

#include <gtest/gtest.h>

#include <iterator>
#include <numeric>

#include "signature.hpp"
#include "test_iterators.hpp"

using VectorInt = signature::VectorInt;

struct IncreasingSequenceProvider {
    static VectorInt Create() {
        VectorInt vector;
        for (std::size_t i = 0; i < kLength; ++i) {
            vector.push_back(i);
        }
        return vector;
    }

    static std::vector<int> ExpectedSequence() {
        std::vector<int> expected;
        for (std::size_t i = 0; i < kLength; ++i) {
            expected.push_back(i);
        }
        return expected;
    }

    static const std::size_t kLength = 10;
};

// clang-format off

using IterTest = IteratorTest<
  /* ValueType = */ int,
  /* Container = */ VectorInt,
  /* ExpectedIteratorCategory = */ std::random_access_iterator_tag,
  /* SequenceProvider = */ IncreasingSequenceProvider>;

// clang-format on

TEST(IteratorTests, Interface) {
    IterTest::Run();
}

TEST(IteratorTests, NoInvalidationOnPopBack) {
    const std::size_t kSize = 1000;

    VectorInt vector;
    vector.resize(kSize);

    std::iota(vector.begin(), vector.end(), 0);

    auto begin = vector.begin();
    auto middle = begin + (kSize / 2);

    EXPECT_EQ(*begin, 0);
    EXPECT_EQ(*middle, 500);

    for (std::size_t i = 0; i < (kSize / 2) - 2; ++i) {
        vector.pop_back();
    }

    EXPECT_EQ(*begin, 0);
    EXPECT_EQ(*middle, 500);

    while (vector.size() != 1) {
        vector.pop_back();
    }

    EXPECT_EQ(*begin, 0);
}

TEST(IteratorTests, NoInvalidationOnResizeToLower) {
    const std::size_t kSize = 1000;

    VectorInt vector;
    vector.resize(kSize);

    std::iota(vector.begin(), vector.end(), 0);

    auto begin = vector.begin();
    auto middle = begin + kSize / 2;

    EXPECT_EQ(*begin, 0);
    EXPECT_EQ(*middle, 500);

    vector.resize((kSize / 2) + 10);

    EXPECT_EQ(*begin, 0);
    EXPECT_EQ(*middle, 500);

    vector.resize(1);

    EXPECT_EQ(*begin, 0);
}
