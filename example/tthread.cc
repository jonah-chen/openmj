
#include <thread>
#include <atomic>
#include <array>
#include <iostream>
#include <random>
inline auto workers = std::thread::hardware_concurrency();

void parallel(int N, int threads)
{
    std::cout << "Parallel: " << N << " threads: " << threads << std::endl;
    std::array<std::thread, 8> threads_;
    std::atomic<int> counter{0};
    
}

int main()
{
    std::atomic_long l = 0;
    std::thread t1(f, std::ref(l));
    std::thread t2(f, std::ref(l));
    t1.join();
    t2.join();
    std::cout << l << std::endl;
    return 0;
}