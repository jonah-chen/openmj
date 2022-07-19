/**
 * This file is for reading the specific type of config file for the OpenMJ
 * simulation and game settings. This file is not in standard YAML format,
 * as it has additional features to easily parse mahjong tiles, mahjong hands,
 * and other configs.
 */

#pragma once
#include "yaml_data.hpp"
namespace mj {
namespace io {
class Yaml
{
public:
    constexpr static std::size_t k_MaxFileSize = 0x100000; // 1 MB max
    constexpr static std::size_t k_IndentSize = 4;         // tab size
public:
    Yaml() = default;
    Yaml(std::filesystem::path fp);
    Yaml(const Yaml &);
    Yaml &operator=(const Yaml &);
    ~Yaml();

    operator bool() const { return buf_ != nullptr; }

    constexpr const char *str() const noexcept { return buf_; }
    const YamlData &operator[](const char *key) const { return *data_.at(key); }
    const YamlData &operator[](std::string_view key) const
    {
        return *data_.at(key);
    }

private:
    // we need some buffer here to store the file contents
    char *buf_{nullptr};
    std::size_t buf_size_{0};
    std::size_t *ref_count_{nullptr};

    // here is where the actual data is stored
    YamlData::Child data_;
};

} // namespace io
} // namespace mj
