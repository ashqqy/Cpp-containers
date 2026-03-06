#pragma once

#include <gtest/gtest.h>

#include <stdexcept>

#include "common.hpp"
#include "signature.hpp"

struct TestException : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

struct ThrowingInt {
    ThrowingInt() { ThrowIfNeed(); }
    ThrowingInt(int value) : value_(value) { ThrowIfNeed(); }

    ThrowingInt(const ThrowingInt& other) : value_(other.value_) { ThrowIfNeed(); }
    ThrowingInt& operator=(const ThrowingInt& other) {
        if (this != &other) {
            ThrowIfNeed();
            value_ = other.value_;
        }
        return *this;
    }

    bool operator==(const ThrowingInt& other) const = default;
    auto operator<=>(const ThrowingInt& other) const = default;

    static void ThrowAt(std::size_t count) { throws_at = count; }

    static void ThrowNext() { throws_at = 1; }

    static void Nothrow() { throws_at = 0; }

    static void ThrowRandom() {
        const int kLimit = 75;
        if (GenerateRandomInt(0, 100) < kLimit) { ThrowNext(); }
    }

    static bool WillNextThrow() { return throws_at < 1; }

  private:
    static void ThrowIfNeed() {
        if (throws_at == 0) { return; }
        --throws_at;
        if (throws_at == 0) { Throw(); }
    }

    static void Throw() { throw TestException("Throwing int exception"); }

    inline static std::size_t throws_at = 0;

  private:
    int value_ = 0;
};

using Vector = signature::VectorT<ThrowingInt>;

TEST(ExceptionInType, Construction_CountElement_AtFirst) {
    std::size_t size = 10;
    ThrowingInt value(10);

    ThrowingInt::ThrowNext();

    EXPECT_THROW(Vector vec(size, value), TestException);

    ThrowingInt::Nothrow();
}

TEST(ExceptionInType, Construction_CountElement_InMiddle) {
    std::size_t size = 10;
    ThrowingInt value(10);

    ThrowingInt::ThrowAt(size / 2);

    EXPECT_THROW(Vector vec(size, value), TestException);

    ThrowingInt::Nothrow();
}

TEST(ExceptionInType, Construction_CountElement_AtLast) {
    std::size_t size = 10;
    ThrowingInt value(10);

    ThrowingInt::ThrowAt(size);

    EXPECT_THROW(Vector vec(size, value), TestException);

    ThrowingInt::Nothrow();
}

TEST(ExceptionInType, Construction_Copy_AtFirst) {
    std::size_t size = 10;
    ThrowingInt value(10);

    Vector vec(size, value);

    ThrowingInt::ThrowNext();

    EXPECT_THROW(Vector copy = vec, TestException);

    ThrowingInt::Nothrow();
}

TEST(ExceptionInType, Construction_Copy_InMiddle) {
    std::size_t size = 10;
    ThrowingInt value(10);

    Vector vec(size, value);

    ThrowingInt::ThrowAt(size / 2);

    EXPECT_THROW(Vector copy = vec, TestException);

    ThrowingInt::Nothrow();
}

TEST(ExceptionInType, Construction_Copy_AtLast) {
    std::size_t size = 10;
    ThrowingInt value(10);

    Vector vec(size, value);

    ThrowingInt::ThrowAt(size);

    EXPECT_THROW(Vector copy = vec, TestException);

    ThrowingInt::Nothrow();
}

TEST(ExceptionInType, Assignment_SameSize) {
    std::size_t size = 10;
    ThrowingInt value(1);
    ThrowingInt other_value(2);

    Vector vec(size, value);
    Vector vec_other(size, other_value);

    ThrowingInt::ThrowAt(size / 2);

    EXPECT_THROW(vec = vec_other, TestException);

    ThrowingInt::Nothrow();

    EXPECT_EQ(vec, Vector(size, value));
}

TEST(ExceptionInType, Assignment_Bigger) {
    std::size_t size = 10;
    ThrowingInt value(10);
    ThrowingInt other_value(20);

    Vector vec(size, value);
    Vector vec_other(size * size, other_value);

    ThrowingInt::ThrowAt(size * 4);

    EXPECT_THROW(vec = vec_other, TestException);

    ThrowingInt::Nothrow();

    EXPECT_EQ(vec, Vector(size, value));
}

TEST(ExceptionInType, Assignment_UnreacheableThrow) {
    std::size_t size = 10;
    ThrowingInt value(10);
    ThrowingInt other_value(20);

    Vector vec(size, value);
    Vector vec_other(size * 2, other_value);

    ThrowingInt::ThrowAt(4 * size + 1);

    EXPECT_NO_THROW(vec = vec_other);

    ThrowingInt::Nothrow();

    EXPECT_EQ(vec, vec_other);
}

TEST(ExceptionInType, Resize) {
    std::size_t size = 10;
    ThrowingInt value(10);
    ThrowingInt other_value(20);

    Vector vec(size, value);

    ThrowingInt::ThrowAt(size * 2);

    EXPECT_THROW(vec.resize(size * 3), TestException);

    ThrowingInt::Nothrow();

    EXPECT_EQ(vec, Vector(size, value));
}

TEST(ExceptionInType, Reserve) {
    std::size_t size = 10;
    ThrowingInt value(10);

    Vector vec(size, value);

    ThrowingInt::ThrowAt(size / 2);

    EXPECT_THROW(vec.reserve(vec.capacity() * 3), TestException);

    ThrowingInt::Nothrow();

    EXPECT_EQ(vec, Vector(size, value));
}

TEST(ExceptionInType, PushBackNoRealloc) {
    std::size_t size = 10;
    ThrowingInt value(10);
    ThrowingInt other_value(20);

    Vector vec(size, value);
    vec.reserve(size * 2);

    ThrowingInt::ThrowNext();

    EXPECT_THROW(vec.push_back(other_value), TestException);

    ThrowingInt::Nothrow();

    EXPECT_EQ(vec, Vector(size, value));
}

TEST(ExceptionInType, PushBackRealloc) {
    std::size_t size = 10;
    ThrowingInt value(10);
    ThrowingInt other_value(20);

    Vector vec(size, value);
    vec.reserve(size * 2);

    std::size_t capacity = vec.capacity();

    while (vec.size() != capacity) {
        vec.push_back(other_value);
    }

    if (capacity != vec.capacity()) {
        EXPECT_TRUE(true);
        return;
    }

    Vector copy = vec;

    ThrowingInt::ThrowNext();
    EXPECT_THROW(vec.push_back(other_value), TestException);

    ThrowingInt::Nothrow();
    EXPECT_EQ(vec, copy);
}

TEST(ExceptionInType, PushBackRandom) {
    std::size_t iterations = 5;
    std::size_t steps_in_iteration = 10000;

    for (std::size_t i = 0; i < iterations; ++i) {
        Vector expected;
        Vector vector;

        for (std::size_t step = 0; step < steps_in_iteration; ++step) {
            auto value = ThrowingInt(GenerateRandomInt(0, 100));

            ThrowingInt::ThrowRandom();
            try {
                vector.push_back(value);
            } catch (TestException&) { 
                continue; 
            }

            ThrowingInt::Nothrow();

            expected.push_back(value);
        }

        EXPECT_EQ(expected, vector);
    }
}
