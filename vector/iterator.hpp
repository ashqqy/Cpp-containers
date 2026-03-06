#pragma once

#include <cstddef>
#include <iterator>

template <typename T, bool IsConst = false>
class VectorIterator {
  public:
    using element_type = std::conditional_t<IsConst, const T, T>;

    using iterator_tag = std::random_access_iterator_tag;
    using value_type = T;
    using difference_type = std::ptrdiff_t;
    using pointer = element_type*;
    using reference = element_type&;

  public:
    VectorIterator() noexcept = default;
    explicit VectorIterator(pointer ptr) noexcept : ptr_(ptr) {}

    template <bool IsOtherConst>
        requires IsConst && (!IsOtherConst)
    VectorIterator(const VectorIterator<T, IsOtherConst>& other) : ptr_(other.ptr_) {}

  public:
    reference operator*() const noexcept { return *ptr_; }
    pointer operator->() const noexcept { return ptr_; }
    reference operator[](difference_type n) const noexcept { return *(ptr_ + n); }

  public:
    auto operator<=>(const VectorIterator& other) const noexcept = default;

  public:
    VectorIterator& operator++() noexcept {
        ++ptr_;
        return *this;
    }

    VectorIterator operator++(int) noexcept {
        VectorIterator tmp = *this;
        ++ptr_;
        return tmp;
    }

    VectorIterator& operator--() noexcept {
        --ptr_;
        return *this;
    }

    VectorIterator operator--(int) noexcept {
        VectorIterator tmp = *this;
        --ptr_;
        return tmp;
    }

    VectorIterator& operator+=(difference_type n) noexcept {
        ptr_ += n;
        return *this;
    }

    VectorIterator& operator-=(difference_type n) noexcept {
        ptr_ -= n;
        return *this;
    }

    VectorIterator operator-(difference_type n) const noexcept { return VectorIterator(ptr_ - n); }

    difference_type operator-(const VectorIterator& other) const noexcept {
        return ptr_ - other.ptr_;
    }

    VectorIterator operator+(difference_type n) const noexcept { return VectorIterator(ptr_ + n); }

  private:
    pointer ptr_ = nullptr;
};

template <typename T, bool IsConst>
VectorIterator<T, IsConst> operator+(typename VectorIterator<T, IsConst>::difference_type lhs,
                                     const VectorIterator<T, IsConst>& rhs) noexcept {
    return rhs + lhs;
}
