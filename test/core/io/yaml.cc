
#include "test/test.hpp"
#include "core/io/yaml.hpp"

int main()
{
    mj::io::Yaml y("../example/sample.mj.yaml");
    return g_FailureCount;
}
