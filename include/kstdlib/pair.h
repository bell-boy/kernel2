#pragma once
namespace kstd {
template<typename U, typename V>
class Pair {
    U first_;
    V second_;
public:
    Pair(U first, V second) : first_(first), second_(second) {}
    U& first() {return first_;}
    V& second() {return second_;}
};
}
