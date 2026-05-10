#pragma once
#include "kstdlib/debug.h"
#include <kstdlib/shared.h>
namespace kstd {
template<typename T>
class Maybe  {
    SharedPtr<T> ptr_;
    Maybe(SharedPtr<T> ptr) : ptr_(ptr) {}
public:
    Maybe() = delete;
    Maybe(const Maybe<T> &other) : ptr_(other.ptr_) {}
    Maybe<T>& operator=(const Maybe<T>& other) {
        ptr_ = other.ptr_;
        return *this;
    }
    template<typename... Args>
    static Maybe<T> just(Args... args) {
        return Maybe<T>(SharedPtr<T>::make(args...));
    }
    static Maybe<T> nothing() {
        return Maybe<T>(nullptr);
    }
    bool is_nothing() const {
        return ptr_ == nullptr;
    }
    T get() {
        if (is_nothing()) {
            KPANIC("Tried to get value out of Nothing.");
        }
        return *ptr_;
    }

};
}
