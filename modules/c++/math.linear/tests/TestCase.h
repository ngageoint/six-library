#ifndef __TEST_CASE_H__
#define __TEST_CASE_H__

#include <import/sys.h>
#include <cmath>

#define TEST_CHECK(X) X(std::string(#X)); std::cout << #X << ": PASSED" << std::endl
#define TEST_ASSERT(X) if (!(X)) die_printf("%s (%s,%d): FAILED\n", me.c_str(), __FILE__, __LINE__)
#define TEST_ASSERT_EQ(X1, X2) if ((X1) != (X2)) die_printf("%s (%s,%d): FAILED: Recv'd %s, Expected %s\n", me.c_str(), __FILE__, __LINE__, str::toString((X1)).c_str(), str::toString((X2)).c_str())
#define TEST_ASSERT_ALMOST_EQ(X1, X2) if (std::abs((X1) - (X2)) > std::numeric_limits<float>::epsilon()) die_printf("%s (%s,%d): FAILED: Recv'd %s, Expected %s\n", me.c_str(), __FILE__, __LINE__, str::toString((X1)).c_str(), str::toString((X2)).c_str())
#define TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS) if (std::abs((X1) - (X2)) > EPS) die_printf("%s (%s,%d): FAILED: Recv'd %s, Expected %s\n", me.c_str(), __FILE__, __LINE__, str::toString((X1)).c_str(), str::toString((X2)).c_str())

#define TEST_CASE(X) void X(std::string me)

#endif
