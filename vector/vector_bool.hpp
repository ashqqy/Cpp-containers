#pragma once

#include <algorithm>
#include <compare>
#include <cstring>

namespace containers {

namespace details {

inline std::size_t kBlockSize = CHAR_BIT;

inline std::size_t bits_to_bytes(std::size_t n_bits) noexcept {
    return (n_bits + kBlockSize - 1) / kBlockSize;
}

inline std::size_t block_idx(std::size_t idx) noexcept {
    return idx / kBlockSize;
}

inline std::size_t offset_in_block(std::size_t idx) noexcept {
    return idx % kBlockSize;
}

inline std::byte get_bit_mask(std::size_t offset) noexcept {
    return static_cast<std::byte>(1) << offset;
}

inline bool get_bit_value(const std::byte* buffer, std::size_t index) noexcept {
    return (buffer[block_idx(index)] & get_bit_mask(offset_in_block(index))) != std::byte(0);
}

class BitRef {
  public:
    BitRef(std::byte* block, std::size_t offset) : block_ptr_(block), mask_(get_bit_mask(offset)) {}

    operator bool() const noexcept { return (*block_ptr_ & mask_) != std::byte(0); }

    BitRef& operator=(bool value) noexcept {
        if (value) {
            *block_ptr_ |= mask_;
        } else {
            *block_ptr_ &= ~mask_;
        }

        return *this;
    }

    BitRef& operator=(const BitRef& other) noexcept {
        if (other) {
            *block_ptr_ |= mask_;
        } else {
            *block_ptr_ &= ~mask_;
        }
        return *this;
    }

  private:
    std::byte* block_ptr_;
    std::byte mask_;
};

} // namespace details

template <typename T>
class Vector;

template <>
class Vector<bool> {
  public:
    using value_type = bool;

  private:
    static inline std::size_t kCapacityMultiplier = 2;
    static inline std::size_t kMinCapacity = 1;

  public: // constructors
    Vector() = default;

    Vector(std::size_t size, bool value = false)
        : size_(size), capacity_(size), buffer_(allocate_bool(capacity_)) {
        fill(value, 0, size_);
    }

    Vector(std::initializer_list<bool> init)
        : size_(init.size()), capacity_(size_), buffer_(allocate_bool(capacity_)) {
        copy(init.begin(), init.end());
    }

    Vector(const bool* begin, const bool* end)
        : size_(end - begin), capacity_(size_), buffer_(allocate_bool(capacity_)) {
        copy(begin, end);
    }

  public: // rule of five
    ~Vector() { deallocate(buffer_); }

    Vector(const Vector& vector)
        : size_(vector.size()), capacity_(vector.capacity()), buffer_(allocate_bool(capacity_)) {
        std::copy(vector.buffer_, vector.buffer_ + details::bits_to_bytes(vector.size()), buffer_);
    }

    Vector(Vector&& other) { swap(other); }

    Vector& operator=(const Vector& vector) {
        if (this != std::addressof(vector)) {
            Vector temp(vector);
            swap(temp);
        }
        return *this;
    }

    Vector& operator=(Vector&& vector) {
        if (this != std::addressof(vector)) { swap(vector); }

        return *this;
    }

  public: // comparsion
    std::strong_ordering operator<=>(const Vector& other) const noexcept {
        std::size_t min_size = std::min(size_, other.size_);
        for (std::size_t i = 0; i < min_size; ++i) {
            std::strong_ordering cmp = (*this)[i] <=> other[i];
            if (cmp != 0) { return cmp; }
        }

        return size_ <=> other.size_;
    }

    bool operator==(const Vector& other) const noexcept {
        if (size() != other.size()) { return false; }

        return (*this <=> other) == 0;
    }

    bool equal_to(const Vector& other) const noexcept { return *this == other; }

  public: // element access
    details::BitRef operator[](std::size_t index) noexcept {
        return details::BitRef(buffer_ + details::block_idx(index),
                               details::offset_in_block(index));
    }

    bool operator[](std::size_t index) const noexcept {
        return details::get_bit_value(buffer_, index);
    }

    bool at(std::size_t idx) const {
        if (idx >= size()) { throw std::out_of_range("Vector::at"); }
        return (*this)[idx];
    }
    details::BitRef at(std::size_t idx) {
        if (idx >= size()) { throw std::out_of_range("Vector::at"); }
        return (*this)[idx];
    }

    bool front() const noexcept { return (*this)[0]; }
    details::BitRef front() noexcept { return (*this)[0]; }

    bool back() const noexcept { return (*this)[size() - 1]; }
    details::BitRef back() noexcept { return (*this)[size() - 1]; }

  public: // capacity
    bool empty() const noexcept { return size() == 0; }

    std::size_t size() const noexcept { return size_; }

    std::size_t capacity() const noexcept { return capacity_; }

    void reserve(std::size_t new_cap) {
        if (new_cap > capacity()) { reallocate_bool(new_cap); }
    }

    void shrink_to_fit() {
        if (capacity() > size()) { reallocate_bool(size()); }
    }

  public: // modifiers
    void clear() noexcept { size_ = 0; }

    void push_back(bool value) {
        if (size_out_of_range()) { reallocate_bool(doubled_capacity()); }

        set(size_++, value);
    }

    void pop_back() noexcept {
        if (!empty()) { --size_; }
    }

    void resize(std::size_t new_size, bool value = false) {
        if (new_size < size()) {
            size_ = new_size;
        } else if (new_size < capacity()) {
            fill(value, size_, new_size);
            size_ = new_size;
        } else {
            std::size_t old_size = size();
            reallocate_bool(new_size);
            fill(value, old_size, new_size);
            size_ = new_size;
        }
    }

    void swap(Vector& other) noexcept {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(buffer_, other.buffer_);
    }

    void set(std::size_t index, bool value = true) noexcept { (*this)[index] = value; }

    void flip(std::size_t index) noexcept {
        std::size_t block = details::block_idx(index);
        std::size_t offset = details::offset_in_block(index);
        buffer_[block] ^= (details::get_bit_mask(offset));
    }

    void flip() noexcept {
        for (std::size_t i = 0; i < size(); ++i) {
            flip(i);
        }
    }

  private:
    static std::byte* allocate_bool(std::size_t capacity) {
        return new std::byte[details::bits_to_bytes(capacity)];
    }
    static void deallocate(const std::byte* buffer) noexcept { delete[] buffer; }

    void reallocate_bool(std::size_t new_cap) {
        if (new_cap == capacity()) { return; }

        std::byte* new_buffer = allocate_bool(new_cap);

        if (buffer_ != nullptr) {
            std::size_t to_copy = details::bits_to_bytes(std::min(size(), new_cap));
            std::copy(buffer_, buffer_ + to_copy, new_buffer);
            deallocate(buffer_);
        }

        buffer_ = new_buffer;
        capacity_ = new_cap;
    }

    void fill(bool value, std::size_t start_idx, std::size_t end_idx) noexcept {
        if (start_idx > end_idx) { return; }

        for (std::size_t i = start_idx; i < end_idx; ++i) {
            set(i, value);
        }
    }

    void copy(const bool* begin, const bool* end) noexcept {
        if (begin > end) { return; }

        for (std::size_t i = 0; i < size(); ++i) {
            set(i, begin[i]);
        }
    }

    bool size_out_of_range() const noexcept { return size() >= capacity(); }

    std::size_t doubled_capacity() const noexcept {
        return std::max(capacity() * kCapacityMultiplier, kMinCapacity);
    }

  private:
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
    std::byte* buffer_ = nullptr;
};

} // namespace containers
