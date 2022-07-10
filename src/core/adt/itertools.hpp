
#pragma once
#include "common.hpp"
#include <tuple>

namespace mj {
/**
 * Similar to python's enumerate. Returns a tuple of the index and the value.
 *
 * @tparam T The type of the original container.
 * @tparam IdxType The type of the index. Default is size_t.
 * @param iterable The original container.
 * @return The iterable for the tuple of the index and the value.
 */
template <typename T, typename IdxType = std::size_t>
constexpr auto enumerate(T &&iterable)
{
    using TIter = decltype(std::begin(std::declval<T>()));
    struct iterator
    {
        IdxType i;
        TIter iter;
        constexpr bool operator!=(const iterator &other) const
        {
            return iter != other.iter;
        }
        constexpr void operator++()
        {
            ++i;
            ++iter;
        }
        constexpr auto operator*() const { return std::tie(i, *iter); }
    };
    struct iterable_wrapper
    {
        T iterable;
        constexpr auto begin()
        {
            return iterator{IdxType(), std::begin(iterable)};
        }
        constexpr auto end() { return iterator{IdxType(), std::end(iterable)}; }
    };
    return iterable_wrapper{std::forward<T>(iterable)};
}

/**
 * Similar to python's range function.
 *
 * @tparam IdxType The type of the index.
 * @param _start The start index.
 * @param _end The end index.
 * @param _step The step size. If omitted, it defaults to 1.
 * @return The range iterable.
 */
template <typename IdxType>
constexpr auto range(IdxType _start, IdxType _end, IdxType _step = 1)
{
    struct iterator
    {
        IdxType i;
        IdxType step;
        constexpr bool operator!=(const iterator &other) const
        {
            return i != other.i;
        }
        constexpr void operator++() { i += step; }
        constexpr auto &operator*() { return i; }
        constexpr auto operator*() const { return i; }
    };
    struct iterable_wrapper
    {
        IdxType start, back, step;
        constexpr auto begin() { return iterator{start, step}; }
        constexpr auto end() { return iterator{back, step}; }
    };
    return iterable_wrapper{_start, _end, _step};
}

/**
 * Range starting from 0 and with step 1.
 *
 * @tparam IdxType The type of the index.
 * @param _end The end index.
 * @return The range iterable.
 */
template <typename IdxType> constexpr auto range(IdxType _end)
{
    return range(IdxType{}, _end);
}

/**
 * Range starting from 1 and with step 1.
 *
 * @tparam IdxType The type of the index.
 * @param _end The end index.
 * @return The range iterable.
 */
template <typename IdxType> constexpr auto range1(IdxType _end)
{
    return range(IdxType{1}, _end + 1);
}

} // namespace mj
