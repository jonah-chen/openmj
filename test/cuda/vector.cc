
#include "test/test.hpp"
#include "cuda/vector.hpp"

int main()
{
    mj::vector<short, 10> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);

    assert(v.size() == 4, "size should be 4");
    assert(v[0] == 1, "v[0]");
    assert(v.front() == 1, "v.front()");
    assert(v[3] == 4, "v[3]");
    assert(v.back() == 4, "v.back()");
    
    v.pop_back();
    assert(v.size() == 3, "size should be 3");
    assert(v[0] == 1, "v[0] after pop_back");
    assert(v.back() == 3, "v.back() after pop_back");

    return g_FailureCount;
}