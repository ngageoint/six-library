#include "pch.h"
#include "TestCase.h"

#include "str/EncodedStringView.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

// EQUALS_MESSAGE() wants ToString() specializations (or overloads) for our types, which is a nusiance.
// This hooks up our existing str::toString() into the VC++ unit-test infrastructure

// C++ hack to call private methods
// https://stackoverflow.com/a/71578383/8877

using FailOnCondition_t = void(bool condition, const unsigned short* message, const __LineInfo* pLineInfo); // declare method's type
using GetAssertMessage_t = std::wstring(bool equality, const std::wstring& expected, const std::wstring& actual, const wchar_t *message); // declare method's type
template <FailOnCondition_t fFailOnCondition, GetAssertMessage_t fGetAssertMessage>
struct caller final // helper structure to inject call() code
{
    friend void FailOnCondition(bool condition, const unsigned short* message, const __LineInfo* pLineInfo)
    {
        fFailOnCondition(condition, message, pLineInfo);
    }

    friend std::wstring GetAssertMessage(bool equality, const std::wstring& expected, const std::wstring& actual, const wchar_t *message)
    {
        return fGetAssertMessage(equality, expected, actual, message);
    }
};
template struct caller<&Assert::FailOnCondition, &Assert::GetAssertMessage>; // even instantiation of the helper

void FailOnCondition(bool condition, const unsigned short* message, const __LineInfo* pLineInfo);  // declare caller
void test::Assert::FailOnCondition(bool condition, const unsigned short* message, const __LineInfo* pLineInfo)
{
    ::FailOnCondition(condition, message, pLineInfo);  // and call!
}

std::wstring GetAssertMessage(bool equality, const std::wstring& expected, const std::wstring& actual, const wchar_t *message); // declare caller
std::wstring test::Assert::GetAssertMessage(bool equality, const std::string& expected, const std::string& actual, const wchar_t *message)
{
    const str::EncodedStringView vExpected(expected);
    const str::EncodedStringView vActual(actual);
    return ::GetAssertMessage(equality, vExpected.wstring(), vActual.wstring(), message); // and call!
}
