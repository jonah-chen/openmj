
#include "test/test.hpp"
#include "core/io/yaml.hpp"

int main()
{
    // mj::io::Yaml y("../example/sample.mj.yaml");
    std::cout << std::bit_cast<mj::U64>("East\0\0\0") << std::endl;
    std::cout << std::bit_cast<mj::U64>("South\0\0") << std::endl;
    std::cout << std::bit_cast<mj::U64>("West\0\0\0") << std::endl;
    std::cout << std::bit_cast<mj::U64>("North\0\0") << std::endl;
    
    return g_FailureCount;
}
