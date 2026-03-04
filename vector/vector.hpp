#pragma once

#include <algorithm>
#include <compare>
#include <cstring>

#include "vector_bool.hpp"

namespace containers
{

template <typename T>
class Vector
{
  public:
    using value_type = T;

  private:
    static constexpr std::size_t kCapacityMultiplier = 2;
    static constexpr std::size_t kMinCapacity = 1;

  public: // constructors
    Vector() = default;

    Vector(std::size_t size, value_type value = value_type{})
        : size_(size), capacity_(size), buffer_(allocate_and_safe_fill(size, value))
    {
    }

    Vector(std::initializer_list<value_type> init)
        : size_(init.size()), capacity_(size_),
          buffer_(allocate_and_safe_copy(init.begin(), init.end()))
    {
    }

    Vector(const value_type* begin, const value_type* end)
        : size_(end - begin), capacity_(size_), buffer_(allocate_and_safe_copy(begin, end))
    {
    }

  public: // rule of five
    ~Vector() { deallocate(data()); }

    Vector(const Vector& vector)
        : size_(vector.size()), capacity_(vector.capacity()),
          buffer_(allocate_and_safe_copy(vector.data(), vector.data() + vector.size()))
    {
    }

    Vector(Vector&& other) { swap(other); }

    Vector& operator=(const Vector& vector)
    {
        if (this != std::addressof(vector))
        {
            Vector temp(vector);
            swap(temp);
        }

        return *this;
    }

    Vector& operator=(Vector&& vector)
    {
        if (this != std::addressof(vector))
        {
            swap(vector);
        }

        return *this;
    }

  public: // comparison
    auto operator<=>(const Vector& other) const
        noexcept(noexcept(std::declval<const T&>() <=> std::declval<const T&>()))
    {
        return std::lexicographical_compare_three_way(data(), data() + size(), other.data(),
                                                      other.data() + other.size());
    }

    bool operator==(const Vector& other) const noexcept(noexcept(*this <=> other))
    {
        if (size() != other.size())
        {
            return false;
        }

        return (*this <=> other) == 0;
    }

    bool equal_to(const Vector& other) const noexcept(noexcept(*this == other))
    {
        return *this == other;
    }

  public: // element access
    value_type& operator[](std::size_t idx) noexcept { return buffer_[idx]; }

    const value_type& operator[](std::size_t idx) const noexcept { return buffer_[idx]; }

    value_type& at(std::size_t idx)
    {
        if (idx >= size())
        {
            throw std::out_of_range("Vector::at");
        }
        return buffer_[idx];
    }
    const value_type& at(std::size_t idx) const
    {
        if (idx >= size())
        {
            throw std::out_of_range("Vector::at");
        }
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

    void reserve(std::size_t new_cap)
    {
        if (new_cap > capacity())
        {
            reallocate(new_cap);
        }
    }

    void shrink_to_fit()
    {
        if (capacity() > size())
        {
            reallocate(size());
        }
    }

  public: // modifiers
    void clear() noexcept { size_ = 0; }

    void push_back(const value_type& value)
    {
        if (size_out_of_range())
        {
            Vector temp(doubled_capacity());
            copy(data(), data() + size(), temp.data());
            temp.size_ = size();
            temp.push_back(value);
            swap(temp);
            return;
        }

        (*this)[size_] = value;
        ++size_;
    }

    void pop_back() noexcept
    {
        if (!empty())
        {
            --size_;
        }
    }

    void resize(std::size_t new_size, value_type value = value_type{})
    {
        if (new_size < size())
        {
            size_ = new_size;
        }
        else if (new_size < capacity())
        {
            fill(data() + size(), data() + new_size, value);
            size_ = new_size;
        }
        else
        {
            reallocate_and_fill(new_size, value);
        }
    }

    void swap(Vector& other) noexcept
    {
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
        std::swap(buffer_, other.buffer_);
    }

  private:
    static value_type* allocate(std::size_t size) { return new value_type[size]; }
    static void deallocate(value_type* buffer) noexcept { delete[] buffer; }

    void reallocate(std::size_t new_cap)
    {
        if (new_cap == capacity())
        {
            return;
        }

        value_type* new_buffer = allocate(new_cap);

        if (data() != nullptr)
        {
            std::size_t to_copy = std::min(size(), new_cap);
            copy(data(), data() + to_copy, new_buffer);
            deallocate(data());
        }

        buffer_ = new_buffer;
        capacity_ = new_cap;
    }

    void reallocate_and_fill(std::size_t new_cap, const value_type& value)
    {
        Vector temp(new_cap);
        copy(data(), data() + size(), temp.data());

        value_type* first = temp.data() + size();
        value_type* last = temp.data() + new_cap;

        if (last > first)
        {
            fill(first, last, value);
        }

        swap(temp);
    }

    bool size_out_of_range() const noexcept { return size() >= capacity(); }

    std::size_t doubled_capacity() const noexcept
    {
        return std::max(capacity() * kCapacityMultiplier, kMinCapacity);
    }

    static void fill(value_type* begin, value_type* end, const value_type& value)
    {
        std::fill(begin, end, value);
    }

    static void copy(const value_type* begin, const value_type* end, value_type* result)
    {
        std::copy(begin, end, result);
    }

    static T* allocate_and_safe_fill(std::size_t size, const value_type& value)
    {
        T* dst = allocate(size);

        try
        {
            fill(dst, dst + size, value);
        }
        catch (...)
        {
            deallocate(dst);
            throw;
        }

        return dst;
    }

    static value_type* allocate_and_safe_copy(const value_type* begin, const value_type* end)
    {
        T* result = allocate(end - begin);
        try
        {
            copy(begin, end, result);
        }
        catch (...)
        {
            deallocate(result);
            throw;
        }
        return result;
    }

  private:
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;
    value_type* buffer_ = nullptr;
};

} // namespace containers
