/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __TEST_H__
#define __TEST_H__

#ifdef __cplusplus

#include <import/sys.h>
#include <import/str.h>

#define CHECK(X) X(std::string(#X)); std::cout << #X << ": PASSED" << std::endl
#define TEST_ASSERT(X) if (!(X)) { die_printf("%s (%s,%s,%d): FAILED: Value should not be NULL\n", testName.c_str(), __FILE__, __FUNC__, __LINE__) }
#define TEST_ASSERT_NULL(X) if ((X) != NULL) { die_printf("%s (%s,%s,%d): FAILED: Value should be NULL\n", testName.c_str(), __FILE__, __FUNC__, __LINE__) }
#define TEST_ASSERT_EQ(X1, X2) if ((X1) != (X2)) { die_printf("%s (%s,%s,%d): FAILED: Recv'd %s, Expected %s\n", testName.c_str(), __FILE__, __FUNC__, __LINE__, str::toString(X1).c_str(), str::toString(X2).c_str()) }
#define TEST_ASSERT_ALMOST_EQ(X1, X2) if (fabs((X1) - (X2)) > std::numeric_limits<float>::epsilon()) { die_printf("%s (%s,%s,%d): FAILED: Recv'd %s, Expected %s\n", testName.c_str(), __FILE__, __FUNC__, __LINE__, str::toString(X1).c_str(), str::toString(X2).c_str()) }
#define TEST_CASE(X) void X(std::string testName)

#else

#   include <stdlib.h>
#   include <string.h>
#   include <stdio.h>

/*  Negotiate the 'context'  */
#define TEST_FILE __FILE__
#define TEST_LINE __LINE__
#if defined(__GNUC__)
#    define TEST_FUNC __PRETTY_FUNCTION__
#elif __STDC_VERSION__ < 199901
#    define TEST_FUNC "unknown function"
#else /* Should be c99 */
#    define TEST_FUNC __func__
#endif

#define CHECK(X) X(#X); fprintf(stderr, "%s : PASSED\n", #X);
#define CHECK_ARGS(X) X(#X,argc,argv); fprintf(stderr, "%s : PASSED\n", #X);
#define TEST_ASSERT(X) if (!(X)) { \
    fprintf(stderr, "%s (%s,%s,%d) : FAILED: Value should not be NULL\n", testName, TEST_FILE, TEST_FUNC, TEST_LINE); \
    exit(EXIT_FAILURE); \
}
#define TEST_ASSERT_NULL(X) if ((X) != NULL) { \
    fprintf(stderr, "%s (%s,%s,%d) : FAILED: Value should be NULL\n", testName, TEST_FILE, TEST_FUNC, TEST_LINE); \
    exit(EXIT_FAILURE); \
}
#define TEST_ASSERT_EQ_STR(X1, X2) if (strcmp((X1), (X2)) != 0) { \
    fprintf(stderr, "%s (%s,%s,%d) : FAILED: Recv'd %s, Expected %s\n", testName, TEST_FILE, TEST_FUNC, TEST_LINE, X1, X2); \
    exit(EXIT_FAILURE); \
}
#define TEST_ASSERT_EQ_INT(X1, X2) if ((X1) != (X2)) { \
    fprintf(stderr, "%s (%s,%s,%d) : FAILED: Recv'd %d, Expected %d\n", testName, TEST_FILE, TEST_FUNC, TEST_LINE, (int)X1, (int)X2); \
    exit(EXIT_FAILURE); \
}
/* TODO use epsilon for comparing floating points */
#define TEST_ASSERT_EQ_FLOAT(X1, X2) if (fabs((X1) - (X2)) > .0000001f) { \
    fprintf(stderr, "%s (%s,%s,%d) : FAILED: Recv'd %f, Expected %f\n", testName, TEST_FILE, TEST_FUNC, TEST_LINE, X1, X2); \
    exit(EXIT_FAILURE); \
}

#define TEST_CASE(X) void X(const char* testName)
#define TEST_CASE_ARGS(X) void X(const char* testName, int argc, char **argv)

#endif

#endif
