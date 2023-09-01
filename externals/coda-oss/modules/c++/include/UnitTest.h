#pragma once

#include <stdint.h>
#include <string>

#include "TestCase.h"

#if !_MSC_VER
#error "This file is for use with a Visual Studio 'UnitTest' project."
#endif

// Use our various TEST macros with a Visual Studio "UnitTest" project.
#include <CppUnitTest.h>

#undef TEST_CHECK
#undef TEST_ASSERT
#undef TEST_ASSERT_NULL 
#undef TEST_ASSERT_NOT_NULL 
#undef TEST_ASSERT_TRUE
#undef TEST_ASSERT_FALSE
#undef TEST_MAIN
#undef TEST_CASE
#define TEST_CHECK(X) 
#define TEST_MAIN(X)
#define TEST_ASSERT_NULL(X) testName, Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNull(X)
#define TEST_ASSERT_NOT_NULL(X) testName, Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsNotNull(X)
#define TEST_ASSERT_TRUE(X) testName, Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(X)
#define TEST_ASSERT_FALSE(X) testName, Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsFalse(X)
#define TEST_ASSERT(X) TEST_ASSERT_TRUE(X)
#define CODA_OSS_testMethod_(X)  testMethod ## _ ## X
#define TEST_CASE(X) TEST_METHOD(X) { CODA_OSS_testMethod_(X)(#X); } void CODA_OSS_testMethod_(X)(std::string testName)

namespace test
{
    struct Assert final
    {
        static void FailOnCondition(bool condition, const unsigned short* message, const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo* pLineInfo);    
        static std::wstring GetAssertMessage(bool equality, const std::string& expected, const std::string& actual, const wchar_t *message);
    };
#define CODA_OSS_equals_message_(expected, actual, message) reinterpret_cast<const unsigned short*>(test::Assert::GetAssertMessage(true, test::toString(expected),  test::toString(actual), message).c_str())
#define CODA_OSS_not_equals_message_(notExpected, actual, message) reinterpret_cast<const unsigned short*>(test::Assert::GetAssertMessage(false,  test::toString(notExpected),  test::toString(actual), message).c_str())
#define CODA_OSS_message_(message) reinterpret_cast<const unsigned short*>(test::Assert::GetAssertMessage(false,  "", "", message).c_str())

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
    Assert::FailOnCondition(notExpected != actual, CODA_OSS_not_equals_message_(notExpected, actual, message), pLineInfo);
}

template <typename TX1, typename TX2>
inline void diePrintf_ge(const TX1& X1, const TX2& X2,
    const wchar_t* message = NULL, const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo* pLineInfo = NULL)
{
    Assert::FailOnCondition(X1 >= X2, CODA_OSS_message_(message), pLineInfo);
}
template <typename TX1, typename TX2>
inline void diePrintf_gt(const TX1& X1, const TX2& X2,
    const wchar_t* message = NULL, const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo* pLineInfo = NULL)
{
    Assert::FailOnCondition(X1 > X2, CODA_OSS_message_(message), pLineInfo);
}
template <typename TX1, typename TX2>
inline void diePrintf_le(const TX1& X1, const TX2& X2,
    const wchar_t* message = NULL, const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo* pLineInfo = NULL)
{
    Assert::FailOnCondition(X1 <= X2, CODA_OSS_message_(message), pLineInfo);
}
template <typename TX1, typename TX2>
inline void diePrintf_lt(const TX1& X1, const TX2& X2,
    const wchar_t* message = NULL, const Microsoft::VisualStudio::CppUnitTestFramework::__LineInfo* pLineInfo = NULL)
{
    Assert::FailOnCondition(X1 < X2, CODA_OSS_message_(message), pLineInfo);
}

}
#undef CODA_OSS_test_diePrintf_eq_
#undef CODA_OSS_test_diePrintf_not_eq_
#define CODA_OSS_test_diePrintf_eq_(X1, X2) testName, ::test::diePrintf_eq(X1, X2)
#define CODA_OSS_test_diePrintf_not_eq_(X1, X2) testName, ::test::diePrintf_ne(X1, X2)

#undef CODA_OSS_test_diePrintf_greater_eq_
#undef CODA_OSS_test_diePrintf_greater_
#undef CODA_OSS_test_diePrintf_lesser_eq_
#undef CODA_OSS_test_diePrintf_lesser_
#define CODA_OSS_test_diePrintf_greater_eq_(X1, X2) testName, ::test::diePrintf_ge(X1, X2)
#define CODA_OSS_test_diePrintf_greater_(X1, X2) testName, ::test::diePrintf_gt(X1, X2)
#define CODA_OSS_test_diePrintf_lesser_eq_(X1, X2) testName, ::test::diePrintf_le(X1, X2)
#define CODA_OSS_test_diePrintf_lesser_(X1, X2) testName, ::test::diePrintf_lt(X1, X2)

template <typename TX1, typename TX2>
inline void test_assert_greater_(const TX1& X1, const TX2& X2)
{
    Microsoft::VisualStudio::CppUnitTestFramework::Assert::IsTrue(X1 > X2);
}
#undef TEST_ASSERT_GREATER
#define TEST_ASSERT_GREATER(X1, X2) testName, test_assert_greater_(X1, X2)

#undef TEST_ASSERT_ALMOST_EQ_EPS
#define TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS) { (void)testName; Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(X1, X2, EPS); Microsoft::VisualStudio::CppUnitTestFramework::Assert::AreEqual(X2, X1, EPS); }
namespace test
{
inline void assert_almost_eq(const std::string& testName, float X1, float X2)
{
    constexpr auto EPS = static_cast<float>(0.0001);
    TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS);
}
inline void assert_almost_eq(const std::string& testName, double X1, double X2)
{
    constexpr auto EPS = static_cast<double>(0.0001);
    TEST_ASSERT_ALMOST_EQ_EPS(X1, X2, EPS);
}
inline void assert_almost_eq(const std::string& testName, long double X1, long double X2)
{
    assert_almost_eq(testName, static_cast<double>(X1), static_cast<double>(X2));
}
}

#undef TEST_ASSERT_ALMOST_EQ 
#define TEST_ASSERT_ALMOST_EQ(X1, X2) test::assert_almost_eq(testName, X1, X2)

#undef TEST_ASSERT_EQ_MSG
#define TEST_ASSERT_EQ_MSG(msg, X1, X2) testName, Microsoft::VisualStudio::CppUnitTestFramework::Logger::WriteMessage(msg.c_str()); TEST_ASSERT_EQ(X1, X2)

#undef TEST_FAIL_MSG
#define TEST_FAIL_MSG(msg) { (void)testName; Microsoft::VisualStudio::CppUnitTestFramework::Assert::Fail(str::toWString(msg).c_str()); }

#undef TEST_EXCEPTION
#undef TEST_THROWS
#undef TEST_SPECIFIC_EXCEPTION
#define TEST_EXCEPTION(X) (void)testName; try{ (X); TEST_FAIL(#X " should have thrown."); } CODA_OSS_TEST_EXCEPTION_catch_
#define TEST_THROWS(X) (void)testName; try{ (X); TEST_FAIL(#X " should have thrown."); } catch (...){ TEST_ASSERT_TRUE(true); }
#define TEST_SPECIFIC_EXCEPTION(X, Y) testName, Microsoft::VisualStudio::CppUnitTestFramework::Assert::ExpectException<Y>([&](){(X);})
