
#pragma once

#ifdef __NVCC__
#include "gpu.hpp"
#define CUDACOMPAT __host__ __device__
#define CUDAGPU __device__
#define CUDAKERNEL __global__ void
#else
#include "cpu.hpp"
#define CUDACOMPAT
#define CUDAGPU
#define CUDAKERNEL
#endif

#if __GNUC__ >= 12
#define CONSTEXPR12 constexpr
#else
#define CONSTEXPR12 inline
#endif
