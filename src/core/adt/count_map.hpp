
#pragma once
#include "core/mahjong/constants.hpp"
namespace mj {
template <typename KeyType, typename CounterType = U32f>
class count_map
{
public:
    using map_type = std::unordered_map<KeyType, CounterType>;
    using key_type = typename map_type::key_type;
    using value_type = typename map_type::value_type;
    using size_type = typename map_type::size_type;
    using difference_type = typename map_type::difference_type;
    using reference = typename map_type::reference;
    using const_reference = typename map_type::const_reference;
    using iterator = typename map_type::iterator;
    using const_iterator = typename map_type::const_iterator;
    using local_iterator = typename map_type::local_iterator;
    using const_local_iterator = typename map_type::const_local_iterator;

public:
    count_map() = default;
    count_map(const count_map &other) : data_(other.data_), total_(other.total_)
    {
    }
    count_map(count_map &&other)
        : data_(std::move(other.data_)), total_(other.total_)
    {
    }
    count_map &operator=(const count_map &other)
    {
        data_ = other.data_;
        total_ = other.total_;
        return *this;
    }
    count_map &operator=(count_map &&other)
    {
        data_ = std::move(other.data_);
        total_ = other.total_;
        return *this;
    }

    void clear()
    {
        data_.clear();
        total_ = 0;
    }
    bool empty() const { return data_.empty(); }
    size_type size() const { return data_.size(); }
    CounterType total() const { return total_; }

    iterator begin() { return data_.begin(); }
    iterator end() { return data_.end(); }
    const_iterator begin() const { return data_.begin(); }
    const_iterator end() const { return data_.end(); }

    void add(const key_type &key, CounterType count = 1)
    {
        auto it = data_.find(key);
        if (it == data_.end())
            data_[key] = count;
        else
            it->second += count;
        total_ += count;
    }

    count_map &operator+=(const count_map &other)
    {
        for (const auto &[key, count] : other)
            add(key, count);
        return *this;
    }

    count_map &operator*=(CounterType factor)
    {
        for (auto &[key, count] : data_)
            count *= factor;
        total_ *= factor;
        return *this;
    }

    count_map &operator/=(CounterType divisor)
    {
        for (auto &[key, count] : data_)
            count /= divisor;
        total_ /= divisor;
        return *this;
    }

    count_map operator+(const count_map &other) const
    {
        count_map result(*this);
        result += other;
        return result;
    }

    count_map operator*(CounterType factor) const
    {
        count_map result(*this);
        result *= factor;
        return result;
    }

    count_map operator/(CounterType divisor) const
    {
        count_map result(*this);
        result /= divisor;
        return result;
    }

    CounterType &operator[](const key_type &key) { return data_[key]; }

    const CounterType &operator[](const key_type &key) const
    {
        return data_[key];
    }

    CounterType &at(const key_type &key) { return data_.at(key); }

    const CounterType &at(const key_type &key) const { return data_.at(key); }

    // find
    iterator find(const key_type &key) { return data_.find(key); }

    const_iterator find(const key_type &key) const { return data_.find(key); }

private:
    map_type data_;
    CounterType total_{};
};

} // namespace mj
