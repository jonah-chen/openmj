
#pragma once

#include <cstdio>
#include <cstdlib>
#include <stdexcept>

class AssertionError : public std::exception
{
public:
    AssertionError() = default;
    constexpr const char *what() const noexcept override
    {
        return "Assertion failed. Terminate Called.";
    }
};

#define MJ_ALWAYS_THROW(COND, CLASS, MSG) \
    if (COND) throw CLASS(MSG)

#if MJ_LOGGING > 0
#warning "critical logging enabled"
#define MJ_ASSERT_CRIT(x, ...) \
    if (!(x)) { \
        fprintf(stderr, "Critical Assertion Failed: %s:%s(%d):\n", __FILE__, __func__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        throw AssertionError(); \
    }
#define MJ_CRIT(...) \
    fprintf(stderr, "Critical: %s:%s(%d):\n", __FILE__, __func__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n")
#define MJ_THROW(COND, CLASS, MSG) \
    if (COND) throw CLASS(MSG)
#define MJ_EXCEPT_CRIT
#else
#define MJ_ASSERT_CRIT(x, ...)
#define MJ_CRIT(...)
#define MJ_THROW(COND, CLASS, MSG)
#define MJ_EXCEPT_CRIT noexcept
#endif

#if MJ_LOGGING > 1
#warning "warn logging enabled"
#define MJ_ASSERT(x, ...) \
    if (!(x)) { \
        fprintf(stderr, "Assertion Failed: %s:%s(%d):\n", __FILE__, __func__, __LINE__); \
        fprintf(stderr, __VA_ARGS__); \
        fprintf(stderr, "\n"); \
        throw AssertionError(); \
    }
#define MJ_WARN(...) \
    fprintf(stderr, "Warning: %s:%s(%d):\n", __FILE__, __func__, __LINE__); \
    fprintf(stderr, __VA_ARGS__); \
    fprintf(stderr, "\n")
#define MJ_EXCEPT_WARN
#else
#define MJ_ASSERT(x, ...)
#define MJ_WARN(...)
#define MJ_EXCEPT_WARN noexcept
#endif

#if MJ_LOGGING > 2
#warning "info logging enabled"
#define MJ_INFO(...) \
    fprintf(stdout, "Info: %s:%s(%d):\n", __FILE__, __func__, __LINE__); \
    fprintf(stdout, __VA_ARGS__); \
    fprintf(stdout, "\n")
#else
#define MJ_INFO(...)
#endif
