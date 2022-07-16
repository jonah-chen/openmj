
#pragma once

#define MJ_RETURN 
#define TEST static void

#include <iostream>
#include <string>
#include <cstdlib>

#if __cplusplus >= 202002L
#include <concepts>
#define FLOATPOINT_T template<std::floating_point T>
#define EQ_COMPARE_T template<std::equality_comparable T>
#else
#define FLOATPOINT_T template<typename T>
#define EQ_COMPARE_T template<typename T>
#endif

#ifdef assert
#undef assert
#endif

static int g_FailureCount = 0;

/**
 * Assert that a condition is truthy. Note this does not terminate execution
 * on failure but will cause the entire test to fail.
 * 
 * @tparam T type of condition
 * @param t condition
 * @param msg message to print if assertion failed
 */
template<typename T>
void assert(const T& t, const std::string& msg="Test failed")
{
    if (!t)
    {
        std::cout << msg << std::endl;
        g_FailureCount++;
    }
}

/**
 * Assert that a condition is truthy. If the assertion fails, the test will
 * terminate. This should be used if the results of this test will be used
 * in subsequent tests.
 * 
 * @tparam T type of condition
 * @param t condition
 * @param msg message to print if assertion failed
 * @param ec error code to return if assertion failed. defaults to 1
 */
template<typename T>
void assert_crit(const T& t, const std::string& msg="Critical Error", int ec=255)
{
    if (!t)
    {
        std::cerr << msg << std::endl;
        exit(ec);
    }
}


#if __cplusplus >= 202002L
/**
 * Weak equality between floating point values.
 * 
 * @tparam T type of floating point value
 * @param a first value
 * @param b second value
 * @param eps tolerance
 * @return true if a and b are within eps of each other
 * @return false if a and b are not within eps of each other
 */
FLOATPOINT_T
constexpr bool eq(T a, T b, T eps=1e-6)
{
    return std::abs(a - b) < eps;
}

#endif

/**
 * Does the same thing as == for non-floating point values. This is here so eq
 * can be used for all comparisons.
 */
EQ_COMPARE_T
constexpr bool eq(T a, T b, double eps=1e-6)
{
    return a == b;
}

EQ_COMPARE_T
void assert_eq(T a, T b, const std::string &msg="", double eps=1e-6)
{
    assert(eq(a, b, eps), msg + "|Expected " + std::to_string(a) + " to equal " + std::to_string(b));
}

EQ_COMPARE_T
void assert_neq(T a, T b, const std::string &msg="", double eps=1e-6)
{
    assert(!eq(a, b, eps), msg + "|Expected " + std::to_string(a) + " to not equal " + std::to_string(b));
}

EQ_COMPARE_T
void assert_crit_eq(T a, T b, const std::string &msg="", double eps=1e-6)
{
    assert_crit(eq(a, b, eps), msg + "|Expected " + std::to_string(a) + " to equal " + std::to_string(b));
}

EQ_COMPARE_T
void assert_crit_neq(T a, T b, const std::string &msg="", double eps=1e-6)
{
    assert_crit(!eq(a, b, eps), msg + "|Expected " + std::to_string(a) + " to not equal " + std::to_string(b));
}
