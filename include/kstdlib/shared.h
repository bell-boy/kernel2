#pragma once
#include "kstdlib/debug.h"
namespace kstd {

template<typename T>
class SharedPtr {
    T* ptr_;
    int* ref_count_;

    void release() {
        if (ref_count_ == nullptr) {
            return;
        }
        (*ref_count_)--;
        if (*ref_count_ == 0) {
            if (ptr_ != nullptr) delete ptr_;
            delete ref_count_;
        }
    }

public:
    SharedPtr() : ptr_(nullptr), ref_count_(nullptr) {}
    SharedPtr(T* ptr) : ptr_(ptr), ref_count_(ptr == nullptr ? nullptr : new int(1)) {}
    SharedPtr(T val) : ptr_(new T(val)), ref_count_(new int(1)) {}
    SharedPtr(const SharedPtr<T> &other) : ptr_(other.ptr_), ref_count_(other.ref_count_) {
        if (ref_count_ != nullptr) (*ref_count_)++;
    }
    template<typename... Args>
    static SharedPtr<T> make(Args... args) {
        return SharedPtr<T>(T(args...));
    }
    ~SharedPtr() {
        release();
    }
    SharedPtr<T>& operator=(const SharedPtr<T> &other) {
        if (this == &other) {
            return *this;
        }
        if (other.ref_count_ != nullptr) {
            *other.ref_count_ += 1;
        }
        release();
        ptr_ = other.ptr_;
        ref_count_ = other.ref_count_;
        return *this;
    }
    T* get() const {
        return ptr_;
    }
    T* operator->() const {
        return get();
    }
    bool operator==(const T* ptr) const {
        return ptr == ptr_;
    }
    bool operator==(const SharedPtr<T> &ptr) const {
        return ptr_ == ptr.ptr_;
    }
    T& operator*() {
        if (ptr_ == nullptr) KPANIC("Tried to dereference shared nullptr");
        return *ptr_;
    }
};

}
