#pragma once
#include <cassert>
#include <cstddef>
#include <vector>

template<typename T>
class Array2d
{
protected:
    size_t w, h;
    std::vector<T> arr;

private:
    template<typename U>
    class IteratorBase
    {
        friend class Array2d<T>;

        U* ptr;
        size_t w, x, y;

        IteratorBase(U* ptr, size_t w, size_t x, size_t y);

    public:
        class Element
        {
            friend class Array2d<T>::IteratorBase<U>;

            U* const ptr;
            const size_t x, y;

            Element(U* ptr, size_t x, size_t y);

        public:
            Element& operator=(const U& u);
            Element& operator=(U&& u);
            U& operator*() const;
            U* operator->() const;
            [[nodiscard]] size_t get_x() const;
            [[nodiscard]] size_t get_y() const;
        };

        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = U;
        using pointer = Element;
        using reference = Element;

        IteratorBase<U>& operator++();
        IteratorBase<U> operator++(int);
        bool operator==(const IteratorBase<U>& other) const;
        bool operator!=(const IteratorBase<U>& other) const;
        reference operator*() const;
        pointer operator->() const;
    };

public:
    using Iterator = Array2d<T>::IteratorBase<T>;
    using ConstIterator = Array2d<T>::IteratorBase<const T>;

    Array2d(size_t w = 0, size_t h = 1);
    T& operator()(size_t x, size_t y);
    const T& operator()(size_t x, size_t y) const;
    [[nodiscard]] bool is_in(size_t x, size_t y) const;
    [[nodiscard]] size_t get_w() const;
    [[nodiscard]] size_t get_h() const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] const T* data() const;
    [[nodiscard]] Array2d<T>::Iterator begin();
    [[nodiscard]] Array2d<T>::ConstIterator cbegin() const;
    [[nodiscard]] Array2d<T>::Iterator end();
    [[nodiscard]] Array2d<T>::ConstIterator cend() const;
};

// Array2d
template<typename T>
Array2d<T>::Array2d(size_t w, size_t h)
    : w{ w }
    , h{ h }
    , arr(w * h)
{
}

template<typename T>
T& Array2d<T>::operator()(size_t x, size_t y)
{
    assert(is_in(x, y));
    return arr[x + y * w];
}

template<typename T>
const T& Array2d<T>::operator()(size_t x, size_t y) const
{
    assert(is_in(x, y));
    return arr[x + y * w];
}

template<typename T>
bool Array2d<T>::is_in(size_t x, size_t y) const
{
    if (x >= w) {
        return false;
    }
    if (y >= h) {
        return false;
    }
    return true;
}

template<typename T>
size_t Array2d<T>::get_w() const
{
    return w;
}

template<typename T>
size_t Array2d<T>::get_h() const
{
    return h;
}

template<typename T>
size_t Array2d<T>::size() const
{
    return arr.size();
}

template<typename T>
const T* Array2d<T>::data() const
{
    return arr.data();
}

template<typename T>
typename Array2d<T>::Iterator Array2d<T>::begin()
{
    return Iterator(arr.data(), w, 0, 0);
}

template<typename T>
typename Array2d<T>::ConstIterator Array2d<T>::cbegin() const
{
    return ConstIterator{ arr.data(), w, 0, 0 };
}

template<typename T>
typename Array2d<T>::Iterator Array2d<T>::end()
{
    return Iterator{ arr.data() + w * h, w, 0, h };
}

template<typename T>
typename Array2d<T>::ConstIterator Array2d<T>::cend() const
{
    return ConstIterator{ arr.data() + w * h, w, 0, h };
}
// Array2d

// Array2d::IteratorBase
template<typename T>
template<typename U>
Array2d<T>::IteratorBase<U>::IteratorBase(U* ptr, size_t w, size_t x, size_t y)
    : ptr{ ptr }
    , w{ w }
    , x{ x }
    , y{ y }
{
}

template<typename T>
template<typename U>
Array2d<T>::IteratorBase<U>& Array2d<T>::IteratorBase<U>::operator++()
{
    ++ptr;
    if (x != w - 1) {
        ++x;
    } else {
        x = 0;
        ++y;
    }
    return *this;
}

template<typename T>
template<typename U>
Array2d<T>::IteratorBase<U> Array2d<T>::IteratorBase<U>::operator++(int)
{
    IteratorBase tmp = *this;
    ++*this;
    return tmp;
}

template<typename T>
template<typename U>
bool Array2d<T>::IteratorBase<U>::operator==(const IteratorBase<U>& other) const
{
    return ptr == other.ptr;
}

template<typename T>
template<typename U>
bool Array2d<T>::IteratorBase<U>::operator!=(const IteratorBase<U>& other) const
{
    return !(*this == other);
}

template<typename T>
template<typename U>
typename Array2d<T>::template IteratorBase<U>::reference Array2d<T>::IteratorBase<U>::operator*() const
{
    return Element{ ptr, x, y };
}

template<typename T>
template<typename U>
typename Array2d<T>::template IteratorBase<U>::pointer Array2d<T>::IteratorBase<U>::operator->() const
{
    return Element{ ptr, x, y };
}
// Array2d::IteratorBase

// Array2d::IteratorBase::Element
template<typename T>
template<typename U>
Array2d<T>::IteratorBase<U>::Element::Element(U* ptr, size_t x, size_t y)
    : ptr{ ptr }
    , x{ x }
    , y{ y }
{
}

template<typename T>
template<typename U>
Array2d<T>::IteratorBase<U>::Element& Array2d<T>::IteratorBase<U>::Element::operator=(const U& u)
{
    if (ptr == &u) {
        return *this;
    }
    *ptr = u;
    return *this;
}

template<typename T>
template<typename U>
Array2d<T>::IteratorBase<U>::Element& Array2d<T>::IteratorBase<U>::Element::operator=(U&& u)
{
    if (ptr == &u) {
        return *this;
    }
    *ptr = std::move(u);
    return *this;
}

template<typename T>
template<typename U>
U& Array2d<T>::IteratorBase<U>::Element::operator*() const
{
    return *ptr;
}

template<typename T>
template<typename U>
U* Array2d<T>::IteratorBase<U>::Element::operator->() const
{
    return ptr;
}

template<typename T>
template<typename U>
size_t Array2d<T>::IteratorBase<U>::Element::get_x() const
{
    return x;
}

template<typename T>
template<typename U>
size_t Array2d<T>::IteratorBase<U>::Element::get_y() const
{
    return y;
}
// Array2d::IteratorBase::Element