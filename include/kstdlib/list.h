#pragma once
#include <kstdlib/shared.h>

namespace kstd {

template<typename T>
class List {
    struct Entry {
        T value;
        SharedPtr<Entry> next;
        Entry* prev;

        Entry(T value) : value(value), next(nullptr), prev(nullptr) {}
    };

    SharedPtr<Entry> head_;
    Entry* tail_;

public:
    class Iterator {
        friend class List<T>;

        Entry* entry_;

        explicit Iterator(Entry* entry) : entry_(entry) {}

    public:
        Iterator() : entry_(nullptr) {}

        Iterator next() const {
            return entry_ == nullptr || entry_->next == nullptr
                ? Iterator(nullptr)
                : Iterator(entry_->next.get());
        }

        Iterator prev() const {
            return entry_ == nullptr ? Iterator(nullptr) : Iterator(entry_->prev);
        }

        T* operator->() const {
            if (entry_ == nullptr) KPANIC("Tried to dereference list end iterator");
            return &entry_->value;
        }

        T& operator*() const {
            if (entry_ == nullptr) KPANIC("Tried to dereference list end iterator");
            return entry_->value;
        }

        explicit operator bool() const {
            return entry_ != nullptr;
        }
    };

    List() : head_(nullptr), tail_(nullptr) {}

    void push_back(T val) {
        SharedPtr<Entry> new_entry = new Entry(val);
        Entry* entry = new_entry.get();
        if (tail_ != nullptr) {
            tail_->next = new_entry;
            entry->prev = tail_;
        } else {
            head_ = new_entry;
        }
        tail_ = entry;
    }

    void push_front(T val) {
        SharedPtr<Entry> new_entry = new Entry(val);
        Entry* entry = new_entry.get();
        if (head_ != nullptr) {
            head_->prev = entry;
            new_entry->next = head_;
        } else {
            tail_ = entry;
        }
        head_ = new_entry;
    }

    Iterator head() const {
        return Iterator(head_.get());
    }

    Iterator tail() const {
        return Iterator(tail_);
    }
};

}
