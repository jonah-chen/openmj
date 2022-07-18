
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
    sv.remove_prefix(std::min(sv.find_first_not_of(" \t"), sv.size()));
    sv.remove_suffix(sv.size() -
                     std::min(sv.find_last_not_of(" \t") + 1, sv.size()));
}

using Data = mj::io::YamlData;
using Child = Data::child;

/**
 * Ignore the inline comments and purge the whitspaces to ensure the value is
 * valid.
 *
 * @param value The value to be processed.
 * @return true if the value is valid
 * @return false if the value is invalid
 */
constexpr bool ignore_inline_comments(std::string_view &value)
{
    std::size_t comment_start{value.find_first_of('#')};
    std::size_t qstart{}, qendp1{}; // so loop runs
    while (comment_start >= qstart)
    {
        if (comment_start < qendp1)
            comment_start = value.find_first_of('#', qendp1);
        if (comment_start == std::string_view::npos)
        {
            purge_lead_trail_whitespace(value);
            return true;
        }
        qstart = value.find_first_of("'\"", qendp1);
        if (qstart < comment_start)
        {
            qendp1 = value.find_first_of(value[qstart], qstart + 1);
            if (qendp1 == std::string_view::npos)
                return false;
            qendp1++;
        }
    }
    value.remove_suffix(value.size() - comment_start);
    purge_lead_trail_whitespace(value);
    return true;
}

template <typename ArrayType>
bool try_parse_cs_array(std::string_view value, ArrayType &arr)
{
    const std::size_t arr_size = arr.size();
    for (std::size_t i = 0; i < arr_size - 1; ++i)
    {
        std::size_t pos = value.find(',');
        std::string_view piece = value.substr(0, pos);
        value = value.substr(pos + 1);
        if ((void *)std::from_chars(piece.data(), piece.data() + piece.size(),
                                    arr[i])
                .ec)
            return false;
    }
    if ((void *)std::from_chars(value.data(), value.data() + value.size(),
                                arr[arr_size - 1])
            .ec)
        return false;
    return true;
}

bool try_parse_string_0(std::string_view value, Data &loc)
{
    if ((value.front() == '"' && value.back() == '"') ||
        (value.front() == '\'' && value.back() == '\''))
    {
        loc = value.substr(1, value.size() - 2);
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

bool try_parse_tiles(std::string_view value, Data &loc)
{
    if (value.front() != '[' || value.back() != ']')
        return false;
    value = value.substr(1, value.size() - 2);
    // count the commas and divide the string into pieces
    if (std::count(value.begin(), value.end(), ',') != mj::k_UniqueTiles - 1)
        return false;

    Data::tiles out;
    if (!try_parse_cs_array(value, out))
        return false;
    // check for any number > 4
    if (std::find_if(out.begin(), out.end(), [](auto x) { return x > 4; }) !=
        out.end())
        return false;

    loc = out;
    return true;
}

bool try_parse_probability(std::string_view value, Data &loc)
{
    if ((value.front() != '(' || value.back() != ')') &&
        (value.front() != '[' || value.back() != ']'))
        return false;
    std::size_t end_first = value.find_first_of(value.back());
    if (end_first == value.size() - 1)
    {
        // we are expecting 34 values, one for each tile
        // count the commas
        value = value.substr(1, value.size() - 2);
        if (std::count(value.begin(), value.end(), ',') !=
            mj::k_UniqueTiles - 1)
            return false;
        mj::io::YamlData::probability out;
        if (!try_parse_cs_array(value, out))
            return false;
        loc = out;
        return true;
    }

    std::size_t comma = value.find_first_of(',', end_first);
    if (comma == std::string_view::npos)
        return false;
    std::size_t open_second = value.find_first_of(value.front(), comma);
    if (open_second == std::string_view::npos)
        return false;
    std::string_view first = value.substr(1, end_first - 1);
    std::string_view second =
        value.substr(open_second + 1, value.size() - open_second - 2);

    std::array<Data::probability::value_type, 9> normal;
    if (!try_parse_cs_array(first, normal))
        return false;
    std::array<Data::probability::value_type, 7> honors;
    if (!try_parse_cs_array(second, honors))
        return false;
    Data::probability out;
    for (std::size_t i = 0; i < mj::k_FirstHonorIdx; ++i)
        out[i] = normal[i % 9];
    for (std::size_t i = mj::k_FirstHonorIdx; i < mj::k_UniqueTiles; ++i)
        out[i] = honors[i - mj::k_FirstHonorIdx];
    loc = out;
}

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
    // TODO: if there is another character after the number that isn't empty,
    // then it's not a int
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
    // TODO: if there is another character after the number that isn't empty,
    // then it's not a float
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
    using parse_fn = std::function<bool(std::string_view, Data &)>;
    static parse_fn order[] = {try_parse_string_0, try_parse_bool,
                               try_parse_int,      try_parse_float,
                               try_parse_dir,      try_parse_tile,
                               try_parse_tiles,    try_parse_probability,
                               try_parse_list,     nullptr};

    parse_fn *fn = order;
    while (*fn)
    {
        if ((*fn)(value, loc))
            return;
        ++fn;
    }
    throw std::runtime_error("Unable to parse value: " + std::string(value));
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
            continue;
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
        if (!ignore_inline_comments(value))
            throw std::runtime_error("Yaml::Yaml: The value of " +
                                     std::string(key) + " is malformed");
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
