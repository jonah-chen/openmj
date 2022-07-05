
#include <benchmark/benchmark.h>
#include "core/mahjong/mahjong.hpp"
#include "core/mahjong/rng.hpp"
#include <random>
#include <atomic>
#include <thread>

void gen_hand(std::mt19937_64 &rng)
{
    mj::random::RngDeck<std::mt19937_64> deck(rng);
    for (int i = 0; i < 14; ++i)
        auto j = deck(mj::k_East);
}

void calc_agari_omp(mj::Hand &h, int N)
{
#pragma omp parallel for
    for (int i = 0; i < N; ++i)
        benchmark::DoNotOptimize(h.agari());
}

void calc_agari_thread(mj::Hand &h, int N)
{
    auto fn = [&h,N]() {
        for (int i = 0; i < N/8; ++i)
            benchmark::DoNotOptimize(h.agari());
    };
    std::thread t1(fn);
    std::thread t2(fn);
    std::thread t3(fn);
    std::thread t4(fn);
    std::thread t5(fn);
    std::thread t6(fn);
    std::thread t7(fn);
    std::thread t8(fn);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
}

void calc_agari_thread_split(mj::Hand &h, int N)
{
    std::atomic<int> k{0};
    auto fn = [&h,N,&k]() {
        while (k++ < N)
            benchmark::DoNotOptimize(h.agari());
    };
    std::thread t1(fn);
    std::thread t2(fn);
    std::thread t3(fn);
    std::thread t4(fn);
    std::thread t5(fn);
    std::thread t6(fn);
    std::thread t7(fn);
    std::thread t8(fn);

    t1.join();
    t2.join();
    t3.join();
    t4.join();
    t5.join();
    t6.join();
    t7.join();
    t8.join();
}

void BM_OMP(benchmark::State& state)
{
    // std::mt19937_64 rng {std::random_device{}()};
    mj::Hand h("s11123445678999");
    for (auto _ : state)
    {
        calc_agari_omp(h, state.range(0));
    }
}

void BM_THREAD(benchmark::State& state)
{
    // std::mt19937_64 rng {std::random_device{}()};
    mj::Hand h("s11123445678999");
    for (auto _ : state)
    {
        calc_agari_thread(h, state.range(0));
    }
}

void BM_SPLIT(benchmark::State& state)
{
    // std::mt19937_64 rng {std::random_device{}()};
    mj::Hand h("s11123445678999");
    for (auto _ : state)
    {
        calc_agari_thread_split(h, state.range(0));
    }
}

BENCHMARK(BM_THREAD)->RangeMultiplier(2)->Range(1<<8, 1<<18)->Threads(8)->UseRealTime()->Unit(benchmark::kMillisecond)
    ->Repetitions(32)->DisplayAggregatesOnly();
BENCHMARK(BM_OMP)->RangeMultiplier(2)->Range(1<<8, 1<<18)->Threads(8)->UseRealTime()->Unit(benchmark::kMillisecond)
    ->Repetitions(32)->DisplayAggregatesOnly();
BENCHMARK(BM_SPLIT)->RangeMultiplier(2)->Range(1<<8, 1<<18)->Threads(8)->UseRealTime()->Unit(benchmark::kMillisecond)
    ->Repetitions(32)->DisplayAggregatesOnly();
BENCHMARK_MAIN();