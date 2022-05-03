/* =========================================================================
 * This file is part of CODA-OSS
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * CODA-OSS is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __TEST_CASE_H__
#define __TEST_CASE_H__
#pragma once

#ifdef __cplusplus

#  include <cmath>
#  include <limits>
# include <exception>
# include <iostream>
#  include <import/sys.h>
#  include <import/str.h>
#  include <import/except.h>

#  define IS_NAN(X) X != X

namespace coda_oss { namespace test {
inline void diePrintf(const char* format, const std::string& testName, const char* s)
{
    sys::diePrintf(format, testName.c_str(), s);
}

inline void diePrintf(const char* format, const std::string& testName, const char* file, const char* func, int line)
{
    sys::diePrintf(format, testName.c_str(), file, func, line);
}

template<typename TX1, typename TX2>
inline void diePrintf(const char* format, const std::string& testName, const char* file, const char* func, int line,
    TX1&& X1, TX2&& X2)
{
    sys::diePrintf(format, testName.c_str(), file, func, line,
        str::toString(X1).c_str(), str::toString(X2).c_str());
}

template <typename TFunc>
inline void check(TFunc f, const std::string& testName)
{
    try
    {
        f(testName);
        std::cerr << testName  << ": PASSED" << std::endl;
    }
    catch (const except::Throwable& ex)
    {
        diePrintf("%s: FAILED: Exception thrown: %s\n", testName, ex.toString().c_str());
    }
    catch (const except::Throwable11& ex)
    {
        diePrintf("%s: FAILED: Exception thrown: %s\n", testName, ex.what());
    }
}

template<typename TX>
inline void assert_(TX&& X, const std::string& testName, const char* file, const char* func, int line)
{
    if (!X)
    {
        diePrintf("%s (%s,%s,%d): FAILED: Value should not be NULL\n", testName, file, func, line);
    }
}

template<typename TX>
inline void assert_null(TX&& X, const std::string& testName,  const char* file, const char* func, int line)
{
    if ((X != nullptr) || (!(X == nullptr)))
    {
        diePrintf("%s (%s,%s,%d): FAILED: Value should be NULL\n", testName, file, func, line);
    }
}

template<typename TX>
inline void test_assert_false(TX&& X, const std::string& testName,  const char* file, const char* func, int line)
{
    if (X)
    {
        diePrintf("%s (%s,%s,%d): FAILED: Value should evaluate to false\n", testName, file, func, line);
    }
}

template<typename TX>
inline void test_assert_true(TX&& X, const std::string& testName,  const char* file, const char* func, int line)
{
    if (!X)
    {
        diePrintf("%s (%s,%s,%d): FAILED: Value should evaluate to true\n", testName, file, func, line);
    }
}

// These cause warnings (or even errors, depending on compile settings) if the types have different signedness.
// assert_eq(v.size(), 1, ...);
/*
template<typename TX1, typename TX2>
inline void assert_eq(TX1&& X1, TX2&& X2,
    const std::string& testName,  const char* file, const char* func, int line)
{
    if ((X1 != X2) || (!(X1 == X2)))
    {
        diePrintf("%s (%s,%s,%d): FAILED: Recv'd %s, Expected %s\n", testName, file, func, line, X1, X2);
    }
}
template<typename TX1, typename TX2>
inline void assert_eq_msg(const std::string& msg, TX1&& X1, TX2&& X2,
    const std::string& testName,  const char* file, int line)
{
    if ((X1 != X2) || (!(X1 == X2)))
    {
        die_printf("%s (%s,%d): FAILED (%s): Recv'd %s, Expected %s\n", testName.c_str(), file, line, 
                   msg.c_str(), str::toString(X1).c_str(), str::toString(X2).c_str());
    }
}

template<typename TX1, typename TX2>
inline void assert_not_eq(TX1&& X1, TX2&& X2,
    const std::string& testName, const char* file, const char* func, int line)
{
    if ((X1 == X2) || (!(X1 != X2)))
    {
        diePrintf("%s (%s,%s,%d): FAILED: Recv'd %s should not equal %s\n", testName, file, func, line, X1, X2);
    }
}
*/
template<typename TX1, typename TX2, typename TEPS>
inline void assert_almost_eq_eps(TX1&& X1, TX2&& X2, TEPS&& EPS,
    const std::string& testName, const char* file, const char* func, int line)
{
    const auto abs_difference = std::abs(X1 - X2);
    if (abs_difference > EPS || IS_NAN(abs_difference))
    {
        diePrintf("%s (%s,%d): FAILED: Recv'd %s, Expected %s\n", testName, file, func, line, X1, X2);
    }
}
template<typename TX1, typename TX2>
inline void assert_almost_eq(TX1&& X1, TX2&& X2,
    const std::string& testName,  const char* file, const char* func, int line)
{
    const auto abs_difference = std::abs(X1 - X2);
    if (abs_difference > std::numeric_limits<float>::epsilon() || IS_NAN(abs_difference))
    {
        diePrintf("%s (%s,%s,%d): FAILED: Recv'd %s, Expected %s\n", testName, file, func, line, X1, X2);
    }
}
/*
template<typename TX1, typename TX2>
inline void assert_greater_eq(TX1&& X1, TX2&& X2,
    const std::string& testName,  const char* file, const char* func, int line)
{
    if ((X1 < X2) || (!(X1 >= X2)))
    {
        diePrintf("%s (%s,%s,%d): FAILED: Value should be greater than or equal\n", testName, file, func, line);
    }
}

template<typename TX1, typename TX2>
inline void assert_greater(TX1&& X1, TX2&& X2,
    const std::string& testName,  const char* file, const char* func, int line)
{
    if ((X1 <= X2) || (!(X1 > X2)))
    {
        diePrintf("%s (%s,%s,%d): FAILED: Value should be greater than\n", testName, file, func, line);
    }
}

template<typename TX1, typename TX2>
inline void assert_lesser_eq(TX1&& X1, TX2&& X2,
    const std::string& testName,  const char* file, const char* func, int line)
{
    if ((X1 > X2) || (!(X1 <= X2)))
    {
        diePrintf("%s (%s,%s,%d): FAILED: Value should be less than or equal\n", testName, file, func, line);
    }
}

template<typename TX1, typename TX2>
inline void assert_lesser(TX1&& X1, TX2&& X2,
    const std::string& testName,  const char* file, const char* func, int line)
{
    if ((X1 >= X2) || (!(X1 < X2)))
    {
        diePrintf("%s (%s,%s,%d): FAILED: Value should be less than\n", testName, file, func, line);
    }
}
*/
template<typename Tmsg>
inline void fail(Tmsg&& msg,
    const std::string& testName,  const char* file, const char* func, int line)
{
    die_printf("%s (%s,%s,%d): FAILED: %s\n", testName.c_str(), file, func, line, str::toString(msg).c_str());
}

template<typename TFunc>
inline void exception(TFunc f,
    const std::string& testName,  const char* file, const char* func, int line)
{
    try
    {
        f();
        diePrintf("%s (%s,%s,%d): FAILED: Should have thrown exception\n", testName, file, func, line);
    }
    catch (const except::Throwable&) { }
    catch (const except::Throwable11&) { }
}

template<typename TFunc>
inline void throws(TFunc f,
    const std::string& testName,  const char* file, const char* func, int line)
{
    try
    {
        f();
        diePrintf("%s (%s,%s,%d): FAILED: Should have thrown exception\n", testName, file, func, line);
    }
    catch (...) { }
}

template <typename TException, typename TFunc>
inline void specific_exception(TFunc f,
                                    const char* format, const std::string& testName, const char* file, const char* func, int line)
{
    try
    {
        f();
        diePrintf(format, testName, file, func, line);
    }
    catch (const TException&) {  }
    catch (const except::Throwable&)
    {
        diePrintf(format, testName, file, func, line);
    }
    catch (const except::Throwable11&)
    {
        diePrintf(format, testName, file, func, line);
    }
}

template <typename TFunc>
inline int main(TFunc f)
{
    try
    {
        f();
        return EXIT_SUCCESS;
    }
    catch (const except::Exception& ex)
    {
        std::cerr << ex.toString() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Unknown exception\n";
    }
    return EXIT_FAILURE;
}
}}
#define TEST_CHECK(X) coda_oss::test::check([&](const std::string& testName) { X(testName); }, #X)
#define TEST_ASSERT(X) coda_oss::test::assert_(X, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_NULL(X) coda_oss::test::assert_null(X, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_FALSE(X) coda_oss::test::test_assert_false(X, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_TRUE(X) coda_oss::test::test_assert_true(X, testName, __FILE__, SYS_FUNC, __LINE__)
/*
#define TEST_ASSERT_EQ(X1, X2) coda_oss::test::assert_eq(X1, X2, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_EQ_MSG(msg, X1, X2) coda_oss::test::assert_eq_msg(msg, X1, X2, testName, __FILE__, __LINE__)
#define TEST_ASSERT_NOT_EQ(X1, X2) coda_oss::test::assert_not_eq(X1, X2, testName, __FILE__, SYS_FUNC, __LINE__)
*/
#define TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS) coda_oss::test::assert_almost_eq_eps(X1, X2, EPS, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_ALMOST_EQ(X1, X2) coda_oss::test::assert_almost_eq(X1, X2, testName, __FILE__, SYS_FUNC, __LINE__)
/*
#define TEST_ASSERT_GREATER_EQ(X1, X2) coda_oss::test::assert_greater_eq(X1, X2, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_GREATER(X1, X2) coda_oss::test::assert_greater(X1, X2, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_LESSER_EQ(X1, X2) coda_oss::test::assert_lesser_eq(X1, X2, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_LESSER(X1, X2) coda_oss::test::assert_lesser(X1, X2, testName, __FILE__, SYS_FUNC, __LINE__)
*/
#define TEST_FAIL(msg) coda_oss::test::fail(msg, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_EXCEPTION(X) coda_oss::test::exception([&](){(X);}, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_THROWS(X) coda_oss::test::throws([&](){(X);}, testName, __FILE__, SYS_FUNC, __LINE__)
# define TEST_SPECIFIC_EXCEPTION(X, Y) coda_oss::test::specific_exception<Y>([&](){(X);}, \
    "%s (%s,%s,%d): FAILED: Should have thrown exception: " # Y ,  testName, __FILE__, SYS_FUNC, __LINE__)
#  define TEST_CASE(X) void X(std::string testName)
#define TEST_MAIN(X) int main(int argc, char** argv) { return coda_oss::test::main([&](){X;}); }
/*
#  define TEST_CHECK(X) try{ X(std::string(#X)); std::cerr << #X << ": PASSED" << std::endl; } \
    catch(const except::Throwable& ex) { die_printf("%s: FAILED: Exception thrown: %s\n", std::string(#X).c_str(), ex.toString().c_str()); } \
    catch(const except::Throwable11& ex) { die_printf("%s: FAILED: Exception thrown: %s\n", std::string(#X).c_str(), ex.what()); }
#  define TEST_ASSERT(X) if (!(X)) { die_printf("%s (%s,%s,%d): FAILED: Value should not be NULL\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_NULL(X) if ((X) != nullptr) { die_printf("%s (%s,%s,%d): FAILED: Value should be NULL\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_FALSE(X) if ((X)) { die_printf("%s (%s,%s,%d): FAILED: Value should evaluate to false\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_TRUE(X) if (!(X)) { die_printf("%s (%s,%s,%d): FAILED: Value should evaluate to true\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
*/
#  define TEST_ASSERT_EQ(X1, X2) if ((X1) != (X2)) { die_printf("%s (%s,%s,%d): FAILED: Recv'd %s, Expected %s\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__, str::toString(X1).c_str(), str::toString(X2).c_str()); }
#  define TEST_ASSERT_EQ_MSG(msg, X1, X2) if ((X1) != (X2)) die_printf("%s (%s,%d): FAILED (%s): Recv'd %s, Expected %s\n", testName.c_str(), __FILE__, __LINE__, (msg).c_str(), str::toString((X1)).c_str(), str::toString((X2)).c_str());
#  define TEST_ASSERT_NOT_EQ(X1, X2) if ((X1) == (X2)) { die_printf("%s (%s,%s,%d): FAILED: Recv'd %s should not equal %s\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__, str::toString(X1).c_str(), str::toString(X2).c_str()); }
#  define TEST_ASSERT_NOT_EQ_MSG(msg, X1, X2) if ((X1) == (X2)) die_printf("%s (%s,%d): FAILED (%s): Recv'd %s should not equal %s\n", testName.c_str(), __FILE__, __LINE__, (msg).c_str(), str::toString((X1)).c_str(), str::toString((X2)).c_str());
/*
#  define TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS) if (std::abs((X1) - (X2)) > EPS || IS_NAN(std::abs((X1) - (X2)))) die_printf("%s (%s,%d): FAILED: Recv'd %s, Expected %s\n", testName.c_str(), __FILE__, __LINE__, str::toString((X1)).c_str(), str::toString((X2)).c_str());
#  define TEST_ASSERT_ALMOST_EQ(X1, X2) if (std::abs((X1) - (X2)) > std::numeric_limits<float>::epsilon() || IS_NAN(std::abs((X1) - (X2)))) { die_printf("%s (%s,%s,%d): FAILED: Recv'd %s, Expected %s\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__, str::toString(X1).c_str(), str::toString(X2).c_str()); }
*/
#  define TEST_ASSERT_GREATER_EQ(X1, X2) if ((X1) < X2) { die_printf("%s (%s,%s,%d): FAILED: Value should be greater than or equal\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_GREATER(X1, X2) if ((X1) <= X2) { die_printf("%s (%s,%s,%d): FAILED: Value should be greater than\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_LESSER_EQ(X1, X2) if ((X1) > X2) { die_printf("%s (%s,%s,%d): FAILED: Value should be less than or equal\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_LESSER(X1, X2) if ((X1) >= X2) { die_printf("%s (%s,%s,%d): FAILED: Value should be less than\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
/*
#  define TEST_FAIL(msg) die_printf("%s (%s,%s,%d): FAILED: %s\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__, str::toString(msg).c_str());
#  define TEST_EXCEPTION(X) try{ (X); die_printf("%s (%s,%s,%d): FAILED: Should have thrown exception\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); } catch (const except::Throwable&){} catch (const except::Throwable11&){}
#  define TEST_THROWS(X) try{ (X); die_printf("%s (%s,%s,%d): FAILED: Should have thrown exception\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); } catch (...){}
#  define TEST_SPECIFIC_EXCEPTION(X,Y) try{ (X); die_printf("%s (%s,%s,%d): FAILED: Should have thrown exception: " # Y ,  testName.c_str(), __FILE__, SYS_FUNC, __LINE__); } catch(const Y&) { }  \
    catch (const except::Throwable&){ die_printf("%s (%s,%s,%d): FAILED: Should have thrown exception: " # Y ,  testName.c_str(), __FILE__, SYS_FUNC, __LINE__);} \
    catch (const except::Throwable11&){ die_printf("%s (%s,%s,%d): FAILED: Should have thrown exception: " # Y ,  testName.c_str(), __FILE__, SYS_FUNC, __LINE__);} #  define TEST_CASE(X) void X(std::string testName)
#  define TEST_CASE(X) void X(std::string testName)
#define TEST_MAIN(X) int main(int argc, char** argv) {  try { X;  return EXIT_SUCCESS; } \
    catch (const except::Exception& ex) { std::cerr << ex.toString() << std::endl; } \
    catch (const std::exception& e)  { std::cerr << e.what() << std::endl; } \
    catch (...) { std::cerr << "Unknown exception\n"; } \
    return EXIT_FAILURE; }
*/

#else /* C only */
#  include <math.h>
#  include <stdlib.h>
#  include <string.h>
#  include <stdio.h>

/*  Copied from sys/Conf.h  */
#  define TEST_FILE __FILE__
#  define TEST_LINE __LINE__
#  if defined(__GNUC__)
#      define TEST_FUNC __PRETTY_FUNCTION__
#  elif __STDC_VERSION__ < 199901
#    define TEST_FUNC "unknown function"
#  else /* Should be c99 */
#    define TEST_FUNC __func__
#  endif

#  define TEST_CHECK(X) X(#X); fprintf(stderr, "%s : PASSED\n", #X);
#  define TEST_CHECK_ARGS(X) X(#X,argc,argv); fprintf(stderr, "%s : PASSED\n", #X);
#  define TEST_ASSERT(X) if (!(X)) { \
    fprintf(stderr, "%s (%s,%s,%d) : FAILED: Value should not be NULL\n", testName, TEST_FILE, TEST_FUNC, TEST_LINE); \
    exit(EXIT_FAILURE); \
}
#  define TEST_ASSERT_NULL(X) if ((X) != NULL) { \
    fprintf(stderr, "%s (%s,%s,%d) : FAILED: Value should be NULL\n", testName, TEST_FILE, TEST_FUNC, TEST_LINE); \
    exit(EXIT_FAILURE); \
}
#  define TEST_ASSERT_EQ_STR(X1, X2) if (strcmp((X1), (X2)) != 0) { \
    fprintf(stderr, "%s (%s,%s,%d) : FAILED: Recv'd %s, Expected %s\n", testName, TEST_FILE, TEST_FUNC, TEST_LINE, X1, X2); \
    exit(EXIT_FAILURE); \
}
#  define TEST_ASSERT_EQ_INT(X1, X2) if ((X1) != (X2)) { \
    fprintf(stderr, "%s (%s,%s,%d) : FAILED: Recv'd %d, Expected %d\n", testName, TEST_FILE, TEST_FUNC, TEST_LINE, (int)X1, (int)X2); \
    exit(EXIT_FAILURE); \
}
/* TODO use epsilon for comparing floating points */
#  define TEST_ASSERT_EQ_FLOAT(X1, X2) if (fabs((X1) - (X2)) > .0000001f) { \
    fprintf(stderr, "%s (%s,%s,%d) : FAILED: Recv'd %f, Expected %f\n", testName, TEST_FILE, TEST_FUNC, TEST_LINE, X1, X2); \
    exit(EXIT_FAILURE); \
}

#  define TEST_CASE(X) void X(const char* testName)
#  define TEST_CASE_ARGS(X) void X(const char* testName, int argc, char **argv)

#endif

#endif
