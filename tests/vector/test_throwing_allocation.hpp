#pragma once

#include <gtest/gtest.h>

#include "common.hpp"
#include "signature.hpp"
#include "types_for_test.hpp"


std::size_t throws_at = 0;

struct AllocException : public std::bad_alloc {};

inline void ThrowAt(std::size_t count) {
    throws_at = count;
}

inline void ThrowNext() {
    throws_at = 1;
}

inline void Nothrow() {
    throws_at = 0;
}

inline void ThrowRandom() {
    const int kLimit = 75;
    if (GenerateRandomInt(0, 100) < kLimit) { ThrowNext(); }
}


void* operator new(std::size_t size, const std::nothrow_t&) noexcept {
    return malloc(size);
}

void* operator new(std::size_t size) {
    if (throws_at == 0) { return malloc(size); }
    --throws_at;
    if (throws_at == 0) { throw AllocException{}; }
    return malloc(size);
}

void* operator new[](std::size_t size) {
    if (throws_at == 0) { return malloc(size); }
    --throws_at;
    if (throws_at == 0) { throw AllocException{}; }
    return malloc(size);
}

void operator delete(void* data) noexcept {
  free(data);
}

void operator delete(void* data, std::size_t /* unused */) noexcept {
  free(data);
}

void operator delete[](void* data) noexcept {
  free(data);
}

void operator delete[](void* data, std::size_t /* unused */) noexcept {
  free(data);
}


template <typename T>
using VectorT = signature::VectorT<T>;

template <typename T>
class ExceptionSafety : public ::testing::Test {};

TYPED_TEST_SUITE(ExceptionSafety, TestTypes);

TYPED_TEST(ExceptionSafety, ConstructionCount) {
    std::size_t size = 10;
    ThrowNext();
    EXPECT_THROW(VectorT<TypeParam> vec(size), AllocException);
}

TYPED_TEST(ExceptionSafety, ConstructionCopy) {
    std::size_t size = 10;
    VectorT<TypeParam> vec(size);
    ThrowNext();
    EXPECT_THROW(VectorT<TypeParam> copy = vec, AllocException);
}

TYPED_TEST(ExceptionSafety, AssignmentCopy) {
    std::size_t size = 10;

    VectorT<TypeParam> vec(size);
    VectorT<TypeParam> other;

    ThrowNext();
    EXPECT_THROW(other = vec, AllocException);
    Nothrow();

    EXPECT_EQ(other, VectorT<TypeParam>());
}

TYPED_TEST(ExceptionSafety, Resize) {
    std::size_t size = 10;
    VectorT<TypeParam> vec(size);
    VectorT<TypeParam> vec_copy = vec;

    ThrowNext();
    EXPECT_THROW(vec.resize(vec.capacity() * 2), AllocException);
    Nothrow();

    EXPECT_EQ(vec, vec_copy);
}

TYPED_TEST(ExceptionSafety, Reserve) {
    std::size_t size = 10;
    VectorT<TypeParam> vec(size);
    VectorT<TypeParam> vec_copy = vec;

    ThrowNext();
    EXPECT_THROW(vec.reserve(vec.capacity() * 2), AllocException);
    Nothrow();

    EXPECT_EQ(vec, vec_copy);
}

TYPED_TEST(ExceptionSafety, PushBack) {
    std::size_t iterations = 5;
    std::size_t steps_in_iteration = 10000;

    for (std::size_t i = 0; i < iterations; ++i) {
        VectorT<TypeParam> vector;

        std::size_t errors_count = 0;

        for (std::size_t step = 0; step < steps_in_iteration; ++step) {
            ThrowRandom();

            try {
                vector.push_back(TypeParam());
            } catch (AllocException&) { ++errors_count; }

            Nothrow();
        }
        
        VectorT<TypeParam> expected(steps_in_iteration - errors_count);

        EXPECT_EQ(expected, vector);
    }
}
