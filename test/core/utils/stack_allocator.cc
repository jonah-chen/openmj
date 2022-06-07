
#include "test/test.hpp"
#include "core/utils/stack_allocator.hpp"
#include <cstdlib>

void *operator new(std::size_t size)
{
    g_FailureCount++;
    return malloc(size);
}

struct S5
{
    char r[5];
};

int main()
{
    mj::s_Vector<int, 9> v1;
    for (int i = 0; i < 9; ++i)
        v1.push_back(i);

    mj::s_Vector<S5, 9> v2;
    for (int i = 0; i < 9; ++i)
        v2.push_back(S5());

    return g_FailureCount;
}