#pragma once

#include <stdint.h>
#include <string>

#include "CppUnitTest.h"
#include "include/TestCase.h"

#undef TEST_CHECK
#undef TEST_ASSERT
#undef TEST_ASSERT_NULL 
#undef TEST_ASSERT_TRUE
#undef TEST_ASSERT_FALSE
#undef TEST_MAIN
#undef TEST_CASE
#define TEST_CHECK(X) 
#define TEST_MAIN(X)
#define TEST_ASSERT_NULL(X) Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNull((X))
#define TEST_ASSERT_TRUE(X) Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue((X))
#define TEST_ASSERT_FALSE(X) Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsFalse((X))
#define TEST_ASSERT(X) TEST_ASSERT_TRUE(X)
#define TEST_CASE(X) TEST_METHOD(X)

namespace test
{
    struct Assert final
    {
        static void FailOnCondition(bool condition, const unsigned short* message, const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo* pLineInfo);    
        static std::wstring GetAssertMessage(bool equality, const std::string& expected, const std::string& actual, const wchar_t *message);
    };
#define CODA_OSS_equals_message_(expected, actual, message) reinterpret_cast<const unsigned short*>(test::Assert::GetAssertMessage(true, str::toString(expected),  str::toString(actual), message).c_str())
#define CODA_OSS_not_equals_message_(notExpected, actual, message) reinterpret_cast<const unsigned short*>(test::Assert::GetAssertMessage(false,  str::toString(notExpected),  str::toString(actual), message).c_str())

// see Assert::AreEqual<>
template<typename TExpected, typename TActual>
inline void diePrintf_eq(const TExpected& expected, const TActual& actual,
    const wchar_t* message = NULL, const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo* pLineInfo = NULL)
{
	Assert::FailOnCondition(expected == actual, CODA_OSS_equals_message_(expected, actual, message), pLineInfo);
}

// see Assert::AreNotEqual<>
template<typename TExpected, typename TActual>
inline void diePrintf_ne(const TExpected& notExpected, const TActual& actual,
    const wchar_t* message = NULL, const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo* pLineInfo = NULL)
{
	Assert::FailOnCondition(!(notExpected == actual), CODA_OSS_not_equals_message_(notExpected, actual, message), pLineInfo);
}
}
#undef CODA_OSS_test_diePrintf_eq_
#undef CODA_OSS_test_diePrintf_ne_
#define CODA_OSS_test_diePrintf_eq_(X1, X2) test::diePrintf_eq(X1, X2)
#define CODA_OSS_test_diePrintf_ne_(X1, X2) test::diePrintf_ne(X1, X2)

template <typename TX1, typename TX2>
inline void test_assert_greater_(const TX1& X1, const TX2& X2)
{
	Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(X1 > X2);
}
#undef TEST_ASSERT_GREATER
#define TEST_ASSERT_GREATER(X1, X2) test_assert_greater_(X1, X2)

#undef TEST_ASSERT_ALMOST_EQ_EPS
#define TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS) { Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(X1, X2, EPS); Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(X2, X1, EPS); }
namespace test
{
inline void assert_almost_eq(float X1, float X2)
{
    constexpr auto EPS = static_cast<float>(0.0001);
    TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS);
}
inline void assert_almost_eq(double X1, double X2)
{
    constexpr auto EPS = static_cast<double>(0.0001);
    TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS);
}
inline void assert_almost_eq(long double X1, long double X2)
{
    assert_almost_eq(static_cast<double>(X1), static_cast<double>(X2));
}
}

#undef TEST_ASSERT_ALMOST_EQ 
#define TEST_ASSERT_ALMOST_EQ(X1, X2) test::assert_almost_eq(X1, X2)

#undef TEST_ASSERT_EQ_MSG
#define TEST_ASSERT_EQ_MSG(msg, X1, X2) Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(msg.c_str()); TEST_ASSERT_EQ(X1, X2)

#undef TEST_EXCEPTION
#undef TEST_THROWS
#define TEST_EXCEPTION(X) try { (X); TEST_ASSERT_FALSE(false); } catch (...) { TEST_ASSERT_TRUE(true); }
#define TEST_THROWS(X) TEST_EXCEPTION(X)


