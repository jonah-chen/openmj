
#include "core/io/yaml.hpp"
#include "test/test.hpp"

const static std::filesystem::path test_dir = "../test/core/io/in_files";

TEST t_RiichiYaml()
{
    using namespace mj::io;
    constexpr YamlData::tiles gt{1, 1, 0, 0, 0, 1, 1, 1, 2, 1, 1, 0,
                                         0, 0, 1, 1, 1, 2, 1, 1, 0, 0, 0, 1,
                                         1, 1, 2, 1, 2, 3, 4, 4, 4, 4};
    constexpr YamlData::probability gp{
        9, 1, 1, 1, 1, 1, 1, 1, 9, 9, 1, 1, 1, 1, 1, 1, 1,
        9, 9, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9};

    Yaml y(test_dir/"riichi_test.mj.yaml");
    auto a = *y["GAME"]["Player"]->get<YamlData::direction>();
    auto b = *y["GAME"]["DrawsRemain"]->get<YamlData::integer>();
    auto c = *y["TILES"]["Dead"]->get<YamlData::tiles>();
    auto d = *y["TILES"]["Discard"]->get<YamlData::probability>();
    auto e = *y["GAME"]["Description"]->get<YamlData::string>();
    auto f = *y["TILES"]["Dora"]->get<YamlData::tile>();
    assert(a == mj::Dir::k_West, "GAME/Player should be west");
    assert(b == 30, "GAME/DrawsRemain should be 30");
    assert(c == gt, "TILES/Dead is wrong");
    assert(d == gp, "TILES/Discard is wrong");
    assert(e == "This is just a test", "GAME/Description is wrong");
    assert(f.eq7(mj::Tile("3s")), "TILES/Dora is wrong");
}

int main() 
{
    t_RiichiYaml();
    return g_FailureCount;
}
