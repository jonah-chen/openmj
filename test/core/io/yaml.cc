
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
    auto g = *y["TILES"]["Danger"]->get<YamlData::list>();
    auto h = *y["Names"].get<YamlData::list>();
    assert(a == mj::Dir::k_West, "GAME/Player should be west");
    assert(b == 30, "GAME/DrawsRemain should be 30");
    assert(c == gt, "TILES/Dead is wrong");
    assert(d == gp, "TILES/Discard is wrong");
    assert(e == "This is just a test", "GAME/Description is wrong");
    assert(f.eq7(mj::Tile("3s")), "TILES/Dora is wrong");
    assert(g[0].get<YamlData::tile>()->eq7(mj::Tile("3s")), "TILES/Danger is wrong");
    assert(g[1].get<YamlData::tile>()->eq7(mj::Tile("2s")), "TILES/Danger is wrong");
    assert(g[2].get<YamlData::tile>()->eq7(mj::Tile("8m")), "TILES/Danger is wrong");
    assert(g.size() == 3, "TILES/Danger is wrong");
    assert(*h[0].get<YamlData::string>() == "Bob's", "Bob's should be first");
    assert(*h[1].get<YamlData::string>() == "Bill,2 #\"", "Bill,2 #\" should be second");
    assert(*h[2].get<YamlData::integer>() == 34, "34 should be third");
    assert(*h[3].get<YamlData::decimal>() == 1.2f, "1.2 should be fourth");
    assert(h[4].get<YamlData::tile>()->eq7(mj::Tile("5p")), "5p should be fifth");
    assert(*h[5].get<YamlData::direction>() == mj::k_South, "South should be sixth");
    assert(*h[6].get<YamlData::boolean>() == false, "false should be seventh");
}

int main() 
{
    t_RiichiYaml();
    return g_FailureCount;
}
