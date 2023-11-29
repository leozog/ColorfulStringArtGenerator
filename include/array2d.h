#pragma once
#include <array>
#include <functional>

template <typename T>
class array2d
{
private:
    size_t w, h;
    std::vector<T> arr;

public:
    array2d(size_t w, size_t h);
    array2d(const array2d &other);
    array2d(array2d &&other);
    array2d &operator=(const array2d &other);
    array2d &operator=(array2d &&other) noexcept;
    T &operator()(size_t x, size_t y);
    void for_each(std::function<T(T &)> f);
    void for_each(std::function<T(T &, size_t)> f);
    void for_each(std::function<T(T &, size_t, size_t)> f);
    const T &operator()(size_t x, size_t y) const;
    void for_each(std::function<void(const T &)> f) const;
    void for_each(std::function<void(const T &, size_t)> f) const;
    void for_each(std::function<void(const T &, size_t, size_t)> f) const;
    bool is_in(size_t x, size_t y) const;
    size_t get_w() const;
    size_t get_h() const;
    const T *data() const;
};

template <typename T>
array2d<T>::array2d(size_t w, size_t h) : w{w}, h{h}, arr(w * h) {}

template <typename T>
array2d<T>::array2d(const array2d &other) : w{other.w}, h{other.h}, arr{other.arr} {}

template <typename T>
array2d<T>::array2d(array2d &&other) : w{other.w}, h{other.h}, arr{std::move(other.arr)} {}

template <typename T>
array2d<T> &array2d<T>::operator=(const array2d &other)
{
    if (this == &other)
        return *this;
    w = other.w;
    h = other.h;
    arr = std::copy(other.arr);
    return *this;
}

template <typename T>
array2d<T> &array2d<T>::operator=(array2d &&other) noexcept
{
    if (this == &other)
        return *this;
    w = other.w;
    h = other.h;
    arr = std::move(other.arr);
    return *this;
}

#ifdef _DEBUG
template <typename T>
T &array2d<T>::operator()(size_t x, size_t y)
{
    return arr.at(x + y * w);
}
#else
template <typename T>
T &array2d<T>::operator()(size_t x, size_t y)
{
    return arr[x + y * w];
}
#endif

#ifdef _DEBUG
template <typename T>
const T &array2d<T>::operator()(size_t x, size_t y) const
{
    return arr.at(x + y * w);
}
#else
template <typename T>
const T &array2d<T>::operator()(size_t x, size_t y) const
{
    return arr[x + y * w];
}
#endif

template <typename T>
void array2d<T>::for_each(std::function<T(T &)> f)
{
    for (auto &a : arr)
        a = f(a);
}

template <typename T>
void array2d<T>::for_each(std::function<T(T &, size_t)> f)
{
    for (size_t i = 0; i < w * h; i++)
        arr[i] = f(arr[i], i);
}

template <typename T>
void array2d<T>::for_each(std::function<T(T &, size_t, size_t)> f)
{
    for (size_t i = 0; i < h; i++)
    {
        for (size_t j = 0; j < w; j++)
            arr[j + i * w] = f(arr[j + i * w], j, i);
    }
}

template <typename T>
void array2d<T>::for_each(std::function<void(const T &)> f) const
{
    for (auto &a : arr)
        f(a);
}

template <typename T>
void array2d<T>::for_each(std::function<void(const T &, size_t)> f) const
{
    for (size_t i = 0; i < w * h; i++)
        f(arr[i], i);
}

template <typename T>
void array2d<T>::for_each(std::function<void(const T &, size_t, size_t)> f) const
{
    for (size_t i = 0; i < h; i++)
    {
        for (size_t j = 0; j < w; j++)
            f(arr[j + i * w], j, i);
    }
}

template <typename T>
bool array2d<T>::is_in(size_t x, size_t y) const
{
    if (x < 0 || x >= w)
        return false;
    if (y < 0 || y >= h)
        return false;
    return true;
}

template <typename T>
size_t array2d<T>::get_w() const
{
    return w;
}

template <typename T>
size_t array2d<T>::get_h() const
{
    return h;
}

template <typename T>
const T *array2d<T>::data() const
{
    return arr.data();
}