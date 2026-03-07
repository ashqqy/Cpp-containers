#pragma once

template <typename T>
class VectorBuffer {
  protected:
    VectorBuffer() = default;

    VectorBuffer(std::size_t size) : buffer_(allocate(size)), capacity_(size) {}

    ~VectorBuffer() {
        destroy(buffer_, buffer_ + size_);
        deallocate(buffer_);
    }

  protected:
    VectorBuffer(const VectorBuffer& other) = delete;
    VectorBuffer& operator=(const VectorBuffer& other) = delete;

    VectorBuffer(VectorBuffer&& other) noexcept { swap(other); }

    VectorBuffer& operator=(VectorBuffer&& other) noexcept {
        if (this != std::addressof(other)) { swap(other); }

        return *this;
    }

  protected:
    void swap(VectorBuffer& other) noexcept {
        std::swap(buffer_, other.buffer_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

  protected:
    static T* allocate(std::size_t size) {
        T* mem = nullptr;
        if (size != 0) { mem = static_cast<T*>(::operator new(size * sizeof(T))); }
        return mem;
    }

    static void deallocate(T* buffer) noexcept { ::operator delete(buffer); }

    static void construct(T* ptr, const T& value) { std::construct_at(ptr, value); }
    static void construct(T* ptr, T&& value) { std::construct_at(ptr, std::move(value)); }

    static void destroy(T* ptr) noexcept { std::destroy_at(ptr); }

    template <std::forward_iterator FwdIter>
    static void destroy(FwdIter begin, FwdIter end) noexcept {
        std::destroy(begin, end);
    }

  protected:
    T* buffer_ = nullptr;
    std::size_t capacity_ = 0;
    std::size_t size_ = 0;
};
