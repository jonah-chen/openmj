
#pragma once
#include "common.hpp"

namespace mj {
template <typename T, std::size_t N>
class vector
{
public:
    using value_type = T;
    using size_type = std::size_t;
    using difference_type = std::ptrdiff_t;
    using reference = T &;
    using const_reference = const T &;
    using pointer = T *;
    using const_pointer = const T *;
    using iterator = T *;
    using const_iterator = const T *;
    using reverse_iterator = std::reverse_iterator<iterator>;
    using const_reverse_iterator = std::reverse_iterator<const_iterator>;

    CUDACOMPAT constexpr vector() noexcept {}
    CUDACOMPAT explicit constexpr vector(InitMode mode) noexcept
    {
        if (mode == InitMode::default_fill)
        {
            size_ = N;
            for (size_type i = 0; i < N; ++i)
                data_[i] = T();
        }
    }
    CUDACOMPAT constexpr vector(size_type n, const_reference v) MJ_EXCEPT_CRIT
    {
        for (size_type i = 0; i < n; ++i)
            push_back(v);
    }
    CUDACOMPAT constexpr T *data() noexcept { return data_; }
    CUDACOMPAT constexpr const T *data() const noexcept { return data_; }
    CUDACOMPAT constexpr iterator begin() noexcept { return data_; }
    CUDACOMPAT constexpr const_iterator begin() const noexcept { return data_; }
    CUDACOMPAT constexpr iterator end() noexcept { return data_ + size_; }
    CUDACOMPAT constexpr const_iterator end() const noexcept
    {
        return data_ + size_;
    }
    CUDACOMPAT constexpr reverse_iterator rbegin() noexcept
    {
        return reverse_iterator(end());
    }
    CUDACOMPAT constexpr const_reverse_iterator rbegin() const noexcept
    {
        return const_reverse_iterator(end());
    }
    CUDACOMPAT constexpr reverse_iterator rend() noexcept
    {
        return reverse_iterator(begin());
    }
    CUDACOMPAT constexpr const_reverse_iterator rend() const noexcept
    {
        return const_reverse_iterator(begin());
    }
    CUDACOMPAT constexpr bool empty() const noexcept { return size_ == 0; }
    CUDACOMPAT constexpr size_type size() const noexcept { return size_; }
    CUDACOMPAT constexpr size_type max_size() const noexcept { return N; }
    CUDACOMPAT constexpr size_type capacity() const noexcept { return N; }
    CUDACOMPAT constexpr void clear() noexcept { size_ = 0; }
    CUDACOMPAT constexpr void push_back(const_reference value) MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(size_ < N);
        data_[size_++] = value;
    }
    CUDACOMPAT constexpr void push_back(value_type &&value) MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(size_ < N);
        data_[size_++] = std::move(value);
    }

    template <typename... Args>
    CUDACOMPAT constexpr void emplace_back(Args &&...args) MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(size_ < N);
        data_[size_++] = T(std::forward<Args>(args)...);
    }
    CUDACOMPAT constexpr void pop_back() MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(size_ > 0);
        --size_;
    }
    CUDACOMPAT constexpr reference back() MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(size_ > 0);
        return data_[size_ - 1];
    }
    CUDACOMPAT constexpr const_reference back() const MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(size_ > 0);
        return data_[size_ - 1];
    }
    CUDACOMPAT constexpr reference front() MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(size_ > 0);
        return data_[0];
    }
    CUDACOMPAT constexpr const_reference front() const MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(size_ > 0);
        return data_[0];
    }
    CUDACOMPAT constexpr reference operator[](size_type index) MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(index < size_);
        return data_[index];
    }
    CUDACOMPAT constexpr const_reference
    operator[](size_type index) const MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(index < size_);
        return data_[index];
    }
    CUDACOMPAT constexpr iterator erase(iterator pos) MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(pos >= begin() && pos < end());
        for (auto i = pos; i < end(); ++i)
            *i = *(i + 1);
        --size_;
        return pos;
    }
    CUDACOMPAT constexpr iterator erase(iterator first,
                                        iterator last) MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(first >= begin() && first < end() && last >= begin() &&
                       last <= end());
        auto n = last - first;
        for (auto i = first; i < end() - n; ++i)
            *i = *(i + n);
        size_ -= n;
        return first;
    }
    CUDACOMPAT constexpr iterator insert(iterator pos,
                                         const_reference value) MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(pos >= begin() && pos < end() && size_ < N);
        for (auto i = end(); i > pos; --i)
            *i = *(i - 1);
        *pos = value;
        ++size_;
        return pos;
    }
    CUDACOMPAT constexpr iterator insert(iterator pos,
                                         value_type &&value) MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(pos >= begin() && pos < end() && size_ < N);
        for (auto i = end(); i > pos; --i)
            *i = *(i - 1);
        *pos = std::move(value);
        ++size_;
        return pos;
    }
    CUDACOMPAT constexpr iterator insert(iterator pos, size_type n,
                                         const_reference value) MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(pos >= begin() && pos < end() && size_ + n <= N);
        for (auto i = end(); i > pos; --i)
            *i = *(i - 1);
        for (auto i = 0; i < n; ++i)
            *(pos + i) = value;
        size_ += n;
        return pos;
    }
    CUDACOMPAT constexpr iterator insert(iterator pos, const_iterator first,
                                         const_iterator last)
    {
        auto n = last - first;
        for (auto i = end(); i > pos; --i)
            *i = *(i - 1);
        for (auto i = 0; i < n; ++i)
            *(pos + i) = *(first + i);
        size_ += n;
        return pos;
    }
    template <typename... Args>
    CUDACOMPAT constexpr iterator emplace(iterator pos,
                                          Args &&...args) MJ_EXCEPT_CRIT
    {
        MJ_CUDA_ASSERT(pos >= begin() && pos < end() && size_ < N);
        for (auto i = end(); i > pos; --i)
            *i = *(i - 1);
        *pos = T(std::forward<Args>(args)...);
        ++size_;
        return pos;
    }

private:
    T data_[N];
    std::size_t size_{0};
};

} // namespace mj
