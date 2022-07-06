
#pragma once
#include "common.hpp"
#include <tuple>

namespace mj {
template <typename T, typename IdxType = std::size_t>
constexpr auto enumerate(T && iterable)
{
    using TIter = decltype(std::begin(std::declval<T>()));
    struct iterator
    {
        IdxType i;
        TIter iter;
        constexpr bool operator!=(const iterator & other) const { return iter != other.iter; }
        constexpr void operator++() { ++i; ++iter; }
        constexpr auto operator*() const { return std::tie(i, *iter); }
    };
    struct iterable_wrapper
    {
        T iterable;
        constexpr auto begin() { return iterator{ IdxType(), std::begin(iterable) }; }
        constexpr auto end() { return iterator{ IdxType(), std::end(iterable) }; }
    };
    return iterable_wrapper{ std::forward<T>(iterable) };
}

template <typename IdxType>
constexpr auto range(IdxType _start, IdxType _end, IdxType _step = 1)
{
    struct iterator
    {
        IdxType i;
        IdxType step;
        constexpr bool operator!=(const iterator & other) const { return i != other.i; }
        constexpr void operator++() { i += step; }
        constexpr auto &operator*() { return i; }
        constexpr auto operator*() const { return i; }
    };
    struct iterable_wrapper
    {
        IdxType start, back, step;
        constexpr auto begin() { return iterator{ start, step }; }
        constexpr auto end() { return iterator{ back, step }; }
    };
    return iterable_wrapper{ _start, _end, _step };
}

template <typename IdxType>
constexpr auto range(IdxType _end)
{
    return range(IdxType{}, _end);
}

template <typename IdxType>
constexpr auto range1(IdxType _end)
{
    return range(IdxType {1}, _end + 1);
}

}