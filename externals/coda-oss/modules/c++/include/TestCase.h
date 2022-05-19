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

namespace test {
inline void diePrintf(const char* format, const std::string& testName, const char* s)
{
    sys::diePrintf(format, testName.c_str(), s);
}
template<typename TX>
inline void diePrintf(const char* format, const std::string& testName, const TX& X)
{
    diePrintf(format, testName, str::toString(X).c_str());
}

inline void diePrintf(const char* format, const std::string& testName, const char* file, const char* func, int line)
{
    sys::diePrintf(format, testName.c_str(), file, func, line);
}

template<typename TX>
inline void diePrintf(const char* format, const std::string& testName, const char* file, const char* func, int line,
    const TX& X)
{
    sys::diePrintf(format, testName.c_str(), file, func, line,
        str::toString(X).c_str());
}
template<typename TX1, typename TX2>
inline void diePrintf(const char* format, const std::string& testName, const char* file, const char* func, int line,
    const TX1& X1, const TX2& X2)
{
    sys::diePrintf(format, testName.c_str(), file, func, line,
        str::toString(X1).c_str(), str::toString(X2).c_str());
}

template<typename TX1, typename TX2>
inline void diePrintf_eq(const std::string& testName, const char* file, const char* func, int line,
    const TX1& X1, const TX2& X2)
{
    diePrintf("%s (%s,%s,%d): FAILED: Recv'd %s, Expected %s\n", testName, file, func, line, X1, X2);      
}
#define CODA_OSS_test_diePrintf_eq_(X1, X2) test::diePrintf_eq(testName, __FILE__, SYS_FUNC, __LINE__, X1, X2)

template<typename TX1, typename TX2, typename TMsg>
inline void diePrintf_eq_msg(const std::string& testName, const TMsg& msg, const char* file, int line,
    const TX1& X1, const TX2& X2)
{
    sys::diePrintf("%s (%s,%d): FAILED (%s): Recv'd %s, Expected %s\n", testName.c_str(), file, line, (msg).c_str(),
        str::toString((X1)).c_str(), str::toString((X2)).c_str());       
}
#define CODA_OSS_test_diePrintf_eq_msg_(msg, X1, X2) test::diePrintf_eq_msg(testName, msg, __FILE__, __LINE__, X1, X2)

template<typename TX1, typename TX2>
inline void diePrintf_ne(const std::string& testName, const char* file, const char* func, int line,
    const TX1& X1, const TX2& X2)
{
    diePrintf("%s (%s,%s,%d): FAILED: Recv'd %s should not equal %s\n", testName, file, func, line, X1, X2);
}
#define CODA_OSS_test_diePrintf_not_eq_(X1, X2) test::diePrintf_ne(testName, __FILE__, SYS_FUNC, __LINE__, X1, X2)

template<typename TX1, typename TX2, typename TMsg>
inline void diePrintf_ne_msg(const std::string& testName, const TMsg& msg, const char* file, int line,
    const TX1& X1, const TX2& X2)
{
    sys::diePrintf("%s (%s,%d): FAILED (%s): Recv'd %s should not equal %s\n", testName.c_str(), file, line, (msg).c_str(),
        str::toString((X1)).c_str(), str::toString((X2)).c_str());       
}
#define CODA_OSS_test_diePrintf_not_eq_msg_(msg, X1, X2) test::diePrintf_ne_msg(testName, msg, __FILE__, __LINE__, X1, X2)

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
        diePrintf("%s: FAILED: Exception thrown: %s\n", testName, ex.toString());
    }
    catch (const except::Throwable11& ex)
    {
        diePrintf("%s: FAILED: Exception thrown: %s\n", testName, ex.what());
    }
}

template<typename TX>
inline void assert_(const TX& X, const std::string& testName, const char* file, const char* func, int line)
{
    if (!X)
    {
        diePrintf("%s (%s,%s,%d): FAILED: Value should not be NULL\n", testName, file, func, line);
    }
}

template<typename TX>
inline void assert_null(const TX& X, const std::string& testName,  const char* file, const char* func, int line)
{
    if ((X != nullptr) || (!(X == nullptr)))
    {
        diePrintf("%s (%s,%s,%d): FAILED: Value should be NULL\n", testName, file, func, line);
    }
}
template<typename TX>
inline void assert_not_null(const TX& X, const std::string& testName,  const char* file, const char* func, int line)
{
    if ((X == nullptr) || (!(X != nullptr)))
    {
        diePrintf("%s (%s,%s,%d): FAILED: Value should *not* be NULL\n", testName, file, func, line);
    }
}

template<typename TX>
inline void test_assert_false(const TX& X, const std::string& testName,  const char* file, const char* func, int line)
{
    if (X)
    {
        diePrintf("%s (%s,%s,%d): FAILED: Value should evaluate to false\n", testName, file, func, line);
    }
}

template<typename TX>
inline void test_assert_true(const TX& X, const std::string& testName,  const char* file, const char* func, int line)
{
    if (!X)
    {
        diePrintf("%s (%s,%s,%d): FAILED: Value should evaluate to true\n", testName, file, func, line);
    }
}

// You might be tempted to write something like
//   template<typename TX1, typename TX2>
//  inline void assert_eq(const TX1& X1, const TX2& X2, ...) {}
// A nice thought, but the compiler is able to do more implicit conversions when the code is "inline"
//    if (!(X1 == X2)) { ... }
// behaves differently when it is in the functon (X1 and X2 are arguments).  An easy example
// is std::vector::size() (size_t) compared to a literal 1 which is an "int" not "size_t".

template<typename TX1, typename TX2, typename TEPS>
inline void assert_almost_eq_eps(const TX1& X1, const TX2& X2, const TEPS& EPS,
    const std::string& testName, const char* file, const char* func, int line)
{
    const auto abs_difference = std::abs(X1 - X2);
    if (abs_difference > EPS || IS_NAN(abs_difference))
    {
        diePrintf("%s (%s,%s,%d): FAILED: Recv'd %s, Expected %s\n", testName, file, func, line, X1, X2);
    }
}
template<typename TX1, typename TX2>
inline void assert_almost_eq(const TX1& X1, const TX2& X2,
    const std::string& testName,  const char* file, const char* func, int line)
{
    const auto eps = std::numeric_limits<float>::epsilon();
    assert_almost_eq_eps(X1, X2, eps, testName, file, func, line);
}

template<typename Tmsg>
inline void fail(const Tmsg& msg,
    const std::string& testName,  const char* file, const char* func, int line)
{
    diePrintf("%s (%s,%s,%d): FAILED: %s\n", testName, file, func, line, msg);
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
}
#define TEST_CHECK(X) test::check([&](const std::string& testName) { X(testName); }, #X)
#define TEST_ASSERT(X) test::assert_(X, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_NULL(X) test::assert_null(X, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_NOT_NULL(X) test::assert_not_null(X, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_FALSE(X) test::test_assert_false(X, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_TRUE(X) test::test_assert_true(X, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS) test::assert_almost_eq_eps(X1, X2, EPS, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_ASSERT_ALMOST_EQ(X1, X2) test::assert_almost_eq(X1, X2, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_FAIL(msg) test::fail(msg, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_EXCEPTION(X) test::exception([&](){(X);}, testName, __FILE__, SYS_FUNC, __LINE__)
#define TEST_THROWS(X) test::throws([&](){(X);}, testName, __FILE__, SYS_FUNC, __LINE__)
# define TEST_SPECIFIC_EXCEPTION(X, Y) test::specific_exception<Y>([&](){(X);}, \
    "%s (%s,%s,%d): FAILED: Should have thrown exception: " # Y ,  testName, __FILE__, SYS_FUNC, __LINE__)
#  define TEST_CASE(X) void X(std::string testName)
#define TEST_MAIN(X) int main() { return test::main([&](){X;}); }
#define TEST_MAIN_ARGS(X) int main(int argc, char* argv[]) { return test::main([&](){X;}); }
/*
#  define TEST_CHECK(X) try{ X(std::string(#X)); std::cerr << #X << ": PASSED" << std::endl; } \
    catch(const except::Throwable& ex) { die_printf("%s: FAILED: Exception thrown: %s\n", std::string(#X).c_str(), ex.toString().c_str()); } \
    catch(const except::Throwable11& ex) { die_printf("%s: FAILED: Exception thrown: %s\n", std::string(#X).c_str(), ex.what()); }
#  define TEST_ASSERT(X) if (!(X)) { die_printf("%s (%s,%s,%d): FAILED: Value should not be NULL\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_NULL(X) if ((X) != nullptr) { die_printf("%s (%s,%s,%d): FAILED: Value should be NULL\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_FALSE(X) if ((X)) { die_printf("%s (%s,%s,%d): FAILED: Value should evaluate to false\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_TRUE(X) if (!(X)) { die_printf("%s (%s,%s,%d): FAILED: Value should evaluate to true\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
*/
//#define CODA_OSS_test_eq_(X1, X2) (((X1) == (X2)) && ((X2) == (X1))) // X1 == X2 means X2 == X1
#define CODA_OSS_test_eq_(X1, X2) ((X1) == (X2)) // above breaks CODA :-(
//#define CODA_OSS_test_ne_(X1, X2) (((X1) != (X2)) && ((X2) != (X1))) // X1 != X2 means X2 != X1
#define CODA_OSS_test_ne_(X1, X2) ((X1) != (X2)) // above breaks CODA :-(
//#define CODA_OSS_test_ne(X1, X2) (CODA_OSS_test_ne_(X1, X2) && !CODA_OSS_test_eq_(X1, X2))
#define CODA_OSS_test_ne(X1, X2) CODA_OSS_test_ne_(X1, X2) // above breaks CODA :-(
#define TEST_ASSERT_EQ(X1, X2) if (CODA_OSS_test_ne((X1), (X2))) { CODA_OSS_test_diePrintf_eq_(X1, X2); }
#define TEST_ASSERT_EQ_MSG(msg, X1, X2) if (CODA_OSS_test_ne((X1), (X2))) { CODA_OSS_test_diePrintf_eq_msg_(msg, X1, X2); }
//#define CODA_OSS_test_eq(X1, X2) (CODA_OSS_test_eq_(X1, X2) && !CODA_OSS_test_ne_(X1, X2))
#define CODA_OSS_test_eq(X1, X2) CODA_OSS_test_eq_(X1, X2) // above breaks CODA :-(
#define TEST_ASSERT_NOT_EQ(X1, X2) if (CODA_OSS_test_eq((X1), (X2))) { CODA_OSS_test_diePrintf_not_eq_(X1, X2); }
#define TEST_ASSERT_NOT_EQ_MSG(msg, X1, X2) if (CODA_OSS_test_eq((X1), (X2))) { CODA_OSS_test_diePrintf_not_eq_msg_(msg, X1, X2); }
#  define TEST_ASSERT_GREATER_EQ(X1, X2) if ((X1) < X2) { test::diePrintf("%s (%s,%s,%d): FAILED: Value should be greater than or equal\n", testName, __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_GREATER(X1, X2) if ((X1) <= X2) { test::diePrintf("%s (%s,%s,%d): FAILED: Value should be greater than\n", testName, __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_LESSER_EQ(X1, X2) if ((X1) > X2) { test::diePrintf("%s (%s,%s,%d): FAILED: Value should be less than or equal\n", testName, __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_LESSER(X1, X2) if ((X1) >= X2) { test::diePrintf("%s (%s,%s,%d): FAILED: Value should be less than\n", testName, __FILE__, SYS_FUNC, __LINE__); }
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
