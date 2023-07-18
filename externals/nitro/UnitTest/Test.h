#pragma once

#include "nitf/unittests/Test.h"
#include "TestCase.h"

#define TEST_ASSERT_EQ_INT(X1, X2) TEST_ASSERT_EQ(X2, X1)
#define TEST_ASSERT_EQ_STR(X1, X2) TEST_ASSERT_EQ(std::string(X1), std::string(X2))
#define TEST_ASSERT_EQ_FLOAT(X1, X2) TEST_ASSERT_EQ(static_cast<float>(X1), static_cast<float>(X2))