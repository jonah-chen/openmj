
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

}