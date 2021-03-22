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
#  define TEST_CHECK(X) try{ X(std::string(#X)); std::cerr << #X << ": PASSED" << std::endl; } catch(except::Throwable& ex) { die_printf("%s: FAILED: Exception thrown: %s\n", std::string(#X).c_str(), ex.toString().c_str()); }
#  define TEST_ASSERT(X) if (!(X)) { die_printf("%s (%s,%s,%d): FAILED: Value should not be NULL\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_NULL(X) if ((X) != nullptr) { die_printf("%s (%s,%s,%d): FAILED: Value should be NULL\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_FALSE(X) if ((X)) { die_printf("%s (%s,%s,%d): FAILED: Value should evaluate to false\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_TRUE(X) if (!(X)) { die_printf("%s (%s,%s,%d): FAILED: Value should evaluate to true\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_EQ(X1, X2) if ((X1) != (X2)) { die_printf("%s (%s,%s,%d): FAILED: Recv'd %s, Expected %s\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__, str::toString(X1).c_str(), str::toString(X2).c_str()); }
#  define TEST_ASSERT_EQ_MSG(msg, X1, X2) if ((X1) != (X2)) die_printf("%s (%s,%d): FAILED (%s): Recv'd %s, Expected %s\n", testName.c_str(), __FILE__, __LINE__, (msg).c_str(), str::toString((X1)).c_str(), str::toString((X2)).c_str());
#  define TEST_ASSERT_NOT_EQ(X1, X2) if ((X1) == (X2)) { die_printf("%s (%s,%s,%d): FAILED: Recv'd %s should not equal %s\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__, str::toString(X1).c_str(), str::toString(X2).c_str()); }
#  define TEST_ASSERT_NOT_EQ_MSG(msg, X1, X2) if ((X1) == (X2)) die_printf("%s (%s,%d): FAILED (%s): Recv'd %s should not equal %s\n", testName.c_str(), __FILE__, __LINE__, (msg).c_str(), str::toString((X1)).c_str(), str::toString((X2)).c_str());
#  define TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS) if (std::abs((X1) - (X2)) > EPS || IS_NAN(std::abs((X1) - (X2)))) die_printf("%s (%s,%d): FAILED: Recv'd %s, Expected %s\n", testName.c_str(), __FILE__, __LINE__, str::toString((X1)).c_str(), str::toString((X2)).c_str());
#  define TEST_ASSERT_ALMOST_EQ(X1, X2) if (std::abs((X1) - (X2)) > std::numeric_limits<float>::epsilon() || IS_NAN(std::abs((X1) - (X2)))) { die_printf("%s (%s,%s,%d): FAILED: Recv'd %s, Expected %s\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__, str::toString(X1).c_str(), str::toString(X2).c_str()); }
#  define TEST_ASSERT_GREATER_EQ(X1, X2) if ((X1) < X2) { die_printf("%s (%s,%s,%d): FAILED: Value should be greater than or equal\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_GREATER(X1, X2) if ((X1) <= X2) { die_printf("%s (%s,%s,%d): FAILED: Value should be greater than\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_LESSER_EQ(X1, X2) if ((X1) > X2) { die_printf("%s (%s,%s,%d): FAILED: Value should be less than or equal\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_ASSERT_LESSER(X1, X2) if ((X1) >= X2) { die_printf("%s (%s,%s,%d): FAILED: Value should be less than\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); }
#  define TEST_FAIL(msg) die_printf("%s (%s,%s,%d): FAILED: %s\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__, str::toString(msg).c_str());
#  define TEST_EXCEPTION(X) try{ (X); die_printf("%s (%s,%s,%d): FAILED: Should have thrown exception\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); } catch (except::Throwable&){}
#  define TEST_THROWS(X) try{ (X); die_printf("%s (%s,%s,%d): FAILED: Should have thrown exception\n", testName.c_str(), __FILE__, SYS_FUNC, __LINE__); } catch (...){}
#  define TEST_SPECIFIC_EXCEPTION(X,Y) try{ (X); die_printf("%s (%s,%s,%d): FAILED: Should have thrown exception: " # Y ,  testName.c_str(), __FILE__, SYS_FUNC, __LINE__); } catch(Y&) { }  catch (except::Throwable&){ die_printf("%s (%s,%s,%d): FAILED: Should have thrown exception: " # Y ,  testName.c_str(), __FILE__, SYS_FUNC, __LINE__);}
#  define TEST_CASE(X) void X(std::string testName)

#define TEST_MAIN_catch_std_exception_ catch (const std::exception& e)  { std::cerr << e.what() << std::endl; }
#if CODA_OSS_Throwable_isa_std_exception
#define TEST_MAIN_catch_ TEST_MAIN_catch_std_exception_
#else
#define TEST_MAIN_catch_ catch (const except::Exception& ex) { std::cerr << ex.toString() << std::endl; } \
    TEST_MAIN_catch_std_exception_
#endif
#define TEST_MAIN(X) int main(int argc, char** argv) {  try { X;  return EXIT_SUCCESS; } \
TEST_MAIN_catch_ catch (...) { std::cerr << "Unknown exception\n"; } \
return EXIT_FAILURE; }

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
