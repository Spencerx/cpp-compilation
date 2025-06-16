#pragma once

#include <type_traits>

/**
 * This is a proof of concept to show what a shared_ptr would look
 * like with const correctness. It doesn't have any optimization and
 * is missing functionality.
 */
template <typename T>
class SharedPtr {
  template <typename X>
  friend class SharedPtr;

public:
  SharedPtr(T *ptr)
      : ptr(ptr)
      , count(new int(1)) {
  }

  SharedPtr(SharedPtr &copy)
      : ptr(copy.ptr)
      , count(copy.count) {
    *count += 1;
  }

  // Enable this copy constructor only if T is const
  template <typename U = T, typename X,
            typename = std::enable_if_t<std::is_const<U>::value>>
  SharedPtr(const SharedPtr<X> &copy)
      : ptr(copy.ptr)
      , count(copy.count) {
    *count += 1;
  }

  SharedPtr(SharedPtr &&move)
      : ptr(move.ptr)
      , count(move.count) {
    move.ptr = nullptr;
    move.count = nullptr;
  }

  ~SharedPtr() {
    if (count == nullptr) {
      return;
    }

    *count -= 1;

    if (*count == 0) {
      delete count;

      if (ptr != nullptr) {
        delete ptr;
      }
    }
  }

  void operator=(SharedPtr &copy) noexcept {
    ptr = copy.ptr;
    count = copy.count;
    if (count != nullptr)
      *count += 1;
  }

  void operator=(SharedPtr &&move) noexcept {
    ptr = move.ptr;
    move.ptr = nullptr;

    count = move.count;
    move.count = nullptr;
  }

  T &operator*() noexcept {
    return *ptr;
  }
  const T &operator*() const noexcept {
    return *ptr;
  }

  T *operator->() noexcept {
    return ptr;
  }
  const T *operator->() const noexcept {
    return ptr;
  }

private:
  T *ptr;
  int *count;
};
