#pragma once
#include <algorithm>
#include <queue>
#include <vector>

// Extends std::priority_queue with a top_pop() method,
// which removes the top element and returns it using move semantics
// Based on the approach suggested in https://stackoverflow.com/a/51368276
template<class T, class Container = std::vector<T>, class Compare = std::less<typename Container::value_type>>
class PriorityQueue : public std::priority_queue<T, Container, Compare>
{
protected:
    using std::priority_queue<T, Container, Compare>::priority_queue;
    using std::priority_queue<T, Container, Compare>::c;
    using std::priority_queue<T, Container, Compare>::comp;

public:
    T top_pop()
    {
        std::pop_heap(c.begin(), c.end(), comp);
        T value = std::move(c.back());
        c.pop_back();
        return value;
    }
};