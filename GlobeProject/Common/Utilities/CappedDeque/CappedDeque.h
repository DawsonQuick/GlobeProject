#pragma once
#include <deque>
#include <iostream>

template<typename T>
class CappedDeque {
public:
    explicit CappedDeque(size_t cap) : capacity(cap) {}

    void push_front(const T& value) {

        if (deque.size() > capacity) {
            while (deque.size() > capacity) {
                deque.pop_back();
            }
        }

        if (deque.size() == capacity) {
            deque.pop_back();  // Remove the oldest element (from the back)
        }
        deque.push_front(value);  // Add new element to the front
    }

    void updateCapacity(size_t cap) {
        capacity = cap;
    }

    const std::deque<T>& get() const {
        return deque;
    }

private:
    size_t capacity;
    std::deque<T> deque;
};
