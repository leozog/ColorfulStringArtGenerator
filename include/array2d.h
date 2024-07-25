#pragma once
#include <array>
#include <functional>
#include <stdexcept>

template <typename T>
class Array2d
{
protected:
    size_t w, h;
    std::vector<T> arr;

public:
    Array2d(size_t w = 0, size_t h = 1);
    T &operator()(size_t x, size_t y);
    const T &operator()(size_t x, size_t y) const;
    bool is_in(size_t x, size_t y) const;
    size_t get_w() const;
    size_t get_h() const;
    size_t size() const;
    const T *data() const;
    Array2d<T>::iterator begin();
    Array2d<T>::iterator end();
    Array2d<T>::const_iterator cbegin() const;
    Array2d<T>::const_iterator cend() const;

    class element
    {
    private:
        T *const ptr;
        const size_t x, y;

    public:
        element(T *ptr, size_t x, size_t y);
        T &operator*();
        const T &operator*() const;
        T *operator->();
        const T *operator->() const;
        bool operator==(const element &other) const;
        bool operator!=(const element &other) const;
        size_t get_x() const;
        size_t get_y() const;
    };

    class iterator
    {
    private:
        element e;
        const size_t w;

    public:
        iterator(element e, size_t w);
        Array2d<T>::iterator &operator++();
        Array2d<T>::iterator operator++(int);
        bool operator==(const iterator &other) const;
        bool operator!=(const iterator &other) const;
        Array2d<T>::element &operator*();
    };

    class const_iterator
    {
    private:
        element e;
        const size_t w;

    public:
        const_iterator(element e, size_t w);
        Array2d<T>::const_iterator &operator++();
        Array2d<T>::const_iterator operator++(int);
        bool operator==(const const_iterator &other) const;
        bool operator!=(const const_iterator &other) const;
        const Array2d<T>::element &operator*() const;
    };
};

// Array2d
template <typename T>
Array2d<T>::Array2d(size_t w, size_t h) : w{w}, h{h}, arr(w * h) {}

#ifndef NDEBUG
template <typename T>
T &Array2d<T>::operator()(size_t x, size_t y)
{
    if (!is_in(x, y))
        throw std::out_of_range("Array2d::operator()");
    return arr[x + y * w];
}
#else
template <typename T>
T &Array2d<T>::operator()(size_t x, size_t y)
{
    return arr[x + y * w];
}
#endif

#ifndef NDEBUG
template <typename T>
const T &Array2d<T>::operator()(size_t x, size_t y) const
{
    if (!is_in(x, y))
        throw std::out_of_range("Array2d::operator()");
    return arr[x + y * w];
}
#else
template <typename T>
const T &Array2d<T>::operator()(size_t x, size_t y) const
{
    return arr[x + y * w];
}
#endif

template <typename T>
bool Array2d<T>::is_in(size_t x, size_t y) const
{
    if (x >= w)
        return false;
    if (y >= h)
        return false;
    return true;
}

template <typename T>
size_t Array2d<T>::get_w() const
{
    return w;
}

template <typename T>
size_t Array2d<T>::get_h() const
{
    return h;
}

template <typename T>
size_t Array2d<T>::size() const
{
    return arr.size();
}

template <typename T>
const T *Array2d<T>::data() const
{
    return arr.data();
}

template <typename T>
typename Array2d<T>::iterator Array2d<T>::begin() { return iterator{element{arr.data(), 0, 0}, w}; }

template <typename T>
typename Array2d<T>::iterator Array2d<T>::end() { return iterator{element{arr.data() + w * h, 0, h}, w}; }

template <typename T>
typename Array2d<T>::const_iterator Array2d<T>::cbegin() const { return const_iterator{element{arr.data(), 0, 0}, w}; }

template <typename T>
typename Array2d<T>::const_iterator Array2d<T>::cend() const { return const_iterator{element{arr.data() + w * h, 0, h}, w}; }
// Array2d

// Array2d::element
template <typename T>
Array2d<T>::element::element(T *ptr, size_t x, size_t y) : ptr{ptr}, x{x}, y{y} {}

template <typename T>
T &Array2d<T>::element::operator*() { return *ptr; }

template <typename T>
const T &Array2d<T>::element::operator*() const { return *ptr; }

template <typename T>
T *Array2d<T>::element::operator->() { return ptr; }

template <typename T>
const T *Array2d<T>::element::operator->() const { return ptr; }

template <typename T>
bool Array2d<T>::element::operator==(const element &other) const { return ptr == other.ptr; }

template <typename T>
bool Array2d<T>::element::operator!=(const element &other) const { return ptr != other.ptr; }

template <typename T>
size_t Array2d<T>::element::get_x() const { return x; }

template <typename T>
size_t Array2d<T>::element::get_y() const { return y; }
// Array2d::element

// Array2d::iterator
template <typename T>
Array2d<T>::iterator::iterator(element e, size_t w) : e{e}, w{w} {}

template <typename T>
typename Array2d<T>::iterator &Array2d<T>::iterator::operator++()
{
    e = element{
        e.ptr + 1,
        e.get_x() == w - 1 ? 0 : e.get_x() + 1,
        e.get_x() == w - 1 ? e.get_y() + 1 : e.get_y()};
    return *this;
}

template <typename T>
typename Array2d<T>::iterator Array2d<T>::iterator::operator++(int)
{
    iterator tmp = *this;
    ++*this;
    return tmp;
}

template <typename T>
bool Array2d<T>::iterator::operator==(const iterator &other) const { return e == other.e; }

template <typename T>
bool Array2d<T>::iterator::operator!=(const iterator &other) const { return e != other.e; }

template <typename T>
typename Array2d<T>::element &Array2d<T>::iterator::operator*() { return e; }
// Array2d::iterator

// Array2d::const_iterator
template <typename T>
Array2d<T>::const_iterator::const_iterator(element e, size_t w) : e{e}, w{w} {}

template <typename T>
typename Array2d<T>::const_iterator &Array2d<T>::const_iterator::operator++()
{
    e = element{
        e.ptr + 1,
        e.get_x() == w - 1 ? 0 : e.get_x() + 1,
        e.get_x() == w - 1 ? e.get_y() + 1 : e.get_y()};
    return *this;
}

template <typename T>
typename Array2d<T>::const_iterator Array2d<T>::const_iterator::operator++(int)
{
    const_iterator tmp = *this;
    ++*this;
    return tmp;
}

template <typename T>
bool Array2d<T>::const_iterator::operator==(const const_iterator &other) const { return e == other.e; }

template <typename T>
bool Array2d<T>::const_iterator::operator!=(const const_iterator &other) const { return e != other.e; }

template <typename T>
const typename Array2d<T>::element &Array2d<T>::const_iterator::operator*() const { return e; }
// Array2d::const_iterator