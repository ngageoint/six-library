#pragma once

#include "gtest/gtest.h"

// use GTest macros to mimic existing unit-tests

#define TEST_ASSERT(X) EXPECT_TRUE(X)
#define TEST_ASSERT_NULL(X) TEST_ASSERT((X) == NULL)
#define TEST_ASSERT_TRUE(X) TEST_ASSERT((X) == true)
#define TEST_ASSERT_FALSE(X) TEST_ASSERT((X) == false)

#define TEST_ASSERT_EQ(X1, X2) EXPECT_EQ(X1, X2)
#define TEST_ASSERT_NOT_EQ(X1, X2) EXPECT_NE(X1, X2)
#define TEST_ASSERT_EQ_INT(X1, X2) TEST_ASSERT_EQ(X1, X2)
#define TEST_ASSERT_EQ_STR(X1, X2) EXPECT_STREQ(X1, X2)
#define TEST_ASSERT_EQ_FLOAT(X1, X2) EXPECT_FLOAT_EQ(static_cast<float>(X1), static_cast<float>(X2))

#define TEST_ASSERT_GREATER(X1, X2) EXPECT_GT(X1, X2)


#define TEST_ASSERT_EQ_MSG(msg, X1, X2) SCOPED_TRACE(msg); TEST_ASSERT_EQ(X1, X2)

#define TEST_EXCEPTION(X) EXPECT_ANY_THROW((X))

#define TEST_MAIN(X)
