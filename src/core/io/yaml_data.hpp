
#pragma once
#include "core/mahjong/mahjong.hpp"

namespace mj {
namespace io {
struct YamlData
{
    using child = std::unordered_map<std::string_view, YamlData>;
    using list = std::vector<YamlData>;
    using string = std::string_view;
    using integer = S32f;
    using decimal = F32;
    using boolean = bool;
    using direction = Dir;
    using tile = Tile;
    using probability = mod_array<F32, k_UniqueTiles>;
    using tiles = Hand4Hot;

    template <typename T>
    YamlData &operator=(T &&v)
    {
        data_ = std::forward<T>(v);
        return *this;
    }

    template <typename T>
    T *get()
    {
        return std::get_if<T>(&data_);
    }
    template <typename T>
    const T *get() const
    {
        return std::get_if<T>(&data_);
    }

    std::optional<YamlData> operator[](std::size_t i)
    {
        auto *p = std::get_if<list>(&data_);
        if (p == nullptr || i >= p->size())
            return std::nullopt;
        return (*p)[i];
    }
    std::optional<YamlData> operator[](std::size_t i) const
    {
        auto *p = std::get_if<list>(&data_);
        if (p == nullptr || i >= p->size())
            return std::nullopt;
        return (*p)[i];
    }
    std::optional<YamlData> operator[](std::string_view key)
    {
        auto *p = std::get_if<child>(&data_);
        if (p == nullptr || p->find(key) == p->end())
            return std::nullopt;
        return (*p)[key];
    }
    std::optional<const YamlData> operator[](std::string_view key) const
    {
        auto *p = std::get_if<child>(&data_);
        if (p == nullptr || p->find(key) == p->end())
            return std::nullopt;
        return p->at(key);
    }
    std::optional<YamlData> operator[](const char *key)
    {
        return operator[](std::string_view(key));
    }
    std::optional<const YamlData> operator[](const char *key) const
    {
        return operator[](std::string_view(key));
    }

private:
    std::variant<child, list, string, integer, decimal, boolean, direction,
                 tile, probability, tiles>
        data_;
};

} // namespace io
} // namespace mj
