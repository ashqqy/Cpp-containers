#pragma once

template <typename T, bool IsConst = true>
class VectorIterator {
  public:
    using value_type = T;
    using pointer = std::conditional_t<IsConst, const T*, T*>;
    using reference = std::conditional_t<IsConst, const T&, T&>;
    using difference_type = std::ptrdiff_t;

    VectorIterator() noexcept = default;
    explicit VectorIterator(pointer ptr) noexcept : ptr_(ptr) {}

    template <bool OtherConst>
        requires(IsConst && !OtherConst)
    VectorIterator(VectorIterator<T, OtherConst> const& other) : ptr_(other.ptr_) {}

  public:
    reference operator*() const { return *ptr_; }
    pointer operator->() const { return ptr_; }
    reference operator[](difference_type n) const { return *(ptr_ + n); }

    VectorIterator& operator++() {
        ++ptr_;
        return *this;
    }

    VectorIterator operator++(int) {
        VectorIterator tmp = *this;
        ++ptr_;
        return tmp;
    }

    VectorIterator& operator--() {
        --ptr_;
        return *this;
    }

    VectorIterator operator--(int) {
        VectorIterator tmp = *this;
        --ptr_;
        return tmp;
    }

    VectorIterator& operator+=(difference_type n) {
        ptr_ += n;
        return *this;
    }

    VectorIterator& operator-=(difference_type n) {
        ptr_ -= n;
        return *this;
    }

    VectorIterator operator+(difference_type n) const { return VectorIterator(ptr_ + n); }

    friend VectorIterator operator+(difference_type lhs, const VectorIterator& rhs) {
        return rhs + lhs;
    }

    VectorIterator operator-(difference_type n) const { return VectorIterator(ptr_ - n); }

    difference_type operator-(const VectorIterator& other) const { return ptr_ - other.ptr_; }

    auto operator<=>(const VectorIterator& other) const = default;

  private:
    pointer ptr_ = nullptr;
};
