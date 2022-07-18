#include "common.hpp"
#include "itertools.hpp"

namespace mj {
/**
 * An array where you are allowed to add, subtract, and compare arrays. Similar
 * to elements in a mathematical module.
 *
 * @tparam T The type of the elements in the array.
 * @tparam N The number of elements in the array.
 * @note Only arrays of the same size can be added, subtracted, or compared.
 */
template <typename T, std::size_t N>
struct mod_array : public std::array<T, N>
{
    using count_type = T;

    constexpr mod_array operator+(const mod_array &other) const noexcept
    {
        mod_array result;
        std::transform(this->begin(), this->end(), other.begin(),
                       result.begin(), std::plus<>{});
        return result;
    }
    constexpr mod_array operator-(const mod_array &other) const noexcept
    {
        mod_array result;
        std::transform(this->begin(), this->end(), other.begin(),
                       result.begin(), std::minus<>{});
        return result;
    }
    constexpr mod_array &operator+=(const mod_array &other) noexcept
    {
        std::transform(this->begin(), this->end(), other.begin(), this->begin(),
                       std::plus<>{});
        return *this;
    }
    constexpr mod_array &operator-=(const mod_array &other) noexcept
    {
        std::transform(this->begin(), this->end(), other.begin(), this->begin(),
                       std::minus<>{});
        return *this;
    }
    constexpr bool operator==(const mod_array &other) const noexcept
    {
        for (auto i : range(k_UniqueTiles))
            if ((*this)[i] != other[i])
                return false;
        return true;
    }
    constexpr bool operator!=(const mod_array &other) const noexcept
    {
        return !(*this == other);
    }
    constexpr count_type sum() const noexcept
    {
        return std::accumulate(this->begin(), this->end(), 0);
    }
    constexpr mod_array cum() const noexcept
    {
        mod_array result;
        std::partial_sum(this->begin(), this->end(), result.begin());
        return result;
    }
};

/**
 * An array where vector addition and scalar multiplication are allowed.
 *
 * @tparam T The type of the elements in the array.
 * @tparam N The number of elements in the array.
 * @note Only arrays of the same size can be added, subtracted, or compared.
 */
template <typename T, std::size_t N>
struct vec_array : public mod_array<T, N>
{
    using mod_array<T, N>::mod_array;
    constexpr vec_array operator*(const T &other) const noexcept
    {
        vec_array result;
        std::transform(this->begin(), this->end(), result.begin(),
                       [other](const T &x) { return x * other; });
        return result;
    }

    constexpr vec_array operator/(const T &other) const
    {
        vec_array result;
        std::transform(this->begin(), this->end(), result.begin(),
                       [other](const T &x) { return x / other; });
        return result;
    }

    constexpr vec_array &operator*=(const T &other) noexcept
    {
        std::transform(this->begin(), this->end(), this->begin(),
                       [other](const T &x) { return x * other; });
        return *this;
    }

    constexpr vec_array &operator/=(const T &other)
    {
        std::transform(this->begin(), this->end(), this->begin(),
                       [other](const T &x) { return x / other; });
        return *this;
    }

    constexpr T dot(const vec_array &other) const noexcept
    {
        return std::inner_product(this->begin(), this->end(), other.begin(),
                                  T{});
    }

    constexpr T l1_norm() const noexcept
    {
        return std::accumulate(this->begin(), this->end(), T{},
                               [](T x, T y) { return x + std::abs(y); });
    }

    constexpr vec_array normalize_1() const noexcept
    {
        return *this / l1_norm();
    }

    constexpr vec_array &normalize_1_inplace() noexcept
    {
        *this /= l1_norm();
        return *this;
    }
};

} // namespace mj
