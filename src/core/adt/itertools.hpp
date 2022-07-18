
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
CUDACOMPAT constexpr auto enumerate(T &&iterable, IdxType start_idx = {})
{
    using TIter = decltype(std::begin(std::declval<T>()));
    struct iterator
    {
        IdxType i;
        TIter iter;
        CUDACOMPAT constexpr bool operator!=(const iterator &other) const
        {
            return iter != other.iter;
        }
        CUDACOMPAT constexpr void operator++()
        {
            ++i;
            ++iter;
        }
        CUDACOMPAT constexpr auto operator*() const
        {
            return std::tie(i, *iter);
        }
    };
    struct iterable_wrapper
    {
        IdxType start;
        T iterable;
        CUDACOMPAT constexpr auto begin()
        {
            return iterator{start, std::begin(iterable)};
        }
        CUDACOMPAT constexpr auto end()
        {
            return iterator{IdxType(), std::end(iterable)};
        }
    };
    return iterable_wrapper{start_idx, std::forward<T>(iterable)};
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
CUDACOMPAT constexpr auto range(IdxType _start, IdxType _end, IdxType _step = 1)
{
    struct iterator
    {
        IdxType i;
        IdxType step;
        CUDACOMPAT constexpr bool operator!=(const iterator &other) const
        {
            return i != other.i;
        }
        CUDACOMPAT constexpr void operator++() { i += step; }
        CUDACOMPAT constexpr auto &operator*() { return i; }
        CUDACOMPAT constexpr auto operator*() const { return i; }
    };
    struct iterable_wrapper
    {
        IdxType start, back, step;
        CUDACOMPAT constexpr auto begin() { return iterator{start, step}; }
        CUDACOMPAT constexpr auto end() { return iterator{back, step}; }
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
template <typename IdxType>
CUDACOMPAT constexpr auto range(IdxType _end)
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
template <typename IdxType>
CUDACOMPAT constexpr auto range1(IdxType _end)
{
    return range(IdxType{1}, _end + 1);
}

} // namespace mj
