
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

using Data = mj::io::YamlData;
using Child = Data::child;

/**
 * Parse the MJ_YAML string value and store it.
 * 
 * @param value The string view of the string value to parse.
 * @param loc The location the data is stored in.
 * @return true 
 * @return false 
 */
bool parse_value(std::string_view value, Data &loc) 
{

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

        if (line[0] == '#' || line[0] == '\n') // comment
        {
            auto pos = line.find('\n');
            if (pos == std::string_view::npos)
                return {};
            line = line.substr(pos + 1);
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

        if (value.empty()) // we are in a new layer
            new_layer_name = key;
        else
            parse_value(value, layer[key]);
        str = line.substr(end_line);
    }
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
}

Yaml::~Yaml()
{
    if (buf_ && --*ref_count_ == 0)
        delete[] buf_;
}

} // namespace io
} // namespace mj
