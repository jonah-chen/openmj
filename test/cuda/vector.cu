
#include "core/adt/vector.hpp"
#include "test/test.hpp"

struct vec3
{
    float x, y, z;
    float *c;
    vec3() : x(0), y(0), z(0), c(nullptr) {}
    vec3(const vec3 &v) : x(v.x), y(v.y), z(v.z), c(v.c + 1) {}
    vec3(float x, float y = 1.f, float z = 2.f) : x(x), y(y), z(z), c(nullptr)
    {
    }
};

CUDAKERNEL test1(int *a)
{
    mj::vector<int, 20> v(10, 2);
    for (auto i : v)
        atomicAdd(a, i);
    v[1] = 0;
    for (auto i : v)
        atomicAdd(a + 1, i);
    v.push_back(3);
    for (auto i : v)
        atomicAdd(a + 2, i);
    atomicAdd(a + 3, v.size());

    v.erase(v.begin() + 1); // erase the 0
    for (auto i : v)
        atomicAdd(a + 4, i);

    v.erase(v.begin(), v.begin() + 2); // erase two 2s
    for (auto i : v)
        atomicAdd(a + 5, i);
    v.insert(v.begin() + 4, 21);
    atomicAdd(a + 6, v.size());
    atomicAdd(a + 7, v[4]);

    v.pop_back();
    for (auto i : v)
        atomicAdd(a + 8, i);
    v.clear();
    atomicAdd(a + 9, v.size());
}

CUDAKERNEL test2(vec3 *a)
{
    mj::vector<vec3, 20> v; // default constructor
}

int main()
{
    int *da1, a1[10];
    cudaMalloc(&da1, sizeof(int) * 10);
    cudaMemset(da1, 0, sizeof(int) * 10);
    test1<<<10, 10>>>(da1);
    cudaMemcpy(a1, da1, sizeof(int) * 10, cudaMemcpyDeviceToHost);
    assert(a1[0] == 2000, "fill initialization");
    assert(a1[1] == 1800, "operator[] nonconst");
    assert(a1[2] == 2100, "push_back");
    assert(a1[3] == 1100, "size");
    assert(a1[4] == 2100, "erase");
    assert(a1[5] == 1700, "erase range");
    assert(a1[6] == 900, "size after erase and inserts");
    assert(a1[7] == 2100, "insert");
    assert(a1[8] == 4100, "pop_back");
    assert(a1[9] == 0, "clear");
    cudaFree(da1);

    vec3 *da2, a2[10];
    cudaMalloc(&da2, sizeof(int) * 10);
    cudaMemset(da2, 0, sizeof(int) * 10);
    test2<<<10, 10>>>(da2);
    cudaMemcpy(a2, da2, sizeof(int) * 10, cudaMemcpyDeviceToHost);
    cudaFree(da2);
}