#pragma once
#include <array>
#include <functional>
#include <stdexcept>

template<typename T>
class Array2d
{
protected:
    size_t w, h;
    std::vector<T> arr;

public:
    class Element
    {
        friend class Array2d<T>;
        Element(T* ptr, size_t x, size_t y);

        T* ptr;
        size_t x, y;

    public:
        T& operator*();
        const T& operator*() const;
        T* operator->();
        const T* operator->() const;
        T operator=(const T& t);
        size_t get_x() const;
        size_t get_y() const;
    };

    class ConstElement
    {
        friend class Array2d<T>;
        ConstElement(T* ptr, size_t x, size_t y);

        const T* ptr;
        size_t x, y;

    public:
        const T& operator*() const;
        const T* operator->() const;
        size_t get_x() const;
        size_t get_y() const;
    };

private:
    template<typename E>
    class IteratorBase
    {
        E e;
        const size_t w;

    public:
        IteratorBase(E e, size_t w);
        Array2d<T>::IteratorBase<E>& operator++();
        Array2d<T>::IteratorBase<E> operator++(int);
        Array2d<T>::IteratorBase<E>& operator--();
        Array2d<T>::IteratorBase<E> operator--(int);
        bool operator==(const IteratorBase& other) const;
        bool operator!=(const IteratorBase& other) const;
        E operator*() const;
    };

public:
    using Iterator = IteratorBase<Element>;
    using ConstIterator = IteratorBase<ConstElement>;

    Array2d(size_t w = 0, size_t h = 1);
    T& operator()(size_t x, size_t y);
    const T& operator()(size_t x, size_t y) const;
    bool is_in(size_t x, size_t y) const;
    size_t get_w() const;
    size_t get_h() const;
    size_t size() const;
    const T* data() const;
    Array2d<T>::Iterator begin();
    Array2d<T>::Iterator end();
    Array2d<T>::ConstIterator cbegin() const;
    Array2d<T>::ConstIterator cend() const;
};

// Array2d
template<typename T>
Array2d<T>::Array2d(size_t w, size_t h)
    : w{ w }
    , h{ h }
    , arr(w * h)
{
}

#ifndef NDEBUG
template<typename T>
T& Array2d<T>::operator()(size_t x, size_t y)
{
    if (!is_in(x, y)) {
        throw std::out_of_range("Array2d::operator()");
    }
    return arr[x + y * w];
}
#else
template<typename T>
T& Array2d<T>::operator()(size_t x, size_t y)
{
    return arr[x + y * w];
}
#endif

#ifndef NDEBUG
template<typename T>
const T& Array2d<T>::operator()(size_t x, size_t y) const
{
    if (!is_in(x, y)) {
        throw std::out_of_range("Array2d::operator()");
    }
    return arr[x + y * w];
}
#else
template<typename T>
const T& Array2d<T>::operator()(size_t x, size_t y) const
{
    return arr[x + y * w];
}
#endif

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
    return Iterator{ Element{ arr.data(), 0, 0 }, w };
}

template<typename T>
typename Array2d<T>::Iterator Array2d<T>::end()
{
    return Iterator{ Element{ arr.data() + w * h, 0, h }, w };
}

template<typename T>
typename Array2d<T>::ConstIterator Array2d<T>::cbegin() const
{
    return ConstIterator{ ConstElement{ arr.data(), 0, 0 }, w };
}

template<typename T>
typename Array2d<T>::ConstIterator Array2d<T>::cend() const
{
    return ConstIterator{ ConstElement{ arr.data() + w * h, 0, h }, w };
}
// Array2d

// Array2d::Element
template<typename T>
Array2d<T>::Element::Element(T* ptr, size_t x, size_t y)
    : ptr{ ptr }
    , x{ x }
    , y{ y }
{
}

template<typename T>
T& Array2d<T>::Element::operator*()
{
    return *ptr;
}

template<typename T>
const T& Array2d<T>::Element::operator*() const
{
    return *ptr;
}

template<typename T>
T* Array2d<T>::Element::operator->()
{
    return ptr;
}

template<typename T>
const T* Array2d<T>::Element::operator->() const
{
    return ptr;
}

template<typename T>
T Array2d<T>::Element::operator=(const T& t)
{
    return *ptr = t;
}

template<typename T>
size_t Array2d<T>::Element::get_x() const
{
    return x;
}

template<typename T>
size_t Array2d<T>::Element::get_y() const
{
    return y;
}
// Array2d::Element

// Array2d::ConstElement
template<typename T>
Array2d<T>::ConstElement::ConstElement(T* ptr, size_t x, size_t y)
    : ptr{ ptr }
    , x{ x }
    , y{ y }
{
}

template<typename T>
const T& Array2d<T>::ConstElement::operator*() const
{
    return *ptr;
}

template<typename T>
const T* Array2d<T>::ConstElement::operator->() const
{
    return ptr;
}

template<typename T>
size_t Array2d<T>::ConstElement::get_x() const
{
    return x;
}

template<typename T>
size_t Array2d<T>::ConstElement::get_y() const
{
    return y;
}
// Array2d::ConstElement

// Array2d::IteratorBase
template<typename T>
template<typename E>
Array2d<T>::IteratorBase<E>::IteratorBase(E e, size_t w)
    : e{ e }
    , w{ w }
{
}

template<typename T>
template<typename E>
Array2d<T>::IteratorBase<E>& Array2d<T>::IteratorBase<E>::operator++()
{
    e = e.x != w - 1 ? E{ e.ptr + 1, e.x + 1, e.y } : E{ e.ptr + 1, 0, e.y + 1 };
    return *this;
}

template<typename T>
template<typename E>
Array2d<T>::IteratorBase<E> Array2d<T>::IteratorBase<E>::operator++(int)
{
    IteratorBase tmp = *this;
    ++*this;
    return tmp;
}

template<typename T>
template<typename E>
Array2d<T>::IteratorBase<E>& Array2d<T>::IteratorBase<E>::operator--()
{
    e = e.x != 0 ? E{ e.ptr - 1, e.x - 1, e.y } : E{ e.ptr - 1, w - 1, e.y - 1 };
    return *this;
}

template<typename T>
template<typename E>
Array2d<T>::IteratorBase<E> Array2d<T>::IteratorBase<E>::operator--(int)
{
    IteratorBase tmp = *this;
    --*this;
    return tmp;
}

template<typename T>
template<typename E>
bool Array2d<T>::IteratorBase<E>::operator==(const IteratorBase& other) const
{
    return e.ptr == other.e.ptr;
}

template<typename T>
template<typename E>
bool Array2d<T>::IteratorBase<E>::operator!=(const IteratorBase& other) const
{
    return !(*this == other);
}

template<typename T>
template<typename E>
E Array2d<T>::IteratorBase<E>::operator*() const
{
    return e;
}
// Array2d::IteratorBase
