#pragma once

#include <gtest/gtest.h>

#include <vector>

#include "signature.hpp"
#include "common.hpp"

using VectorInt = signature::VectorInt;

TEST(Constructors, Default)
{
    VectorInt vector;
    ASSERT_EQ(vector.size(), 0UL);
    ASSERT_TRUE(vector.empty());
}

TEST(Constructors, FromSize)
{
    size_t size = 100;
    VectorInt vector(size);
    for (size_t i = 0; i < size; ++i)
    {
        ASSERT_EQ(vector.at(i), 0);
    }
}

TEST(Constructors, FromSizeAndValue)
{
    size_t size = 100;
    VectorInt vector(size, 10);
    for (size_t i = 0; i < size; ++i)
    {
        ASSERT_EQ(vector.at(i), 10);
    }
}

TEST(Constructors, FromStaticArray)
{
    int arr[] = {1, 2, 3};
    VectorInt vector(arr, arr + 3);

    ASSERT_EQ(vector.at(0), 1);
    ASSERT_EQ(vector.at(1), 2);
    ASSERT_EQ(vector.at(2), 3);
}

TEST(Constructors, CopyConstructor)
{
    size_t size = 100;
    VectorInt vector(size, 10);
    VectorInt other(vector);

    for (size_t i = 0; i < size; ++i)
    {
        ASSERT_EQ(vector.at(i), other.at(i));
    }
}

TEST(Clear, Clear)
{
    size_t size = 100;
    VectorInt vector(size, 10);
    vector.clear();

    ASSERT_EQ(vector.size(), 0UL);
    ASSERT_TRUE(vector.empty());
}

TEST(PopBack, FromNonEmpty)
{
    int arr[] = {0, 1};
    VectorInt vector(arr, arr + 2);

    vector.pop_back();
    ASSERT_EQ(vector.size(), 1UL);

    vector.pop_back();
    ASSERT_EQ(vector.size(), 0UL);
}

TEST(Resize, ToLess)
{
    int arr[] = {0, 1, 2, 3, 4, 5, 6};
    VectorInt vector(arr, arr + 7);
    vector.resize(3);

    ASSERT_EQ(vector.size(), 3UL);
    ASSERT_EQ(vector.at(2), 2);
}

TEST(Resize, ToBigger)
{
    int arr[] = {0, 1};
    VectorInt vector(arr, arr + 2);
    vector.resize(10);

    ASSERT_EQ(vector.size(), 10UL);
    ASSERT_EQ(vector.at(7), 0);
}

TEST(Resize, ToZero)
{
    int arr[] = {0, 1};
    VectorInt vector(arr, arr + 2);
    vector.resize(0);

    EXPECT_TRUE(vector.empty());
}

TEST(Resize, ToBiggerWithVal)
{
    int arr[] = {0, 1};
    VectorInt vector(arr, arr + 2);
    vector.resize(10, 20);

    ASSERT_EQ(vector.size(), 10UL);
    ASSERT_EQ(vector.at(0), 0);
    ASSERT_EQ(vector.at(4), 20);
    ASSERT_EQ(vector.at(9), 20);
}

TEST(Reserve, ToLess)
{
    int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    VectorInt vector(arr, arr + 10);

    vector.reserve(1);
    ASSERT_EQ(vector.size(), 10UL);

    vector.shrink_to_fit();
    ASSERT_LE(vector.capacity(), vector.size() * 2);
}

TEST(Reserve, ToBigger)
{
    int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    VectorInt vector(arr, arr + 10);
    vector.reserve(100);
    ASSERT_EQ(vector.size(), 10UL);
    ASSERT_GE(vector.capacity(), 100UL);

    vector.shrink_to_fit();
    ASSERT_LE(vector.capacity(), vector.size() * 2);
}

TEST(ShrinkToFit, ShrinkToFit)
{
    int arr[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    VectorInt vector(arr, arr + 10);
    vector.shrink_to_fit();

    ASSERT_LE(vector.capacity(), 20UL);
    ASSERT_GE(vector.capacity(), 10UL);
    ASSERT_EQ(vector.size(), 10UL);
}

TEST(Swap, Two)
{
    int arr[] = {0, 0, 1, 0, 0};
    VectorInt first(arr, arr + 5);
    int brr[] = {0, 0, 2, 0, 0};
    VectorInt second(brr, brr + 5);
    first.swap(second);

    ASSERT_EQ(first.size(), second.size());
    ASSERT_EQ(first.at(2), 2);
    ASSERT_EQ(second.at(2), 1);
}

TEST(Swap, One)
{
    int arr[] = {0, 0, 1, 0, 0};
    VectorInt vector(arr, arr + 5);
    vector.swap(vector);

    ASSERT_EQ(vector.size(), vector.size());
    ASSERT_EQ(vector.at(2), 1);
}

TEST(At, NonConst)
{
    int arr[] = {0, 0, 1, 0, 0};
    VectorInt vector(arr, arr + 5);

    const bool is_ref = std::is_reference_v<decltype(vector.at(0))>;
    EXPECT_TRUE(is_ref);

    vector.at(0) = 10;
    const bool is_int = std::is_same_v<std::remove_reference_t<decltype(vector.at(0))>, int>;
    EXPECT_TRUE(is_int);
}

TEST(At, Const)
{
    int arr[] = {0, 0, 1, 0, 0};
    const VectorInt vector(arr, arr + 5);

    const bool is_const_ref = std::is_reference_v<decltype(vector.at(0))> &&
                              std::is_const_v<std::remove_reference_t<decltype(vector.at(0))>>;
    const bool is_not_ref = !std::is_reference_v<decltype(vector.at(0))>;

    EXPECT_TRUE(is_const_ref || is_not_ref);
}

TEST(FrontBack, Const)
{
    int arr[] = {10, 0, 1, 0, 20};
    const VectorInt vector(arr, arr + 5);

    bool is_const_ref = std::is_reference_v<decltype(vector.front())> &&
                        std::is_const_v<std::remove_reference_t<decltype(vector.front())>>;
    bool is_not_ref = !std::is_reference_v<decltype(vector.front())>;

    EXPECT_TRUE(is_const_ref || is_not_ref);
    EXPECT_EQ(vector.front(), 10);

    is_const_ref = std::is_reference_v<decltype(vector.back())> &&
                   std::is_const_v<std::remove_reference_t<decltype(vector.back())>>;
    is_not_ref = !std::is_reference_v<decltype(vector.back())>;

    EXPECT_TRUE(is_const_ref || is_not_ref);
    EXPECT_EQ(vector.back(), 20);
}

TEST(FrontBack, NonConst)
{
    int arr[] = {10, 0, 1, 0, 20};
    VectorInt vector(arr, arr + 5);

    bool is_ref = std::is_reference_v<decltype(vector.front())>;
    bool is_int = std::is_same_v<std::remove_reference_t<decltype(vector.front())>, int>;
    EXPECT_TRUE(is_ref && is_int);

    vector.front() = 15;
    EXPECT_EQ(vector.at(0), 15);

    is_ref = std::is_reference_v<decltype(vector.back())>;
    is_int = std::is_same_v<std::remove_reference_t<decltype(vector.back())>, int>;
    EXPECT_TRUE(is_ref && is_int);

    vector.back() = 25;
    EXPECT_EQ(vector.at(vector.size() - 1), 25);
}

TEST(Data, NonConst)
{
    int arr[] = {10, 0, 1, 0, 20};
    VectorInt vector(arr, arr + 5);
    bool are_same = std::is_same_v<decltype(vector.data()), int*>;

    EXPECT_TRUE(are_same);
}

TEST(Data, Const)
{
    int arr[] = {10, 0, 1, 0, 20};
    const VectorInt vector(arr, arr + 5);
    bool are_same = std::is_same_v<decltype(vector.data()), const int*>;

    EXPECT_TRUE(are_same);
}

TEST(Compare, Equal)
{
    int arr[] = {10, 0, 1, 0, 20};
    VectorInt v1(arr, arr + 5);
    VectorInt v2(arr, arr + 5);

    ASSERT_TRUE(v1.equal_to(v2));
}

TEST(Compare, NotEqual)
{
    int arr[] = {10, 0, 1, 0, 20};
    VectorInt v1(arr, arr + 5);
    int brr[] = {10, 0, 2, 0, 20};
    VectorInt v2(brr, brr + 5);

    ASSERT_FALSE(v1.equal_to(v2));
}

TEST(Compare, NotEqualDiffLen)
{
    int arr[] = {10, 0, 1, 0, 20};
    VectorInt v1(arr, arr + 5);
    int brr[] = {10, 0, 1, 0};
    VectorInt v2(brr, brr + 4);
    int crr[] = {10, 0, 1, 0, 20, 0};
    VectorInt v3(crr, crr + 6);

    ASSERT_FALSE(v1.equal_to(v2));
    ASSERT_FALSE(v1.equal_to(v3));
}

TEST(PushBack, ToExisting)
{
    int arr[] = {0, 1, 2};
    VectorInt vector(arr, arr + 3);
    vector.push_back(4);

    ASSERT_EQ(vector.size(), 4UL);
}

TEST(PushBack, ToDefault)
{
    VectorInt vector;
    vector.push_back(1);

    ASSERT_EQ(vector.size(), 1UL);
}

TEST(PushBack, Random)
{
    size_t iterations_count = 1 << 20;
    auto data = GenerateRandomInts(iterations_count, -100, 100);

    VectorInt real;
    for (int elem : data)
    {
        real.push_back(elem);
    }

    VectorInt expected(data.data(), data.data() + data.size());

    EXPECT_TRUE(real.equal_to(expected));
}
