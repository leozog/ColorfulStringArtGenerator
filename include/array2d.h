#pragma once
#include <cassert>
#include <cstddef>
#include <vector>

template<typename T>
class Array2d
{
    size_t w, h;
    std::vector<T> arr;

private:
    template<typename U>
    class IteratorBase
    {
        friend class Array2d<T>;

        U* ptr;
        size_t w;
        size_t x, y;
        size_t start_x, end_x;

        IteratorBase(U* ptr, size_t w, size_t start_x, size_t start_y, size_t end_x);

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

    class Region
    {
        friend class Array2d<T>;

        Array2d<T>& arr;
        const size_t start_x, start_y, end_x, end_y;

        Region(Array2d<T>& arr, size_t start_x, size_t start_y, size_t end_x, size_t end_y);

    public:
        [[nodiscard]] Array2d<T>::Iterator begin();
        [[nodiscard]] Array2d<T>::Iterator end();
    };

    class ConstRegion
    {
        friend class Array2d<T>;

        const Array2d<T>& arr;
        const size_t start_x, start_y, end_x, end_y;

        ConstRegion(const Array2d<T>& arr, size_t start_x, size_t start_y, size_t end_x, size_t end_y);

    public:
        [[nodiscard]] Array2d<T>::ConstIterator cbegin() const;
        [[nodiscard]] Array2d<T>::ConstIterator cend() const;
    };

    Array2d(size_t w = 0, size_t h = 1);
    T& operator()(size_t x, size_t y);
    const T& operator()(size_t x, size_t y) const;
    [[nodiscard]] bool is_in(size_t x, size_t y) const;
    [[nodiscard]] size_t get_w() const;
    [[nodiscard]] size_t get_h() const;
    [[nodiscard]] size_t size() const;
    [[nodiscard]] T* data();
    [[nodiscard]] const T* data() const;
    [[nodiscard]] Array2d<T>::Iterator begin();
    [[nodiscard]] Array2d<T>::ConstIterator cbegin() const;
    [[nodiscard]] Array2d<T>::Iterator end();
    [[nodiscard]] Array2d<T>::ConstIterator cend() const;
    [[nodiscard]] Array2d<T>::Region get_region(size_t start_x, size_t start_y, size_t end_x, size_t end_y);
    [[nodiscard]] Array2d<T>::ConstRegion get_cregion(size_t start_x, size_t start_y, size_t end_x, size_t end_y) const;
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
    return arr[x + (y * w)];
}

template<typename T>
const T& Array2d<T>::operator()(size_t x, size_t y) const
{
    assert(is_in(x, y));
    return arr[x + (y * w)];
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
T* Array2d<T>::data()
{
    return arr.data();
}

template<typename T>
const T* Array2d<T>::data() const
{
    return arr.data();
}

template<typename T>
typename Array2d<T>::Iterator Array2d<T>::begin()
{
    return Iterator(arr.data(), w, 0, 0, w);
}

template<typename T>
typename Array2d<T>::ConstIterator Array2d<T>::cbegin() const
{
    return ConstIterator{ arr.data(), w, 0, 0, w };
}

template<typename T>
typename Array2d<T>::Iterator Array2d<T>::end()
{
    return Iterator{ arr.data() + (w * h), w, 0, h, w };
}

template<typename T>
typename Array2d<T>::ConstIterator Array2d<T>::cend() const
{
    return ConstIterator{ arr.data() + (w * h), w, 0, h, w };
}

template<typename T>
typename Array2d<T>::Region Array2d<T>::get_region(size_t start_x, size_t start_y, size_t end_x, size_t end_y)
{
    return Region(*this, start_x, start_y, end_x, end_y);
}

template<typename T>
typename Array2d<T>::ConstRegion Array2d<T>::get_cregion(size_t start_x,
                                                         size_t start_y,
                                                         size_t end_x,
                                                         size_t end_y) const
{
    return ConstRegion(*this, start_x, start_y, end_x, end_y);
}
// Array2d

// Array2d::IteratorBase
template<typename T>
template<typename U>
Array2d<T>::IteratorBase<U>::IteratorBase(U* ptr, size_t w, size_t start_x, size_t start_y, size_t end_x)
    : ptr{ ptr }
    , w{ w }
    , x{ start_x }
    , y{ start_y }
    , start_x{ start_x }
    , end_x{ end_x }
{
}

template<typename T>
template<typename U>
Array2d<T>::IteratorBase<U>& Array2d<T>::IteratorBase<U>::operator++()
{
    if (x != end_x - 1) {
        ++ptr;
        ++x;
    } else {
        ptr += w - (end_x - start_x) + 1;
        x = start_x;
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

// Array2d::Region
template<typename T>
Array2d<T>::Region::Region(Array2d<T>& arr, size_t start_x, size_t start_y, size_t end_x, size_t end_y)
    : arr{ arr }
    , start_x{ start_x }
    , start_y{ start_y }
    , end_x{ end_x }
    , end_y{ end_y }
{
    assert(arr.is_in(start_x, start_y));
    assert(arr.is_in(end_x - 1, end_y - 1));
    assert(start_x < end_x);
    assert(start_y < end_y);
}

template<typename T>
typename Array2d<T>::Iterator Array2d<T>::Region::begin()
{
    return Iterator{ arr.data() + (start_x + start_y * arr.get_w()), arr.get_w(), start_x, start_y, end_x };
}

template<typename T>
typename Array2d<T>::Iterator Array2d<T>::Region::end()
{
    return Iterator{ arr.data() + (start_x + end_y * arr.get_w()), arr.get_w(), start_x, end_y, end_x };
}
// Array2d::Region

// Array2d::ConstRegion
template<typename T>
Array2d<T>::ConstRegion::ConstRegion(const Array2d<T>& arr, size_t start_x, size_t start_y, size_t end_x, size_t end_y)
    : arr{ arr }
    , start_x{ start_x }
    , start_y{ start_y }
    , end_x{ end_x }
    , end_y{ end_y }
{
    assert(arr.is_in(start_x, start_y));
    assert(arr.is_in(end_x - 1, end_y - 1));
    assert(start_x < end_x);
    assert(start_y < end_y);
}

template<typename T>
typename Array2d<T>::ConstIterator Array2d<T>::ConstRegion::cbegin() const
{
    return ConstIterator{ arr.data() + (start_x + start_y * arr.get_w()), arr.get_w(), start_x, start_y, end_x };
}

template<typename T>
typename Array2d<T>::ConstIterator Array2d<T>::ConstRegion::cend() const
{
    return ConstIterator{ arr.data() + (start_x + end_y * arr.get_w()), arr.get_w(), start_x, end_y, end_x };
}
// Array2d::ConstRegion