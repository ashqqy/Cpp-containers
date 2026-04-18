#pragma once

#include <algorithm>
#include <cstddef>
#include <new>
#include <stdexcept>
#include <type_traits>
#include <utility>

template <std::size_t N, typename... Ts>
struct type_at {};

template <typename Head, typename... Tail>
struct type_at<0, Head, Tail...> {
  using type = Head;
};

template <std::size_t N, typename Head, typename... Tail>
struct type_at<N, Head, Tail...> {
  using type = typename type_at<N - 1, Tail...>::type;
};

template <std::size_t N, typename... Ts>
using type_at_t = typename type_at<N, Ts...>::type;

template <typename... Ts>
struct index_of {};

template <typename T, typename... Tail>
struct index_of<T, T, Tail...> {
  static constexpr std::size_t value = 0;
};

template <typename T, typename U, typename... Tail>
struct index_of<T, U, Tail...> {
  static constexpr std::size_t value = 1 + index_of<T, Tail...>::value;
};

template <typename... Ts>
constexpr std::size_t index_of_v = index_of<Ts...>::value;

template <typename... Ts>
class Variant {
 public:
  static constexpr const char kGetTypeExceptionMessage[] =
      "Variant::get wrong type";
  static constexpr const char kGetIndexExceptionMessage[] =
      "Variant::get wrong index";

 private:
  static constexpr std::size_t kNpos = -1;

 public:
  template <typename T>
  Variant(T&& value) : active_idx_(kNpos) {
    constexpr std::size_t idx = index_of_v<std::decay_t<T>, Ts...>;
    new (storage_) std::decay_t<T>(std::forward<T>(value));
    active_idx_ = idx;
  }

  Variant(const Variant& other) : active_idx_(kNpos) {
    copy_from(other);
    active_idx_ = other.active_idx_;
  }

  Variant(Variant&& other) noexcept : active_idx_(other.active_idx_) {
    move_from(std::move(other));
  }

  ~Variant() {
    if (!valueless_by_exception()) {
      destroy();
    }
  }

  Variant& operator=(const Variant& other) {
    if (this != &other) {
      Variant temp(other);
      swap(temp);
    }
    return *this;
  }

  Variant& operator=(Variant&& other) noexcept {
    swap(other);
    return *this;
  }

 public:  // observers
  std::size_t index() const { return active_idx_; }

  bool valueless_by_exception() const { return active_idx_ == kNpos; }

  template <typename T>
  T& get() {
    constexpr std::size_t idx = index_of_v<T, Ts...>;
    if (idx != active_idx_) {
      throw std::runtime_error(kGetTypeExceptionMessage);
    }
    return *reinterpret_cast<T*>(storage_);
  }

  template <typename T>
  const T& get() const {
    constexpr std::size_t idx = index_of_v<T, Ts...>;
    if (idx != active_idx_) {
      throw std::runtime_error(kGetTypeExceptionMessage);
    }
    return *reinterpret_cast<const T*>(storage_);
  }

  template <std::size_t N>
  type_at_t<N, Ts...>& get() {
    using T = type_at_t<N, Ts...>;
    if (N != active_idx_) {
      throw std::runtime_error(kGetIndexExceptionMessage);
    }
    return *reinterpret_cast<T*>(storage_);
  }

  template <std::size_t N>
  const type_at_t<N, Ts...>& get() const {
    using T = type_at_t<N, Ts...>;
    if (N != active_idx_) {
      throw std::runtime_error(kGetIndexExceptionMessage);
    }
    return *reinterpret_cast<const T*>(storage_);
  }

 public:  // modifiers
  template <typename T, typename... Args>
  T& emplace(Args&&... args) {
    if (!valueless_by_exception()) {
      destroy();
      active_idx_ = kNpos;
    }

    constexpr std::size_t idx = index_of_v<T, Ts...>;
    T* ptr = new (storage_) T(std::forward<Args>(args)...);
    active_idx_ = idx;

    return *ptr;
  }

  template <std::size_t N, typename... Args>
  type_at_t<N, Ts...>& emplace(Args&&... args) {
    using T = type_at_t<N, Ts...>;
    return emplace<T, Args...>(std::forward<Args>(args)...);
  }

  void swap(Variant& other) noexcept {
    std::swap(storage_, other.storage_);
    std::swap(active_idx_, other.active_idx_);
  }

 private:
  template <std::size_t N = 0>
  void destroy() noexcept {
    if constexpr (N < sizeof...(Ts)) {
      if (N == active_idx_) {
        using T = type_at_t<N, Ts...>;
        reinterpret_cast<T*>(storage_)->~T();
      } else {
        destroy<N + 1>();
      }
    }
  }

  template <std::size_t N = 0>
  void copy_from(const Variant& other) {
    if constexpr (N < sizeof...(Ts)) {
      if (N == active_idx_) {
        using T = type_at_t<N, Ts...>;
        new (storage_) T(*reinterpret_cast<const T*>(other.storage_));
      } else {
        copy_from<N + 1>(other);
      }
    }
  }

  template <std::size_t N = 0>
  void move_from(Variant&& other) noexcept {
    if constexpr (N < sizeof...(Ts)) {
      if (N == active_idx_) {
        using T = type_at_t<N, Ts...>;
        new (storage_) T(std::move(*reinterpret_cast<T*>(other.storage_)));
      } else {
        move_from<N + 1>(std::move(other));
      }
    }
  }

 private:
  alignas(
      std::max({alignof(Ts)...})) std::byte storage_[std::max({sizeof(Ts)...})];

  std::size_t active_idx_;
};
