
#pragma once
#include "core/mahjong/mahjong.hpp"

namespace mj {
namespace io {
struct YamlData
{
    using Child = std::unordered_map<std::string_view, std::unique_ptr<YamlData>>;
    using List = std::vector<std::unique_ptr<YamlData>>;
    using String = std::string_view;
    using Integer = S32f;
    using Decimal = F32;
    using Boolean = bool;
    using Direction = Dir;
    using Tile = typename mj::Tile;
    using Probability = mod_array<F32, k_UniqueTiles>;
    using Tiles = Hand4Hot;

    YamlData() = default;

    template <typename T, typename = std::enable_if_t<!std::is_same_v<
                              std::remove_reference_t<T>, YamlData>>>
    YamlData(T &&v)
    {
        data_ = std::forward<T>(v);
    }

    template <typename T, typename = std::enable_if_t<!std::is_same_v<
                              std::remove_reference_t<T>, YamlData>>>
    YamlData &operator=(T &&v)
    {
        data_ = std::forward<T>(v);
        return *this;
    }

    template <typename T, typename = std::enable_if_t<std::is_same_v<T, String>>>
    constexpr String get() const
    {
        return std::get<String>(data_);
    }
    template <typename T, typename = std::enable_if_t<std::is_same_v<T, Integer>>>
    constexpr Integer get() const
    {
        return std::get<Integer>(data_);
    }
    template <typename T, typename = std::enable_if_t<std::is_same_v<T, Decimal>>>
    constexpr Decimal get() const
    {
        return std::get<Decimal>(data_);
    }
    template <typename T, typename = std::enable_if_t<std::is_same_v<T, Boolean>>>
    constexpr Boolean get() const
    {
        return std::get<Boolean>(data_);
    }
    template <typename T, typename = std::enable_if_t<std::is_same_v<T, Direction>>>
    constexpr Direction get() const
    {
        return std::get<Direction>(data_);
    }
    template <typename T, typename = std::enable_if_t<std::is_same_v<T, Tile>>>
    constexpr Tile get() const
    {
        return std::get<Tile>(data_);
    }
    template <typename T, typename = std::enable_if_t<std::is_same_v<T, Probability>>>
    constexpr const Probability &get() const
    {
        return std::get<Probability>(data_);
    }
    template <typename T, typename = std::enable_if_t<std::is_same_v<T, Tiles>>>
    constexpr const Tiles &get() const
    {
        return std::get<Tiles>(data_);
    }

    const YamlData &operator[](std::string_view key) const
    {
        return *std::get<Child>(data_).at(key);
    }

    const YamlData &operator[](const char *key) const
    {
        return operator[](std::string_view(key));
    }

    template<typename T, typename = std::enable_if_t<std::is_integral_v<T>>>
    const YamlData &operator[](T index) const
    {
        return *std::get<List>(data_).at(index);
    }

    static Child &make_child(std::string_view name, Child &parent)
    {
        auto &&out = parent.emplace(name, std::make_unique<YamlData>());
        MJ_ASSERT_CRIT(out.second, "child already exists");
        return std::get<Child>(out.first->second->data_);
    }

    template <typename T>
    T *get_if()
    {
        return std::get_if<T>(&data_);
    }
    template <typename T>
    const T *get_if() const
    {
        return std::get_if<T>(&data_);
    }

private:
    std::variant<Child, List, String, Integer, Decimal, Boolean, Direction,
                 Tile, Probability, Tiles>
        data_;
};

} // namespace io
} // namespace mj
