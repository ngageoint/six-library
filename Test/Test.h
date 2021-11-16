#pragma once

#include "gtest/gtest.h"

// use GTest macros to mimic existing unit-tests

#define TEST_ASSERT(X) EXPECT_TRUE(X)

namespace six
{
	template<typename T, typename U>
	inline void test_expect_eq_(const T& t, const U& u)
	{
		EXPECT_EQ(t, u);
	}
	template<typename T, typename U>
	inline void test_expect_streq_(const T& t, const U& u)
	{
		EXPECT_STREQ(t, u);
	}
	template<typename T, typename U>
	inline void test_expect_ne_(const T& t, const U& u)
	{
		EXPECT_NE(t, u);
	}
}

template<typename T, typename U>
inline void test_assert_eq_(const T& t, const U& u)
{
	six::test_expect_eq_(t, u);
	six::test_expect_eq_(u, t);
	TEST_ASSERT(!(t != u));
	TEST_ASSERT(!(u != t));
}
#define TEST_ASSERT_EQ(X1, X2) test_assert_eq_(X1, X2)
#define TEST_ASSERT_EQ_INT(X1, X2) TEST_ASSERT_EQ(X1, X2)
template<typename T, typename U>
inline void test_assert_str_eq_(const T& t, const U& u)
{
	six::test_expect_streq_(t, u);
	six::test_expect_streq_(u, t);
}
#define TEST_ASSERT_EQ_STR(X1, X2) test_assert_str_eq_(X1, X2)
#define TEST_ASSERT_EQ_FLOAT(X1, X2) EXPECT_FLOAT_EQ(static_cast<float>(X1), static_cast<float>(X2))

#define TEST_ASSERT_NULL(X) TEST_ASSERT_EQ((X), nullptr)
#define TEST_ASSERT_TRUE(X) TEST_ASSERT_EQ((X), true)
#define TEST_ASSERT_FALSE(X) TEST_ASSERT_EQ((X), false)

template<typename T, typename U>
inline void test_assert_not_eq_(const T& t, const U& u)
{
	six::test_expect_ne_(t, u);
	six::test_expect_ne_(u, t);
	TEST_ASSERT(!(t == u));
	TEST_ASSERT(!(u == t));
}
#define TEST_ASSERT_NOT_EQ(X1, X2) test_assert_not_eq_(X1, X2)

#define TEST_ASSERT_GREATER(X1, X2) EXPECT_GT(X1, X2)

#define TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS) EXPECT_NEAR(X1, X2, EPS)
#define TEST_ASSERT_ALMOST_EQ(X1, X2) TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, 0.0001)

#define TEST_ASSERT_EQ_MSG(msg, X1, X2) SCOPED_TRACE(msg); TEST_ASSERT_EQ(X1, X2)

#define TEST_EXCEPTION(X) EXPECT_ANY_THROW((X))
#define TEST_THROWS(X) EXPECT_ANY_THROW((X))

#define TEST_MAIN(X)
