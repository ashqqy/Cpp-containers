#pragma once

#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <type_traits>

#include "signature.hpp"
#include "types_for_test.hpp"

template <typename T>
using VectorT = signature::VectorT<T>;

template <typename T>
class VectorTestEnvironment : public ::testing::Test {
  protected:
    const VectorT<T> empty_{};
    const VectorT<T> single_{T(1)};
    const VectorT<T> ascending_{T(1), T(2), T(3)};
    const VectorT<T> descending_{T(3), T(2), T(1)};
    const VectorT<T> same_values_{T(5), T(5), T(5)};
    const VectorT<T> longer_{T(1), T(2), T(3), T(4)};
};

template <typename T>
class VectorComparison : public VectorTestEnvironment<T> {};

template <typename T>
class VectorCopyAssignment : public VectorTestEnvironment<T> {};

template <typename T>
class VectorConstructors : public VectorTestEnvironment<T> {};

template <typename T>
class VectorMethods : public VectorTestEnvironment<T> {};

// Run tests for all types in TestTypes
TYPED_TEST_SUITE(VectorComparison, TestTypes);
TYPED_TEST_SUITE(VectorCopyAssignment, TestTypes);
TYPED_TEST_SUITE(VectorConstructors, TestTypes);
TYPED_TEST_SUITE(VectorMethods, TestTypes);

TYPED_TEST(VectorComparison, EqualityOperator) {
    EXPECT_TRUE(this->empty_ == this->empty_);
    EXPECT_TRUE(this->single_ == this->single_);
    EXPECT_TRUE(this->ascending_ == this->ascending_);
    EXPECT_TRUE(this->same_values_ == this->same_values_);

    EXPECT_FALSE(this->empty_ == this->single_);
    EXPECT_FALSE(this->ascending_ == this->longer_);
    EXPECT_FALSE(this->single_ == this->same_values_);

    if constexpr (!std::is_same_v<TypeParam, bool>) {
        EXPECT_FALSE(this->ascending_ == this->descending_);
    }
}

TYPED_TEST(VectorComparison, InequalityOperator) {
    EXPECT_TRUE(this->empty_ != this->single_);
    EXPECT_TRUE(this->ascending_ != this->longer_);
    EXPECT_TRUE(this->single_ != this->same_values_);

    EXPECT_FALSE(this->empty_ != this->empty_);
    EXPECT_FALSE(this->single_ != this->single_);
    EXPECT_FALSE(this->ascending_ != this->ascending_);
    EXPECT_FALSE(this->same_values_ != this->same_values_);

    if constexpr (!std::is_same_v<TypeParam, bool>) {
        EXPECT_TRUE(this->ascending_ != this->descending_);
    }
}

TYPED_TEST(VectorComparison, LessThanOperator) {
    EXPECT_TRUE(this->empty_ < this->single_);
    EXPECT_TRUE(this->empty_ < this->ascending_);

    EXPECT_TRUE(this->ascending_ < this->longer_);
    EXPECT_FALSE(this->longer_ < this->ascending_);

    if constexpr (!std::is_same_v<TypeParam, bool>) {
        EXPECT_TRUE(this->ascending_ < this->descending_);
    }
    EXPECT_FALSE(this->descending_ < this->ascending_);

    EXPECT_FALSE(this->ascending_ < this->ascending_);
    EXPECT_FALSE(this->same_values_ < this->same_values_);

    const VectorT<TypeParam> short_same = {TypeParam(5), TypeParam(5)};
    EXPECT_TRUE(short_same < this->same_values_);
    EXPECT_FALSE(this->same_values_ < short_same);
}

TYPED_TEST(VectorComparison, LessThanOrEqualOperator) {
    EXPECT_TRUE(this->empty_ <= this->single_);
    EXPECT_TRUE(this->ascending_ <= this->longer_);
    EXPECT_TRUE(this->ascending_ <= this->descending_);

    EXPECT_TRUE(this->empty_ <= this->empty_);
    EXPECT_TRUE(this->ascending_ <= this->ascending_);
    EXPECT_TRUE(this->same_values_ <= this->same_values_);

    EXPECT_FALSE(this->single_ <= this->empty_);
    EXPECT_FALSE(this->longer_ <= this->ascending_);
    if constexpr (!std::is_same_v<TypeParam, bool>) {
        EXPECT_FALSE(this->descending_ <= this->ascending_);
    }
}

TYPED_TEST(VectorComparison, GreaterThanOperator) {
    EXPECT_TRUE(this->single_ > this->empty_);
    EXPECT_TRUE(this->ascending_ > this->empty_);

    EXPECT_TRUE(this->longer_ > this->ascending_);
    EXPECT_FALSE(this->ascending_ > this->longer_);

    if constexpr (!std::is_same_v<TypeParam, bool>) {
        EXPECT_TRUE(this->descending_ > this->ascending_);
    }
    EXPECT_FALSE(this->ascending_ > this->descending_);

    EXPECT_FALSE(this->ascending_ > this->ascending_);
    EXPECT_FALSE(this->same_values_ > this->same_values_);
}

TYPED_TEST(VectorComparison, GreaterThanOrEqualOperator) {
    EXPECT_TRUE(this->single_ >= this->empty_);
    EXPECT_TRUE(this->longer_ >= this->ascending_);
    EXPECT_TRUE(this->descending_ >= this->ascending_);

    EXPECT_TRUE(this->empty_ >= this->empty_);
    EXPECT_TRUE(this->ascending_ >= this->ascending_);
    EXPECT_TRUE(this->same_values_ >= this->same_values_);

    EXPECT_FALSE(this->empty_ >= this->single_);
    EXPECT_FALSE(this->ascending_ >= this->longer_);
    if constexpr (!std::is_same_v<TypeParam, bool>) {
        EXPECT_FALSE(this->ascending_ >= this->descending_);
    }
}

TEST(StringVectorComparisonTest, StringSpecificTests) {
    const VectorT<std::string> empty;
    const VectorT<std::string> a = {"a"};
    const VectorT<std::string> b = {"b"};
    const VectorT<std::string> ab = {"a", "b"};
    const VectorT<std::string> ba = {"b", "a"};
    const VectorT<std::string> abc = {"a", "b", "c"};

    EXPECT_TRUE(a < b);
    EXPECT_TRUE(a < ab);
    EXPECT_TRUE(ab < abc);
    EXPECT_TRUE(ab < ba);
}

TEST(NestedVectorComparisonTest, NestedVectorTests) {
    const VectorT<VectorT<int>> empty;
    const VectorT<VectorT<int>> v1 = {{1, 2}, {3, 4}};
    const VectorT<VectorT<int>> v2 = {{1, 2}, {3, 5}};
    const VectorT<VectorT<int>> v3 = {{1, 2}, {3, 4}, {5}};

    EXPECT_TRUE(empty < v1);
    EXPECT_TRUE(v1 < v2);
    EXPECT_TRUE(v1 < v3);
    EXPECT_FALSE(v3 < v1);
}

TYPED_TEST(VectorComparison, EdgeCases) {
    const VectorT<TypeParam> max_vec = {std::numeric_limits<TypeParam>::max()};
    const VectorT<TypeParam> kMinVec = {std::numeric_limits<TypeParam>::min()};

    if constexpr (std::is_arithmetic_v<TypeParam>) {
        EXPECT_TRUE(kMinVec < max_vec);
        EXPECT_TRUE(max_vec > kMinVec);
    }

    const VectorT<TypeParam> long_vec_1(1000, TypeParam(1));
    const VectorT<TypeParam> long_vec_2(1000, TypeParam(1));
    const VectorT<TypeParam> long_vec_3(1000, TypeParam(2));

    EXPECT_TRUE(long_vec_1 == long_vec_2);
    if constexpr (!std::is_same_v<TypeParam, bool>) {
        EXPECT_TRUE(long_vec_1 < long_vec_3);
        EXPECT_TRUE(long_vec_3 > long_vec_2);
    }

    VectorT<TypeParam> empty1;
    EXPECT_TRUE(empty1 == this->empty_);

    VectorT<TypeParam> empty2 = this->empty_;
    EXPECT_TRUE(empty2 == this->empty_);
}

TYPED_TEST(VectorComparison, DifferentLengths) {
    const VectorT<TypeParam> v1 = {TypeParam(1), TypeParam(2)};
    const VectorT<TypeParam> v2 = {TypeParam(1), TypeParam(2), TypeParam(3)};
    const VectorT<TypeParam> v3 = {TypeParam(1), TypeParam(3)};

    EXPECT_TRUE(v1 < v2);
    EXPECT_FALSE(v2 < v1);

    if constexpr (!std::is_same_v<TypeParam, bool>) { EXPECT_TRUE(v1 < v3); }
    EXPECT_FALSE(v3 < v1);
}

TYPED_TEST(VectorCopyAssignment, BasicUB) {
    {
        VectorT<TypeParam> copy;
        copy = this->ascending_;
        copy.clear();
    }
    EXPECT_EQ(this->ascending_, (VectorT<TypeParam>{TypeParam(1), TypeParam(2), TypeParam(3)}));
}

TYPED_TEST(VectorCopyAssignment, SameSize) {
    VectorT<TypeParam> copy;
    copy = this->ascending_;
    EXPECT_TRUE(copy == this->ascending_);
    EXPECT_EQ(copy.size(), this->ascending_.size());
}

TYPED_TEST(VectorCopyAssignment, ToLarger) {
    VectorT<TypeParam> copy = this->longer_;
    copy = this->single_;
    EXPECT_TRUE(copy == this->single_);
    EXPECT_EQ(copy.size(), this->single_.size());
}

TYPED_TEST(VectorCopyAssignment, ToSmaller) {
    VectorT<TypeParam> copy = this->single_;
    copy = this->longer_;
    EXPECT_TRUE(copy == this->longer_);
    EXPECT_EQ(copy.size(), this->longer_.size());
}

TYPED_TEST(VectorCopyAssignment, ToEmpty) {
    VectorT<TypeParam> copy;
    copy = this->ascending_;
    EXPECT_TRUE(copy == this->ascending_);
    EXPECT_EQ(copy.size(), this->ascending_.size());
}

TYPED_TEST(VectorCopyAssignment, FromEmpty) {
    VectorT<TypeParam> copy = this->ascending_;
    copy = this->empty_;
    EXPECT_TRUE(copy == this->empty_);
    EXPECT_TRUE(copy.empty());
}

TYPED_TEST(VectorCopyAssignment, Self) {
    VectorT<TypeParam> copy = this->ascending_;
    copy = copy;
    EXPECT_TRUE(copy == this->ascending_);
    EXPECT_EQ(copy.size(), this->ascending_.size());
}

TYPED_TEST(VectorCopyAssignment, Chain) {
    VectorT<TypeParam> copy1, copy2;
    copy2 = copy1 = this->same_values_;
    EXPECT_TRUE(copy1 == this->same_values_);
    EXPECT_TRUE(copy2 == this->same_values_);
    EXPECT_TRUE(copy1 == copy2);
}

TYPED_TEST(VectorCopyAssignment, WithEmpty) {
    VectorT<TypeParam> empty;
    empty = this->empty_;
    EXPECT_TRUE(empty == this->empty_);
}

TYPED_TEST(VectorCopyAssignment, SelfWithEmpty) {
    VectorT<TypeParam> empty;
    empty = empty;
    EXPECT_TRUE(empty == this->empty_);
}

TYPED_TEST(VectorConstructors, Default) {
    VectorT<TypeParam> s;
    EXPECT_EQ(s.size(), 0UL);
    EXPECT_TRUE(s.empty());
}

TYPED_TEST(VectorConstructors, SizeConstructor) {
    const size_t size = 100;
    VectorT<TypeParam> s(size);
    EXPECT_EQ(s.size(), size);
    for (size_t i = 0; i < size; ++i) {
        EXPECT_EQ(s.at(i), TypeParam{});
    }
}

TYPED_TEST(VectorConstructors, SizeValue) {
    const size_t size = 100;
    TypeParam value = TypeParam(10);
    VectorT<TypeParam> s(size, value);
    EXPECT_EQ(s.size(), size);
    for (size_t i = 0; i < size; ++i) {
        EXPECT_EQ(s.at(i), value);
    }
}

TYPED_TEST(VectorConstructors, Copy) {
    const size_t size = 100;
    TypeParam value = TypeParam(10);
    VectorT<TypeParam> s(size, value);
    VectorT<TypeParam> s1(s);

    EXPECT_EQ(s.size(), s1.size());
    for (size_t i = 0; i < size; ++i) {
        EXPECT_EQ(s.at(i), s1.at(i));
    }
}

TYPED_TEST(VectorMethods, Clear) {
    const size_t size = 100;
    TypeParam value = TypeParam(10);
    VectorT<TypeParam> s(size, value);
    s.clear();
    EXPECT_EQ(s.size(), 0UL);
    EXPECT_TRUE(s.empty());
}

TYPED_TEST(VectorMethods, PushBackToExisting) {
    TypeParam arr[] = {TypeParam(0), TypeParam(1), TypeParam(2)};
    VectorT<TypeParam> s(std::begin(arr), std::end(arr));
    s.push_back(TypeParam(4));
    EXPECT_EQ(s.size(), 4UL);
    EXPECT_EQ(s.back(), TypeParam(4));
}

TYPED_TEST(VectorMethods, PushBackToDefault) {
    VectorT<TypeParam> s;
    s.push_back(TypeParam(1));
    EXPECT_EQ(s.size(), 1UL);
    EXPECT_EQ(s.back(), TypeParam(1));
}

TYPED_TEST(VectorMethods, PopBackFromNonEmpty) {
    TypeParam arr[] = {TypeParam(0), TypeParam(1)};
    VectorT<TypeParam> s(std::begin(arr), std::end(arr));
    s.pop_back();
    EXPECT_EQ(s.size(), 1UL);
    EXPECT_EQ(s.back(), TypeParam(0));
    s.pop_back();
    EXPECT_EQ(s.size(), 0UL);
    EXPECT_TRUE(s.empty());
}

TYPED_TEST(VectorMethods, ResizeToSmaller) {
    TypeParam arr[] = {TypeParam(0), TypeParam(1), TypeParam(2), TypeParam(3),
                       TypeParam(4), TypeParam(5), TypeParam(6)};
    VectorT<TypeParam> s(std::begin(arr), std::end(arr));
    s.resize(3);
    EXPECT_EQ(s.size(), 3UL);
    EXPECT_EQ(s.at(2), TypeParam(2));
}

TYPED_TEST(VectorMethods, ResizeToBigger) {
    TypeParam arr[] = {TypeParam(0), TypeParam(1)};
    VectorT<TypeParam> s(std::begin(arr), std::end(arr));
    s.resize(10);
    EXPECT_EQ(s.size(), 10UL);
    EXPECT_EQ(s.at(0), TypeParam(0));
    EXPECT_EQ(s.at(1), TypeParam(1));
}

TYPED_TEST(VectorMethods, ResizeToZero) {
    TypeParam arr[] = {TypeParam(0), TypeParam(1)};
    VectorT<TypeParam> s(std::begin(arr), std::end(arr));
    s.resize(0);
    EXPECT_TRUE(s.empty());
}

TYPED_TEST(VectorMethods, ResizeToBiggerWithValue) {
    TypeParam arr[] = {TypeParam(0), TypeParam(1)};
    VectorT<TypeParam> s(std::begin(arr), std::end(arr));
    TypeParam value = TypeParam(20);
    s.resize(10, value);
    EXPECT_EQ(s.size(), 10UL);
    EXPECT_EQ(s.at(0), TypeParam(0));
    EXPECT_EQ(s.at(1), TypeParam(1));
    EXPECT_EQ(s.at(4), value);
    EXPECT_EQ(s.at(9), value);
}

TYPED_TEST(VectorMethods, ReserveToLess) {
    TypeParam arr[] = {TypeParam(0), TypeParam(1), TypeParam(2), TypeParam(3), TypeParam(4),
                       TypeParam(5), TypeParam(6), TypeParam(7), TypeParam(8), TypeParam(9)};
    VectorT<TypeParam> s(std::begin(arr), std::end(arr));
    size_t old_capacity = s.capacity();
    s.reserve(1);
    EXPECT_EQ(s.size(), 10UL);
    EXPECT_GE(s.capacity(), old_capacity);
}

TYPED_TEST(VectorMethods, ReserveToBigger) {
    TypeParam arr[] = {TypeParam(0), TypeParam(1), TypeParam(2), TypeParam(3), TypeParam(4),
                       TypeParam(5), TypeParam(6), TypeParam(7), TypeParam(8), TypeParam(9)};
    VectorT<TypeParam> s(std::begin(arr), std::end(arr));
    s.reserve(100);
    EXPECT_EQ(s.size(), 10UL);
    EXPECT_GE(s.capacity(), 100UL);
}

TYPED_TEST(VectorMethods, Swap) {
    TypeParam arr[] = {TypeParam(0), TypeParam(0), TypeParam(1), TypeParam(0), TypeParam(0)};
    VectorT<TypeParam> s(std::begin(arr), std::end(arr));
    TypeParam brr[] = {TypeParam(0), TypeParam(0), TypeParam(2), TypeParam(0), TypeParam(0)};
    VectorT<TypeParam> t(std::begin(brr), std::end(brr));

    s.swap(t);
    EXPECT_EQ(s.size(), 5UL);
    EXPECT_EQ(t.size(), 5UL);
    EXPECT_EQ(s.at(2), TypeParam(2));
    EXPECT_EQ(t.at(2), TypeParam(1));
}

TYPED_TEST(VectorMethods, SwapSelf) {
    TypeParam arr[] = {TypeParam(0), TypeParam(0), TypeParam(1), TypeParam(0), TypeParam(0)};
    VectorT<TypeParam> s(std::begin(arr), std::end(arr));
    s.swap(s);
    EXPECT_EQ(s.size(), 5UL);
    EXPECT_EQ(s.at(2), TypeParam(1));
}

TYPED_TEST(VectorMethods, AccessNonConst) {
    TypeParam arr[] = {TypeParam(0), TypeParam(0), TypeParam(1), TypeParam(0), TypeParam(0)};
    VectorT<TypeParam> s(std::begin(arr), std::end(arr));

    if constexpr (!std::is_same_v<TypeParam, bool>) {
        const bool is_ref_at = std::is_reference_v<decltype(s.at(0))>;
        const bool is_ref_op = std::is_reference_v<decltype(s[0])>;
        EXPECT_TRUE(is_ref_at);
        EXPECT_TRUE(is_ref_op);
    }

    s.at(0) = TypeParam(10);
    EXPECT_EQ(s.at(0), TypeParam(10));
    s[1] = TypeParam(10);
    EXPECT_EQ(s[0], TypeParam(10));
}

TYPED_TEST(VectorMethods, AccessConst) {
    TypeParam arr[] = {TypeParam(0), TypeParam(0), TypeParam(1), TypeParam(0), TypeParam(0)};
    const VectorT<TypeParam> s(std::begin(arr), std::end(arr));

    const bool is_const_ref_at = std::is_reference_v<decltype(s.at(0))> &&
                                 std::is_const_v<std::remove_reference_t<decltype(s.at(0))>>;
    const bool is_not_ref_at = !std::is_reference_v<decltype(s.at(0))>;
    EXPECT_TRUE(is_const_ref_at || is_not_ref_at);

    const bool is_const_ref_op = std::is_reference_v<decltype(s[0])> &&
                                 std::is_const_v<std::remove_reference_t<decltype(s[0])>>;
    const bool is_not_ref_op = !std::is_reference_v<decltype(s[0])>;
    EXPECT_TRUE(is_const_ref_op || is_not_ref_op);
}

TYPED_TEST(VectorMethods, FrontBackConst) {
    TypeParam arr[] = {TypeParam(10), TypeParam(0), TypeParam(1), TypeParam(0), TypeParam(20)};
    const VectorT<TypeParam> s(std::begin(arr), std::end(arr));

    bool is_const_ref = std::is_reference_v<decltype(s.front())> &&
                        std::is_const_v<std::remove_reference_t<decltype(s.front())>>;
    bool is_not_ref = !std::is_reference_v<decltype(s.front())>;
    EXPECT_TRUE(is_const_ref || is_not_ref);
    EXPECT_EQ(s.front(), TypeParam(10));

    is_const_ref = std::is_reference_v<decltype(s.back())> &&
                   std::is_const_v<std::remove_reference_t<decltype(s.back())>>;
    is_not_ref = !std::is_reference_v<decltype(s.back())>;
    EXPECT_TRUE(is_const_ref || is_not_ref);
    EXPECT_EQ(s.back(), TypeParam(20));
}

TYPED_TEST(VectorMethods, FrontBackNonConst) {
    TypeParam arr[] = {TypeParam(10), TypeParam(0), TypeParam(1), TypeParam(0), TypeParam(20)};
    VectorT<TypeParam> s(std::begin(arr), std::end(arr));
    if constexpr (!std::is_same_v<TypeParam, bool>) {
        bool is_ref = std::is_reference_v<decltype(s.front())>;
        EXPECT_TRUE(is_ref);
    }
    s.front() = TypeParam(15);
    EXPECT_EQ(s.at(0), TypeParam(15));
    if constexpr (!std::is_same_v<TypeParam, bool>) {
        bool is_ref = std::is_reference_v<decltype(s.back())>;
        EXPECT_TRUE(is_ref);
    }
    s.back() = TypeParam(25);
    EXPECT_EQ(s.at(s.size() - 1), TypeParam(25));
}

TYPED_TEST(VectorMethods, DataNonConst) {
    if constexpr (!std::is_same_v<TypeParam, bool>) {
        TypeParam arr[] = {TypeParam(10), TypeParam(0), TypeParam(1), TypeParam(0), TypeParam(20)};
        VectorT<TypeParam> s(std::begin(arr), std::end(arr));

        using ValueType = typename VectorT<TypeParam>::value_type;
        bool are_same = std::is_same_v<decltype(s.data()), ValueType*>;
        EXPECT_TRUE(are_same);
    }
}

TYPED_TEST(VectorMethods, DataConst) {
    if constexpr (!std::is_same_v<TypeParam, bool>) {
        TypeParam arr[] = {TypeParam(10), TypeParam(0), TypeParam(1), TypeParam(0), TypeParam(20)};
        const VectorT<TypeParam> s(std::begin(arr), std::end(arr));

        using ValueType = typename VectorT<TypeParam>::value_type;
        bool are_same = std::is_same_v<decltype(s.data()), const ValueType*>;
        EXPECT_TRUE(are_same);
    }
}

TYPED_TEST(VectorMethods, SizeCapacityUnsigned) {
    VectorT<TypeParam> s;
    EXPECT_TRUE(std::is_unsigned_v<decltype(s.size())>);
    EXPECT_TRUE(std::is_unsigned_v<decltype(s.capacity())>);
}

TEST(DeductionGuides, FromInitializerList) {
    VectorT v{1, 2, 3, 4, 5};

    EXPECT_TRUE((std::is_same_v<decltype(v), VectorT<int>>));
    EXPECT_EQ(v.size(), 5UL);
    EXPECT_EQ(v[0], 1);
    EXPECT_EQ(v[4], 5);
}

TEST(DeductionGuides, FromSizeAndValue) {
    VectorT v(5, 3.14);

    EXPECT_TRUE((std::is_same_v<decltype(v), VectorT<double>>));
    EXPECT_EQ(v.size(), 5UL);
    for (size_t i = 0; i < v.size(); ++i) {
        EXPECT_DOUBLE_EQ(v[i], 3.14);
    }
}

TEST(DeductionGuides, CopyDeduction) {
    VectorT<int> original{1, 2, 3};

    VectorT copy(original);

    EXPECT_TRUE((std::is_same_v<decltype(copy), VectorT<int>>));
    EXPECT_EQ(copy.size(), 3UL);
    EXPECT_EQ(copy[0], 1);
}

TEST(DeductionGuides, WithCStrings) {
    VectorT v{"hello", "world"};

    EXPECT_TRUE((std::is_same_v<decltype(v), VectorT<const char*>>));
    EXPECT_EQ(v.size(), 2UL);
    EXPECT_STREQ(v[0], "hello");
    EXPECT_STREQ(v[1], "world");
}

TEST(DeductionGuides, NestedVectors) {
    VectorT v{VectorT{1, 2, 3}, VectorT{4, 5, 6}};

    EXPECT_TRUE((std::is_same_v<decltype(v), VectorT<VectorT<int>>>));
    EXPECT_EQ(v.size(), 2UL);
    EXPECT_EQ(v[0].size(), 3UL);
    EXPECT_EQ(v[1].size(), 3UL);
}

TEST(DeductionGuides, SingleElement) {
    VectorT v{3.14};

    EXPECT_TRUE((std::is_same_v<decltype(v), VectorT<double>>));
    EXPECT_EQ(v.size(), 1UL);
    EXPECT_DOUBLE_EQ(v[0], 3.14);
}
