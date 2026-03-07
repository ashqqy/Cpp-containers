#pragma once

#include <algorithm>
#include <compare>
#include <cstring>

#include "vector_bool.hpp"
#include "vector_buf.hpp"
#include "vector_iter.hpp"

namespace containers {

template <typename T>
class Vector : private VectorBuffer<T> {
  public:
    using value_type = T;
    using iterator = VectorIterator<value_type, false>;
    using const_iterator = VectorIterator<value_type, true>;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  private:
    static constexpr std::size_t kCapacityMultiplier = 2;
    static constexpr std::size_t kMinCapacity = 1;

    static constexpr const char* kAtExceptionMessage = "Vector::at";

  public: // constructors and destructor
    Vector() = default;

    Vector(std::size_t size, value_type value = value_type{}) : VectorBuffer<value_type>(size) {
        construct_from_value(size, value);
    }

    Vector(std::initializer_list<value_type> init) : VectorBuffer<value_type>(init.size()) {
        construct_from_range(init.begin(), init.end());
    }

    template <std::forward_iterator FwdIter>
    Vector(FwdIter begin, FwdIter end) : VectorBuffer<value_type>(end - begin) {
        construct_from_range(begin, end);
    }

    ~Vector() = default;

  private:
    template <std::forward_iterator FwdIter>
    Vector(std::size_t capacity, FwdIter begin, FwdIter end) : VectorBuffer<value_type>(capacity) {
        construct_from_range(begin, end);
    }

  public: // copy and move
    Vector(const Vector& vector) : VectorBuffer<value_type>(vector.size()) {
        construct_from_range(vector.begin(), vector.end());
    }

    Vector(Vector&& other) = default;

    Vector& operator=(const Vector& vector) {
        if (this != std::addressof(vector)) {
            Vector temp(vector);
            swap(temp);
        }

        return *this;
    }

    Vector& operator=(Vector&& vector) = default;

  public: // comparison
    auto operator<=>(const Vector& other) const
        noexcept(noexcept(std::declval<const value_type&>() <=>
                          std::declval<const value_type&>())) {
        return std::lexicographical_compare_three_way(data(), data() + size(), other.data(),
                                                      other.data() + other.size());
    }

    bool operator==(const Vector& other) const noexcept(noexcept(*this <=> other)) {
        if (size() != other.size()) { return false; }

        return (*this <=> other) == 0;
    }

    bool equal_to(const Vector& other) const noexcept(noexcept(*this == other)) {
        return *this == other;
    }

  public: // iterators
    iterator begin() noexcept { return iterator(data()); }
    iterator end() noexcept { return iterator(data() + size_); }
    const_iterator begin() const noexcept { return const_iterator(data()); }
    const_iterator end() const noexcept { return const_iterator(data() + size_); }

    const_iterator cbegin() const noexcept { return begin(); }
    const_iterator cend() const noexcept { return end(); }

    reverse_iterator rbegin() noexcept { return reverse_iterator(end()); }
    reverse_iterator rend() noexcept { return reverse_iterator(begin()); }
    const_reverse_iterator rbegin() const noexcept { return const_reverse_iterator(end()); }
    const_reverse_iterator rend() const noexcept { return const_reverse_iterator(begin()); }

    const_reverse_iterator crbegin() const noexcept { return rbegin(); }
    const_reverse_iterator crend() const noexcept { return rend(); }

  public: // element access
    value_type& operator[](std::size_t idx) noexcept { return buffer_[idx]; }

    const value_type& operator[](std::size_t idx) const noexcept { return buffer_[idx]; }

    value_type& at(std::size_t idx) {
        if (idx >= size()) { throw std::out_of_range(kAtExceptionMessage); }
        return buffer_[idx];
    }
    const value_type& at(std::size_t idx) const {
        if (idx >= size()) { throw std::out_of_range(kAtExceptionMessage); }
        return buffer_[idx];
    }

    value_type& front() noexcept { return (*this)[0]; }
    const value_type& front() const noexcept { return (*this)[0]; }

    value_type& back() noexcept { return (*this)[size() - 1]; }
    const value_type& back() const noexcept { return (*this)[size() - 1]; }

    value_type* data() noexcept { return buffer_; }
    const value_type* data() const noexcept { return buffer_; }

  public: // capacity
    bool empty() const noexcept { return size() == 0; }

    std::size_t size() const noexcept { return size_; }

    std::size_t capacity() const noexcept { return capacity_; }

    void reserve(std::size_t new_cap) {
        if (new_cap > capacity()) { reallocate(new_cap); }
    }

    void shrink_to_fit() {
        if (capacity() > size()) { reallocate(size()); }
    }

  public: // modifiers
    void clear() noexcept {
        destroy(begin(), end());
        size_ = 0;
    }

    void push_back(const value_type& value) {
        if (size_out_of_range()) { reallocate(doubled_capacity()); }

        construct(data() + size(), value);
        ++size_;
    }

    void push_back(value_type&& value) {
        if (size_out_of_range()) { reallocate(doubled_capacity()); }

        construct(data() + size(), std::move(value));
        ++size_;
    }

    void pop_back() noexcept {
        if (!empty()) {
            destroy(data() + size() - 1);
            --size_;
        }
    }

    void resize(std::size_t new_size, value_type value = value_type{}) {
        if (new_size < size()) {
            destroy(begin() + new_size, end());
            size_ = new_size;
        } else if (new_size < capacity()) {
            construct_from_value(new_size, value);
        } else {
            reallocate(new_size, value);
        }
    }

    void swap(Vector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(buffer_, other.buffer_);
    }

  private:
    using VectorBuffer<value_type>::construct;
    using VectorBuffer<value_type>::destroy;

    void construct_from_value(size_t new_size, const value_type& value) {
        for (std::size_t i = size(); i < new_size; ++i) {
            construct(data() + i, value);
            ++size_;
        }
    }

    template <std::forward_iterator FwdIter>
    void construct_from_range(FwdIter begin, FwdIter end) {
        std::size_t dist = end - begin;
        for (std::size_t i = size(); i < dist; ++i, ++begin) {
            construct(data() + i, *begin);
            ++size_;
        }
    }

    void reallocate(std::size_t new_cap) {
        if (new_cap == capacity()) { return; }

        Vector temp = reallocate_impl(new_cap);

        swap(temp);
    }

    void reallocate(std::size_t new_cap, const value_type& value) {
        if (new_cap == capacity()) { return; }

        Vector temp = reallocate_impl(new_cap);
        temp.construct_from_value(new_cap, value);

        swap(temp);
    }

    Vector reallocate_impl(std::size_t new_cap) {
        std::size_t to_copy = std::min(size(), new_cap);
        Vector temp(new_cap, begin(), begin() + to_copy);

        return temp;
    }

    bool size_out_of_range() const noexcept { return size() >= capacity(); }

    std::size_t doubled_capacity() const noexcept {
        return std::max(capacity() * kCapacityMultiplier, kMinCapacity);
    }

  private:
    using VectorBuffer<value_type>::buffer_;
    using VectorBuffer<value_type>::capacity_;
    using VectorBuffer<value_type>::size_;
};

} // namespace containers
