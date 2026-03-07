#include <gtest/gtest.h>

#include "signature.hpp"

template <typename T>
using VectorT = signature::VectorT<T>;

struct NoexceptMoveTracker {
    static inline int copy_count = 0;
    static inline int move_count = 0;

    NoexceptMoveTracker() = default;
    
    NoexceptMoveTracker(const NoexceptMoveTracker&) { ++copy_count; }
    
    NoexceptMoveTracker(NoexceptMoveTracker&&) noexcept { ++move_count; }

    static void reset_counters() {
        copy_count = 0;
        move_count = 0;
    }
};

struct ThrowingMoveTracker {
    static inline int copy_count = 0;
    static inline int move_count = 0;

    ThrowingMoveTracker() = default;
    
    ThrowingMoveTracker(const ThrowingMoveTracker&) { ++copy_count; }
    
    ThrowingMoveTracker(ThrowingMoveTracker&&) { ++move_count; }

    static void reset_counters() {
        copy_count = 0;
        move_count = 0;
    }
};


TEST(VectorReallocationTest, MovesWhenNoexcept) {
    NoexceptMoveTracker::reset_counters();
    
    VectorT<NoexceptMoveTracker> vec;
    vec.reserve(2);
    
    NoexceptMoveTracker item;
    vec.push_back(item); 
    vec.push_back(item); 
    EXPECT_EQ(NoexceptMoveTracker::copy_count, 2);
    EXPECT_EQ(NoexceptMoveTracker::move_count, 0);
    
    NoexceptMoveTracker::reset_counters();
    
    vec.reserve(4);
    
    EXPECT_EQ(NoexceptMoveTracker::move_count, 2) << "Elements should be moved!";
    EXPECT_EQ(NoexceptMoveTracker::copy_count, 0) << "No copies should be made during reallocation!";
}

TEST(VectorReallocationTest, CopiesWhenThrowingMove) {
    ThrowingMoveTracker::reset_counters();
    
    VectorT<ThrowingMoveTracker> vec;
    vec.reserve(2);
    
    vec.push_back(ThrowingMoveTracker{});
    vec.push_back(ThrowingMoveTracker{});
    EXPECT_EQ(NoexceptMoveTracker::copy_count, 0) << "No copies should be made during pushing rvalue!";
    EXPECT_EQ(NoexceptMoveTracker::move_count, 2) << "Elements should be moved!";
    
    ThrowingMoveTracker::reset_counters();
    
    vec.reserve(4);
    
    EXPECT_EQ(ThrowingMoveTracker::copy_count, 2) << "Elements must be copied for Exception Safety!";
    EXPECT_EQ(ThrowingMoveTracker::move_count, 0) << "Elements should not be moved!";
}
