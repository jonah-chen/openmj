
#include "test/test.hpp"
#include "core/io/yaml.hpp"

int main()
{
    const char *a = "hello world...";
    std::string_view s1(a, 5);
    std::string_view s2(a+6, 5);
    std::unordered_map<std::string_view, long> m;
    m[s1] = 1;
    m[s2] = 2;
    for (const auto &[k, v] : m)
        std::cout << k << ":" << v << std::endl;

    return g_FailureCount;
}
