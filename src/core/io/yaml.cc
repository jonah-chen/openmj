
#include "yaml.hpp"

namespace {
std::size_t read_file(char **buf, std::filesystem::path fp,
                      std::size_t max_size)
{
    // open file
    std::ifstream ifs(fp);
    MJ_ALWAYS_THROW(!ifs.is_open(), std::runtime_error,
                    "Yaml::Yaml: failed to open file");

    // get file size
    ifs.seekg(0, std::ios::end);
    const std::size_t buf_size = ifs.tellg();
    ifs.seekg(0, std::ios::beg);

    MJ_ALWAYS_THROW(buf_size > max_size, std::runtime_error,
                    "Yaml::Yaml: file size is too large");
    MJ_THROW(buf_size == 0, std::runtime_error,
             "Yaml::Yaml: file size is zero");

    // allocate buffer
    *buf = new char[buf_size + sizeof(std::size_t)];
    MJ_ALWAYS_THROW(!*buf, std::runtime_error,
                    "Yaml::Yaml: failed to allocate buffer");

    // read file
    ifs.read(*buf, buf_size);
    MJ_ALWAYS_THROW(!ifs.good(), std::runtime_error,
                    "Yaml::Yaml: failed to read file");

    return buf_size;
}

constexpr void purge_lead_trail_whitespace(std::string_view &sv) noexcept
{
    sv = sv.substr(sv.find_first_not_of(" \t"));
    sv = sv.substr(0, sv.find_last_not_of(" \t") + 1);
}

using Data = mj::io::YamlData;
using Child = Data::child;

bool try_parse_string_0(std::string_view value, Data &loc)
{
    if ((value.front() == '"' && value.back() == '"') ||
        (value.front() == '\'' && value.back() == '\''))
    {
        value = value.substr(1, value.size() - 2);
        return true;
    }
    return false;
}

bool try_parse_list(std::string_view value, Data &loc)
{
    if (value.front() == '[' && value.back() == ']')
    {
        value = value.substr(1, value.size() - 2);
        // TODO: parse list elements
    }
    return false;
}

bool try_parse_hand(std::string_view value, Data &loc) {}

bool try_parse_tile(std::string_view value, Data &loc)
{
    using namespace mj;
    if (value.size() != 2 && value.size() != 3)
        return false;
    if (value[0] < '1' && value[0] > '9')
        return false;
    if (value[1] < Tile::n2suit_offset ||
        value[1] >= Tile::n2suit_offset + Tile::n2suit_size)
        return false;

    Suit suit = Tile::n2suit[value[1]];
    if (suit == Suit::End)
        return false;
    Tile tile(suit, value[0] - '1');
    if (value.size() == 3)
    {
        switch (value[2])
        {
        case 'e':
        case 'E':
            tile.set_dir(k_East);
            break;
        case 's':
        case 'S':
            tile.set_dir(k_South);
            break;
        case 'w':
        case 'W':
            tile.set_dir(k_West);
            break;
        case 'n':
        case 'N':
            tile.set_dir(k_North);
            break;
        default:
            break;
        }
    }
    loc = tile;
    return true;
}

bool try_parse_dir(std::string_view value, Data &loc)
{
    using namespace mj;
    if (value == "East" || value == "east" || value == "EAST" || value == "E")
        loc = k_East;
    else if (value == "South" || value == "south" || value == "SOUTH" ||
             value == "S")
        loc = k_South;
    else if (value == "West" || value == "west" || value == "WEST" ||
             value == "W")
        loc = k_West;
    else if (value == "North" || value == "north" || value == "NORTH" ||
             value == "N")
        loc = k_North;
    else
        return false;
    return true;
}

bool try_parse_bool(std::string_view value, Data &loc)
{
    if (value == "true" || value == "True" || value == "TRUE" ||
        value == "yes" || value == "Yes" || value == "YES" || value == "on" ||
        value == "On" || value == "ON")
        loc = true;
    else if (value == "false" || value == "False" || value == "FALSE" ||
             value == "no" || value == "No" || value == "NO" ||
             value == "off" || value == "Off" || value == "OFF")
        loc = false;
    else
        return false;
    return true;
}

bool try_parse_int(std::string_view value, Data &loc)
{
    mj::io::YamlData::integer result;
    auto [ptr, err] =
        std::from_chars(value.data(), value.data() + value.size(), result);
    if ((void *)err)
        return false;
    if (ptr != value.data() + value.size())
        return false;
    loc = result;
    return true;
}

bool try_parse_float(std::string_view value, Data &loc)
{
    mj::io::YamlData::decimal result;
    auto [ptr, err] =
        std::from_chars(value.data(), value.data() + value.size(), result);
    if ((void *)err)
        return false;
    if (ptr != value.data() + value.size())
        return false;
    loc = result;
    return true;
}

/**
 * Parse the MJ_YAML string value and store it.
 *
 * @param value The string view of the string value to parse.
 * @param loc The location the data is stored in.
 */
void parse_value(std::string_view value, Data &loc)
{
    using parse_fn = bool (*)(std::string_view, Data &);
    parse_fn order[] = {
        try_parse_string_0, try_parse_bool, try_parse_int, try_parse_float,
        try_parse_dir,      try_parse_tile, nullptr};
    parse_fn *fn = order;
    while (*fn)
    {
        if ((*fn)(value, loc))
            return;
        ++fn;
    }
}

/**
 * Parse the layer of the YAML file.
 *
 * @param str String view describing the the begining of the layer to the end
 * of the file
 * @param indent Indentation of the layer
 * @param tabsize The size of the tab, in characters. Tabs are discouraged.
 * @param layer The layer to parse.
 * @return std::string_view The remaining file to parse if there were a parent.
 */
std::string_view parse_layer(std::string_view str, std::size_t indent,
                             std::size_t tabsize, Child &layer)
{
    std::string_view new_layer_name;
    while (!str.empty())
    {
        auto whitespaces = str.find_first_not_of(" \t");
        std::string_view spaces = str.substr(0, whitespaces);
        std::string_view line = str.substr(whitespaces);
        if (line.empty())
            return line;

        whitespaces +=
            std::count(spaces.begin(), spaces.end(), '\t') * (tabsize - 1);

        if (line.front() == '#' || line.front() == '\n') // comment
        {
            std::size_t pos = line.find('\n');
            if (pos == std::string_view::npos)
                return {};
            str = line.substr(pos + 1);
            continue;
        }

        if (whitespaces < indent) // we are done with this layer
            return str;
        if (whitespaces > indent) // we are in a new layer
        {
            MJ_ALWAYS_THROW(new_layer_name.empty(), std::runtime_error,
                            "Yaml::Yaml: Improper indentation");
            str = parse_layer(str, whitespaces, tabsize,
                              *layer[new_layer_name].get<Child>());
            new_layer_name = {};
        }

        std::size_t colon_pos = line.find(':');
        std::size_t end_line = line.find('\n');
        if (end_line == std::string_view::npos)
            end_line = line.size();

        MJ_ALWAYS_THROW(colon_pos >= end_line, std::runtime_error,
                        "Yaml::Yaml: Improper key value pair format");

        std::string_view key = line.substr(0, colon_pos);
        std::string_view value =
            line.substr(colon_pos + 1, end_line - colon_pos - 1);
        purge_lead_trail_whitespace(key);
        purge_lead_trail_whitespace(value);
        if (value.empty()) // we are in a new layer
            new_layer_name = key;
        else
            parse_value(value, layer[key]);
        str = line.substr(end_line + 1);
    }
    return str;
}

} // namespace

namespace mj {
namespace io {
Yaml::Yaml(std::filesystem::path fp)
{
    // check if file ends with .mj.yml or .mj.yaml
    MJ_THROW(fp.extension() != ".yml" && fp.extension() != ".yaml",
             std::runtime_error,
             "Yaml::Yaml: file extension must be .mj.yml or .mj.yaml");
    MJ_THROW(fp.stem().extension() != ".mj", std::runtime_error,
             "Yaml::Yaml: file extension must be .mj.yml or .mj.yaml");

    buf_size_ = read_file(&buf_, fp, k_MaxFileSize);
    ref_count_ = reinterpret_cast<std::size_t *>(buf_ + buf_size_);
    *ref_count_ = 1;

    // try parse
    parse_layer({buf_, buf_size_}, 0, k_IndentSize, data_);
}

Yaml::~Yaml()
{
    if (buf_ && --*ref_count_ == 0)
        delete[] buf_;
}

} // namespace io
} // namespace mj
