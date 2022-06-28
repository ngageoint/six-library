#pragma once

#include <stdint.h>
#include <string>

#include "CppUnitTest.h"

namespace Microsoft{ namespace VisualStudio {namespace CppUnitTestFramework
{
template<>
inline std::wstring ToString(const uint16_t& q)
{
	return std::to_wstring(q);
}
}}}

#define TEST_ASSERT(X) Assert::IsTrue(X)

template<typename T, typename U>
inline void test_assert_eq_(T&& t, U&& u)
{
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(t, u);
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(u, t);
}
#define TEST_ASSERT_EQ(X1, X2) test_assert_eq_(X1, X2);
#define TEST_ASSERT_EQ_INT(X1, X2) TEST_ASSERT_EQ(X2, X1)
#define TEST_ASSERT_EQ_STR(X1, X2) TEST_ASSERT_EQ(std::string(X1), std::string(X2))
#define TEST_ASSERT_EQ_FLOAT(X1, X2) TEST_ASSERT_EQ(static_cast<float>(X1), static_cast<float>(X2))

#define TEST_ASSERT_NULL(X) Assert::IsNull((X))
#define TEST_ASSERT_NOT_NULL(X) Assert::IsNotNull((X))
#define TEST_ASSERT_TRUE(X) Assert::IsTrue((X))
#define TEST_ASSERT_FALSE(X) Assert::IsFalse((X))

template<typename T, typename U>
inline void test_assert_not_eq_(T&& t, U&& u)
{
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(t, u);
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreNotEqual(u, t);
}
#define TEST_ASSERT_NOT_EQ(X1, X2) test_assert_not_eq_(X1, X2);
template<typename T, typename U>
inline void test_assert_greater_(T&& t, U&& u)
{
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(t > u);
}
#define TEST_ASSERT_GREATER(X1, X2) test_assert_greater_(X1, X2)

#define TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS) { Assert:AreEqual(X1, X2, EPS); Assert:AreEqual(X2, X1, EPS); }
#define TEST_ASSERT_ALMOST_EQ(X1, X2) TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, 0.0001)

#define TEST_ASSERT_EQ_MSG(msg, X1, X2) Logger::WriteMessage(msg.c_str()); TEST_ASSERT_EQ(X1, X2)

#define TEST_EXCEPTION(X) try { (X); TEST_ASSERT_FALSE(false); } catch (...) { TEST_ASSERT_TRUE(true); }
#define TEST_THROWS(X) TEST_EXCEPTION(X)

#define TEST_MAIN(X)
