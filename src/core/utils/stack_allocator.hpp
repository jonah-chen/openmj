/**
 * Try to improve the performance of the program a bit by learning to write some
 * new things.
 */

#pragma once
#include <memory>
#include <vector>

namespace mj {
template <std::size_t MaxSize>
struct optim
{
    optim() = delete;

    /**
     * @brief Allocators for allocating vectors below a certain size on the stack.
     */
    template <typename Type>
    struct allocator
    {
        using value_type = Type;
        using size_type = size_t;
        using difference_type = std::ptrdiff_t;
        using propogate_on_container_move_assignment = std::true_type;
        using is_always_equal = std::true_type;

        constexpr Type *allocate(size_type n)
        {
            if (n > MaxSize)
                return new Type[n];
            return data;
        }
        constexpr void deallocate(Type *ptr, size_t n)
        {
            if (n > MaxSize)
                delete[] ptr;
        }
    private:
        Type data[MaxSize];
    };
};

template <typename T, std::size_t MaxSize>
using s_Alloc = typename optim<MaxSize>::template allocator<T>;

template <typename T, std::size_t MaxSize>
struct s_Vector : public std::vector<T, s_Alloc<T, MaxSize>>
{
    constexpr s_Vector() : std::vector<T, s_Alloc<T, MaxSize>>() 
    { this->reserve(MaxSize); }

    constexpr s_Vector(std::size_t n) : std::vector<T, s_Alloc<T, MaxSize>>(n)
    { this->reserve(MaxSize); }
};

} // namespace mj
