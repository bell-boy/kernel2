#include <kstdlib/shared.h>

namespace kstd {

template<typename T>
class List;

template<typename T>
struct ListNode {
    SharedPtr<ListNode<T>> next;
    SharedPtr<ListNode<T>> prev;
    T val;
};

template<typename T>
class List {
    SharedPtr<ListNode<T>> head_;
    SharedPtr<ListNode<T>> tail_;
public:
    List() : head_(nullptr), tail_(nullptr) {}
    void push_back(T val) {
        SharedPtr<ListNode<T>> new_node = new ListNode<T>();
        new_node->val = val;
        if (tail_ == nullptr) {
            head_ = new_node;
            tail_ = new_node;
        } else {
            tail_->next = new_node;
            new_node->prev = tail_;
            tail_ = new_node;
        }
    }
    void push_front(T val) {
        SharedPtr<ListNode<T>> new_node = new ListNode<T>();
        new_node->val = val;
        if (head_ == nullptr) {
            head_ = new_node;
            tail_ = new_node;
        } else {
            head_->prev = new_node;
            new_node->next = head_;
            head_ = new_node;
        }
    }
    SharedPtr<ListNode<T>> head() {
        return head_;
    }
    SharedPtr<ListNode<T>> tail() {
        return tail_;
    }
};

}
