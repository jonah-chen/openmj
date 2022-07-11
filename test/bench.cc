
// #include "core/mahjong/mahjong.hpp"
// #include <chrono>
// #include <iostream>

// void bench_Agari(const char *hand, int iters=100000)
// {
//     mj::Hand h(hand);
//     std::cout << "Testing " << iters << " iterations of agari on " << hand << "...\n";
//     auto start = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < iters; ++i)
//         h.agari();
//     auto end = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double> elapsed_seconds = end-start;
//     double ms = 1000* elapsed_seconds.count();
//     std::cout << "Completed with " << iters/ms << "Kiter/s\n";
// }

// void bench_Tenpai(const char *hand, int iters=100000)
// {
//     mj::Hand h(hand);
//     std::cout << "Testing " << iters << " iterations of tenpai on " << hand << "...\n";
//     auto start = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < iters; ++i)
//         h.tenpai();
//     auto end = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double> elapsed_seconds = end-start;
//     double ms = 1000* elapsed_seconds.count();
//     std::cout << "Completed with " << iters/ms << "Kiter/s\n";
// }

// void bench_Shanten(const char *hand, int iters=1000000)
// {
//     static bool do_first = true;
//     mj::Hand h(hand);
//     std::cout << "Testing " << iters << " iterations of shanten on " << hand << "...\n";

//     if (do_first)
//     {
//         auto start = std::chrono::high_resolution_clock::now();
//         h.shanten();
//         auto end = std::chrono::high_resolution_clock::now();
//         std::chrono::duration<double> elapsed_seconds = end-start;
//         double ms = 1000* elapsed_seconds.count();
//         std::cout << "First iteration took " << ms << "ms\n";
//         do_first = false;
//     }

//     auto start = std::chrono::high_resolution_clock::now();
//     for (int i = 0; i < iters; ++i)
//         h.shanten();
//     auto end = std::chrono::high_resolution_clock::now();
//     std::chrono::duration<double> elapsed_seconds = end-start;
//     double ms = 1000* elapsed_seconds.count();
//     std::cout << "Completed with " << iters/ms << "Kiter/s\n";
// }

// int main()
// {
//     std::cout << "Begin benchmark\n";
//     const char *a_h1 = "m123345567p333w22";
//     const char *a_h2 = "s33344455566677";
//     const char *a_h3 = "p11123456678999";
//     const char *a_h4 = "w111222333444d22";
//     const char *r_h1 = "m123345567p333w2";
//     const char *r_h2 = "s3334445566677";
//     const char *r_h3 = "p1112345678999";
//     const char *r_h4 = "w11122233444d22";
//     const char *r_h5 = "m1112223334689";

//     bench_Shanten(r_h1);
//     bench_Shanten(r_h2);
//     bench_Shanten(r_h3);
//     bench_Shanten(r_h4);
//     bench_Shanten(r_h5);
//     bench_Shanten(a_h1);
//     bench_Shanten(a_h2);
//     bench_Shanten(a_h3);
//     bench_Shanten(a_h4);

//     bench_Agari(a_h1);
//     bench_Agari(a_h2);
//     bench_Agari(a_h3);
//     bench_Agari(a_h4);
//     bench_Tenpai(r_h1);
//     bench_Tenpai(r_h2);
//     bench_Tenpai(r_h3);
//     bench_Tenpai(r_h4);
//     bench_Tenpai(r_h5);
// }
#if MJ_LOGGING_LEVEL > 0
#warning "Compiling Benchmark in Debug mode" 
#endif

#ifndef MJ_BENCH_REPS
#define MJ_BENCH_REPS 32
#endif

#include <benchmark/benchmark.h>
#include <tuple>

#include "core/mahjong/mahjong.hpp"
#include <iostream>
#include <random>
#include "core/mahjong/rng.hpp"

static void DoSetup(const benchmark::State &state)
{
    mj::Hand h;
    h.shanten();
}

mj::U64 seed = 48;
std::mt19937 rng(seed);

mj::Hand rand_hand()
{
    mj::Hand hand;
    mj::random::RngDeck<std::mt19937> rng_deck(rng);
    for (int i = 0; i < 14; ++i)
        hand.push_back(rng_deck(mj::k_East));
    return hand;
}


static void Rand_Shanten(benchmark::State& state) {
    for (auto _ : state) {
        state.PauseTiming();
        mj::random::Rng34 rng34(rng);
        mj::Hand4Hot h4 = rng34(state.range(0));
        state.ResumeTiming();
        benchmark::DoNotOptimize(mj::shanten(h4, 0, mj::k_ModeAll));
    }
}

template <typename... Args>
void Shanten(benchmark::State& state, Args&&... args) {
    auto args_tuple = std::make_tuple(std::move(args)...);
    mj::Hand4Hot h4 = mj::Hand(std::get<0>(args_tuple)).hand_4hot();
    for (auto _ : state) {
        benchmark::DoNotOptimize(mj::shanten(h4, 0, mj::k_ModeAll));
    }
}

template <typename... Args>
void Tenpai(benchmark::State& state, Args&&... args) {
    auto args_tuple = std::make_tuple(std::move(args)...);
    mj::Hand h4 = mj::Hand(std::get<0>(args_tuple));
    for (auto _ : state) {
        benchmark::DoNotOptimize(h4.tenpai());
    }
}

template <typename... Args>
void Agari(benchmark::State& state, Args&&... args) {
    auto args_tuple = std::make_tuple(std::move(args)...);
    mj::Hand h4 = mj::Hand(std::get<0>(args_tuple));
    for (auto _ : state) {
        benchmark::DoNotOptimize(h4.agari());
    }
}

BENCHMARK_CAPTURE(Shanten, m123345567p333w22, "m123345567p333w22")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);
BENCHMARK_CAPTURE(Shanten, s3334445566677, "s3334445566677")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);
BENCHMARK_CAPTURE(Shanten, p1112345678999, "p1112345678999")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);
BENCHMARK_CAPTURE(Shanten, w11122233444d22, "w11122233444d22")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);
BENCHMARK_CAPTURE(Shanten, m1112223334689, "m1112223334689")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);

BENCHMARK_CAPTURE(Tenpai, m123345567p333w2, "m123345567p333w2")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);
BENCHMARK_CAPTURE(Tenpai, s3334445566677, "s3334445566677")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);
BENCHMARK_CAPTURE(Tenpai, p1112345678999, "p1112345678999")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);
BENCHMARK_CAPTURE(Tenpai, w11122233444d22, "w11122233444d22")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);
BENCHMARK_CAPTURE(Tenpai, m1112223334689, "m1112223334689")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);

BENCHMARK_CAPTURE(Agari, m123345567p333w22, "m123345567p333w22")->Setup(DoSetup)
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);
BENCHMARK_CAPTURE(Agari, s33344455566677, "s33344455566677")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);
BENCHMARK_CAPTURE(Agari, p11123456678999, "p11123456678999")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);
BENCHMARK_CAPTURE(Agari, w111222333444d22, "w111222333444d22")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);
BENCHMARK_CAPTURE(Agari, m11122233346899, "m11122233346899")
    ->Repetitions(MJ_BENCH_REPS)->DisplayAggregatesOnly(true);

BENCHMARK(Rand_Shanten)->DenseRange(10, 22);

BENCHMARK_MAIN();
