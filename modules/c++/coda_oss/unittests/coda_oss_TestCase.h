#pragma once

#include <assert.h>
#include <stdlib.h>

#include <string>
#include <exception>
#include <iostream>

//#define TEST_ASSERT_assert_(X) assert(X)
#define TEST_ASSERT_assert_(X) if (!((X))) { assert(X); exit(EXIT_FAILURE); }

#define TEST_ASSERT_OP_(X, Y, op) ((void)testName); TEST_ASSERT_assert_((X) op (Y))
#define TEST_ASSERT_EQ(X, Y) TEST_ASSERT_OP_(X, Y, ==)
#define TEST_ASSERT_NOT_EQ(X, Y) TEST_ASSERT_OP_(X, Y, !=)
#define TEST_ASSERT_GREATER(X, Y) TEST_ASSERT_OP_(X, Y, >)
#define TEST_ASSERT_GREATER_EQ(X, Y) TEST_ASSERT_OP_(X, Y, >=)
#define TEST_ASSERT_LESSER(X, Y) TEST_ASSERT_OP_(X, Y, <)
#define TEST_ASSERT_LESSER_EQ(X, Y) TEST_ASSERT_OP_(X, Y, <=)

#define TEST_ASSERT_TRUE(X) TEST_ASSERT_EQ(X, true)
#define TEST_ASSERT_FALSE(X) TEST_ASSERT_EQ(X, false)
#define TEST_ASSERT(X) TEST_ASSERT_TRUE(X)

#define TEST_FAIL(msg) { std::cerr << "(" << testName << "): : FAILED: " << msg << "\n"; exit(EXIT_FAILURE); }


#define TEST_CASE(X) void X(const std::string& testName)
#define TEST_CHECK(X) try{ X(#X); std::clog << #X << ": PASSED\n"; } \
  catch(const std::exception& ex) { std::cerr << #X << ": FAILED: Exception thrown: " << ex.what() << "\n"; exit(EXIT_FAILURE); } \
  catch(...) { std::cerr << #X << ": FAILED: Unknown exception thrown\n"; exit(EXIT_FAILURE); }
