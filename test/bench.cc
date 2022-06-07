
#include "core/mahjong/mahjong.hpp"
#include <chrono>
#include <iostream>

void bench_Agari(const char *hand, int iters=100000)
{
    mj::Hand h(hand);
    std::cout << "Testing " << iters << " iterations of agari on " << hand << "...\n";
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iters; ++i)
        h.agari();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    double ms = 1000* elapsed_seconds.count();
    std::cout << "Completed with " << iters/ms << "Kiter/s\n";
}

void bench_Tenpai(const char *hand, int iters=100000)
{
    mj::Hand h(hand);
    std::cout << "Testing " << iters << " iterations of tenpai on " << hand << "...\n";
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iters; ++i)
        h.tenpai();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    double ms = 1000* elapsed_seconds.count();
    std::cout << "Completed with " << iters/ms << "Kiter/s\n";
}

void bench_Shanten(const char *hand, int iters=1000000)
{
    static bool do_first = true;
    mj::Hand h(hand);
    std::cout << "Testing " << iters << " iterations of shanten on " << hand << "...\n";

    if (do_first)
    {
        auto start = std::chrono::high_resolution_clock::now();
        h.shanten();
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed_seconds = end-start;
        double ms = 1000* elapsed_seconds.count();
        std::cout << "First iteration took " << ms << "ms\n";
        do_first = false;
    }

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < iters; ++i)
        h.shanten();
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;
    double ms = 1000* elapsed_seconds.count();
    std::cout << "Completed with " << iters/ms << "Kiter/s\n";
}

int main()
{
    std::cout << "Begin benchmark\n";
    const char *a_h1 = "m123345567p333w22";
    const char *a_h2 = "s33344455566677";
    const char *a_h3 = "p11123456678999";
    const char *a_h4 = "w111222333444d22";
    const char *r_h1 = "m123345567p333w2";
    const char *r_h2 = "s3334445566677";
    const char *r_h3 = "p1112345678999";
    const char *r_h4 = "w11122233444d22";
    const char *r_h5 = "m1112223334689";

    bench_Shanten(r_h1);
    bench_Shanten(r_h2);
    bench_Shanten(r_h3);
    bench_Shanten(r_h4);
    bench_Shanten(r_h5);
    bench_Shanten(a_h1);
    bench_Shanten(a_h2);
    bench_Shanten(a_h3);
    bench_Shanten(a_h4);

    bench_Agari(a_h1);
    bench_Agari(a_h2);
    bench_Agari(a_h3);
    bench_Agari(a_h4);
    bench_Tenpai(r_h1);
    bench_Tenpai(r_h2);
    bench_Tenpai(r_h3);
    bench_Tenpai(r_h4);
    bench_Tenpai(r_h5);
}