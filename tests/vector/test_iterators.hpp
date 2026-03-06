#pragma once

#include <gtest/gtest.h>

#include <algorithm>
#include <concepts>
#include <iterator>
#include <random>
#include <type_traits>

template <typename ValueType, typename Container, typename ExpectedIteratorCategoryTag,
          typename SequenceProvider>
class IteratorTest {
  public:
    static void Run() {
        HasIteratorUsings();
        HasCorrectCategory();
        HasCorrectInterface();
        AllIterationsWork();
        AllAlgorithmsWorks();
        CanCast();
    }

  private:
    constexpr static bool kIsForward =
        std::derived_from<ExpectedIteratorCategoryTag, std::forward_iterator_tag>;
    constexpr static bool kIsBidirectional =
        std::derived_from<ExpectedIteratorCategoryTag, std::bidirectional_iterator_tag>;
    constexpr static bool kIsRandomAccess =
        std::derived_from<ExpectedIteratorCategoryTag, std::random_access_iterator_tag>;
    constexpr static bool kIsContiguous =
        std::derived_from<ExpectedIteratorCategoryTag, std::contiguous_iterator_tag>;

    /* ----------------------------------------------------------------------------------------- */

  private:
    static void HasIteratorUsings() {
        EXPECT_TRUE(HasForwardUsings());
        EXPECT_TRUE(!kIsBidirectional || HasReverseUsings());
    }

    constexpr static bool HasForwardUsings() {
        return requires {
            typename Container::iterator;
            typename Container::const_iterator;
        };
    }

    constexpr static bool HasReverseUsings() {
        return requires {
            typename Container::reverse_iterator;
            typename Container::const_reverse_iterator;
        };
    }

    /* ----------------------------------------------------------------------------------------- */

    static void HasCorrectCategory() {
        using Iterator = typename Container::iterator;
        using Category = typename std::iterator_traits<Iterator>::iterator_category;

        EXPECT_TRUE((std::derived_from<Category, ExpectedIteratorCategoryTag>));
    }

    /* ----------------------------------------------------------------------------------------- */

    static void HasCorrectInterface()
        requires kIsForward && (!kIsBidirectional)
    {
        EXPECT_TRUE(std::forward_iterator<typename Container::iterator>);
        EXPECT_TRUE(std::forward_iterator<typename Container::const_iterator>);
    }

    static void HasCorrectInterface()
        requires kIsBidirectional && (!kIsRandomAccess)
    {
        EXPECT_TRUE(std::bidirectional_iterator<typename Container::iterator>);
        EXPECT_TRUE(std::bidirectional_iterator<typename Container::const_iterator>);
    }

    static void HasCorrectInterface()
        requires kIsRandomAccess && (!kIsContiguous)
    {
        EXPECT_TRUE(std::random_access_iterator<typename Container::iterator>);
        EXPECT_TRUE(std::random_access_iterator<typename Container::const_iterator>);
    }

    static void HasCorrectInterface()
        requires kIsContiguous
    {
        EXPECT_TRUE(std::contiguous_iterator<typename Container::iterator>);
        EXPECT_TRUE(std::contiguous_iterator<typename Container::const_iterator>);
    }

    /* ----------------------------------------------------------------------------------------- */

    static void AllIterationsWork() {
        SimpleIterationWorks();
        ConstantIterationWorks();
        ReverseIterationWorks();
        ConstReverseIterationWorks();
        RangeBasedForWorks();
    }

    static void SimpleIterationWorks() {
        Container container = SequenceProvider::Create();
        std::vector<ValueType> expected = SequenceProvider::ExpectedSequence();

        auto exp_it = expected.begin();
        for (auto it = container.begin(); it != container.end(); ++it, ++exp_it) {
            EXPECT_EQ(*exp_it, *it);
        }

        EXPECT_EQ(exp_it, expected.end());
    }

    static void ConstantIterationWorks() {
        const Container kContainer = SequenceProvider::Create();
        std::vector<ValueType> expected = SequenceProvider::ExpectedSequence();

        auto exp_it = expected.begin();
        for (auto it = kContainer.begin(); it != kContainer.end(); ++it, ++exp_it) {
            EXPECT_EQ(*exp_it, *it);
        }
        EXPECT_EQ(exp_it, expected.end());
    }

    static void ReverseIterationWorks()
        requires kIsBidirectional
    {
        Container container = SequenceProvider::Create();
        std::vector<ValueType> expected = SequenceProvider::ExpectedSequence();

        auto exp_it = expected.rbegin();
        for (auto it = container.rbegin(); it != container.rend(); ++it, ++exp_it) {
            EXPECT_EQ(*exp_it, *it);
        }
        EXPECT_EQ(exp_it, expected.rend());
    }

    static void ReverseIterationWorks() { EXPECT_FALSE(false); }

    static void ConstReverseIterationWorks()
        requires kIsBidirectional
    {
        const Container kContainer = SequenceProvider::Create();
        std::vector<ValueType> expected = SequenceProvider::ExpectedSequence();

        auto exp_it = expected.rbegin();
        for (auto it = kContainer.rbegin(); it != kContainer.rend(); ++it, ++exp_it) {
            EXPECT_EQ(*exp_it, *it);
        }
        EXPECT_EQ(exp_it, expected.rend());
    }

    static void ConstReverseIterationWorks() { EXPECT_FALSE(false); }

    static void RangeBasedForWorks() {
        Container container = SequenceProvider::Create();
        std::vector<ValueType> expected = SequenceProvider::ExpectedSequence();

        auto exp_it = expected.begin();
        for (auto& elem : container) {
            EXPECT_EQ(elem, *exp_it);
            ++exp_it;
        }
        EXPECT_EQ(exp_it, expected.end());
    }

    /* ----------------------------------------------------------------------------------------- */

    static void AllAlgorithmsWorks() {
        ForwardIteratorAlgorithmsWorks();
        BidirectionalIteratorAlgorithmsWorks();
        RandomAccessIteratorAlgorithmsWorks();
    }

    static void ForwardIteratorAlgorithmsWorks()
        requires kIsForward
    {
        Container origin = SequenceProvider::Create();
        std::vector<ValueType> expected = SequenceProvider::ExpectedSequence();

        std::vector<ValueType> buffer(origin.size());
        std::copy(origin.begin(), origin.end(), buffer.begin());

        EXPECT_EQ(expected, buffer);
    }

    static void ForwardIteratorAlgorithmsWorks() { EXPECT_FALSE(false); }

    static void BidirectionalIteratorAlgorithmsWorks()
        requires kIsBidirectional
    {
        Container origin = SequenceProvider::Create();
        std::vector<ValueType> expected = SequenceProvider::ExpectedSequence();

        std::vector<ValueType> buffer(origin.size() * 2);
        std::copy_backward(origin.begin(), origin.end(), buffer.end());

        for (std::size_t i = 0; i < origin.size(); ++i) {
            EXPECT_EQ(buffer[i], ValueType());
        }

        for (std::size_t i = origin.size(); i < origin.size() * 2; ++i) {
            EXPECT_EQ(buffer[i], expected[i - origin.size()]);
        }
    }

    static void BidirectionalIteratorAlgorithmsWorks() { EXPECT_FALSE(false); }

    static void RandomAccessIteratorAlgorithmsWorks()
        requires kIsRandomAccess
    {
        const std::size_t kIterations = 10;
        std::random_device device;
        std::mt19937 generator(device());

        for (std::size_t i = 0; i < kIterations; ++i) {
            Container origin = SequenceProvider::Create();
            std::vector<ValueType> expected = SequenceProvider::ExpectedSequence();

            EXPECT_TRUE(std::ranges::equal(origin, expected));

            std::shuffle(origin.begin(), origin.end(), generator);

            std::sort(origin.begin(), origin.end());

            EXPECT_TRUE(std::ranges::equal(origin, expected));
        }
    }

    static void RandomAccessIteratorAlgorithmsWorks() { EXPECT_FALSE(false); }

    /* ----------------------------------------------------------------------------------------- */

    static void CanCast() {
        const bool kCanCast =
            std::convertible_to<typename Container::iterator, typename Container::const_iterator>;
        EXPECT_TRUE(kCanCast);
    }
};
