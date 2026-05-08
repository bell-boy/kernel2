template<typename T>
class SharedPtr {
    T* ptr_;
    int* ref_count_;
public:
    SharedPtr(T* ptr) : ptr_(ptr), ref_count_(new int(1)) {}
    SharedPtr(SharedPtr<T> &other) : ptr_(other.ptr_), ref_count_(ref_count_) {
        (*ref_count_)++;
    }
    ~SharedPtr() {
        (*ref_count_)--;
        if (*ref_count_ == 0) {
            delete ptr_;
            delete ref_count_;
        }
    }
    SharedPtr<T>& operator=(const SharedPtr<T> &other) {
        (*other.ref_count_)++;
        (*ref_count_--);
        if (*ref_count_ == 0){
            delete ptr_;
            delete ref_count_;
        }
        ptr_ = other.ptr_;
        ref_count_ = other.ref_count_;
    }
    T* operator->() {
        return ptr_;
    }
};
