/* =========================================================================
 * This file is part of str-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * str-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include <tuple> // std::ignore

// TODO: remove this once TIntergers are switched to types::details::complex<TInteger>
// '...': warning STL4037: The effect of instantiating the template std::complex for any type other than float, double, or long double is unspecified. You can define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING to suppress this warning.
#ifndef _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING
#define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING
#endif
#include <complex>

#include <config/compiler_extensions.h>
#include <import/str.h>
#include <types/complex.h>

#include "TestCase.h"

inline std::string to_string(const std::string& value)
{
    return value;
}

TEST_CASE(testTrim)
{
    std::string s = "  test   ";
    str::trim( s);
    TEST_ASSERT_EQ(s, "test");
}

TEST_CASE(testData)
{
    std::string s;
    // https://en.cppreference.com/w/cpp/string/basic_string/resize
    s.resize(3); // "Resizes the string to contain count characters." 
    
    CODA_OSS_disable_warning_push
    #if _MSC_VER
    #pragma warning(disable : 4996)  // '...': This function or variable may be unsafe. ...
    #endif

    // https://en.cppreference.com/w/cpp/string/basic_string/data
    // "Modifying the past-the-end null terminator stored at data()+size() to any value other than CharT() has undefined behavior."
    std::ignore = strcpy(str::data(s), "abc"); 
    
    CODA_OSS_disable_warning_pop
    TEST_ASSERT_EQ(s, "abc");
}

TEST_CASE(testUpper)
{
    std::string s = "test-something1";
    str::upper( s);
    TEST_ASSERT_EQ(s, "TEST-SOMETHING1");
}

TEST_CASE(testLower)
{
    std::string s = "TEST1";
    str::lower( s);
    TEST_ASSERT_EQ(s, "test1");
}

TEST_CASE(test_eq_ne)
{
    const auto s1 = "TEST1";
    const auto s2 = "test1";
    const auto s3 = "T2";

    TEST_ASSERT_TRUE(str::eq(s1, s1));
    TEST_ASSERT_FALSE(str::ne(s1, s1));

    TEST_ASSERT_TRUE(str::eq(s1, s2));
    TEST_ASSERT_FALSE(str::ne(s1, s2));
    TEST_ASSERT_TRUE(str::eq(s2, s1));
    TEST_ASSERT_FALSE(str::ne(s2, s1));

    TEST_ASSERT_FALSE(str::eq(s1, s3));
    TEST_ASSERT_TRUE(str::ne(s1, s3));
    TEST_ASSERT_FALSE(str::eq(s3, s1));
    TEST_ASSERT_TRUE(str::ne(s3, s1));
}

TEST_CASE(testReplace)
{
    std::string s = "helo world";
    str::replace(s, "l", "ll");
    TEST_ASSERT_EQ(s, "hello world");
}

TEST_CASE(testReplaceAllInfinite)
{
    std::string s = "helo hello";
    str::replaceAll(s, "l", "ll");
    TEST_ASSERT_EQ(s, "hello hellllo");
}

TEST_CASE(testReplaceAllRecurse)
{
    std::string s = "Mississippi";
    str::replaceAll(s, "i", " ");
    TEST_ASSERT_EQ(s, "M ss ss pp ");
}

TEST_CASE(testContains)
{
    std::string s = "Mississippi";
    TEST_ASSERT_TRUE(str::contains(s, "ssiss"));
}

TEST_CASE(testNotContains)
{
    std::string s = "Mississippi";
    TEST_ASSERT_FALSE(str::contains(s, "miss"));
}

TEST_CASE(testSplit)
{
    std::string s = "space delimited values are the best!";
    std::vector<std::string> parts = str::split(s, " ");
    TEST_ASSERT_EQ(parts.size(), static_cast<size_t>(6));
    parts = str::split(s, " ", 3);
    TEST_ASSERT_EQ(parts.size(), static_cast<size_t>(3));
    TEST_ASSERT_EQ(parts[2], "values are the best!");
}

TEST_CASE(testIsAlpha)
{
    TEST_ASSERT(str::isAlpha("abcdefghijklmnopqrstuvwxyz"));
    TEST_ASSERT_FALSE(str::isAlpha("abc123"));
    TEST_ASSERT_FALSE(str::isAlpha("abcs with spaces"));
}
TEST_CASE(testIsAlphaSpace)
{
    TEST_ASSERT(str::isAlphaSpace("abcdefghijklmnopqrstuvwxyz"));
    TEST_ASSERT_FALSE(str::isAlphaSpace("abc123"));
    TEST_ASSERT(str::isAlphaSpace("abcs with spaces"));
}
TEST_CASE(testIsNumeric)
{
    TEST_ASSERT_FALSE(str::isNumeric("abcdefghijklmnopqrstuvwxyz"));
    TEST_ASSERT_FALSE(str::isNumeric("abc123"));
    TEST_ASSERT_FALSE(str::isNumeric("abcs with spaces"));
    TEST_ASSERT(str::isNumeric("42"));
}
TEST_CASE(testIsNumericSpace)
{
    TEST_ASSERT_FALSE(str::isNumericSpace("lotto47"));
    TEST_ASSERT(str::isNumericSpace("42"));
    TEST_ASSERT(str::isNumericSpace("42 15 23 5 12"));
}
TEST_CASE(testIsAlphanumeric)
{
    TEST_ASSERT(str::isAlphanumeric("lotto47"));
    TEST_ASSERT(str::isAlphanumeric("42"));
    TEST_ASSERT_FALSE(str::isAlphanumeric("42 15 23 5 12"));
    TEST_ASSERT(str::isAlphanumeric("justtext"));
}
TEST_CASE(testIsWhitespace)
{
    TEST_ASSERT_FALSE(str::isWhitespace("lotto47"));
    TEST_ASSERT(str::isWhitespace(""));
    TEST_ASSERT(str::isWhitespace(" "));
    TEST_ASSERT(str::isWhitespace("                          "));
    TEST_ASSERT(str::isWhitespace("\t"));
    TEST_ASSERT(str::isWhitespace("\t \n"));
}
TEST_CASE(testContainsOnly)
{
    TEST_ASSERT(str::containsOnly("abc", "abcdefghijklmnopqrstuvwxyz"));
    TEST_ASSERT_FALSE(str::containsOnly("abc!", "abcdefghijklmnopqrstuvwxyz"));
    TEST_ASSERT(str::containsOnly("some-cool-id", "-abcdefghijklmnopqrstuvwxyz"));
    TEST_ASSERT(str::containsOnly("\n\r\t ", " \t\n\r0123456789"));
    TEST_ASSERT(str::containsOnly("1-2-3", " \t\n\r0123456789-"));
}
TEST_CASE(testRoundDouble)
{
    double eps = std::numeric_limits<double>::epsilon();
    double numerator = 10005.0;
    double denom = 10007.0;
    double v = numerator / denom;
    std::string s = str::toString(v);
    std::cout << s << std::endl;

    double nv = str::toType<double>(s);
    TEST_ASSERT_ALMOST_EQ_EPS(nv, v, eps);
    nv *= denom;
    TEST_ASSERT_ALMOST_EQ_EPS(nv, numerator, eps);
    std::cout << nv << std::endl;
    std::cout << (nv - (int)nv) << std::endl;
    std::cout << std::numeric_limits<double>::epsilon() << std::endl;
    TEST_ASSERT_EQ((int)std::ceil(nv), (int)numerator);
}

TEST_CASE(testEscapeForXMLNoReplace)
{
    const std::string origMessage("This is a perfectly fine string");
    std::string message(origMessage);
    str::escapeForXML(message);
    TEST_ASSERT_EQ(message, origMessage);
}

TEST_CASE(testEscapeForXMLKitchenSink)
{
    std::string message(
            "This & that with <angles> and \"quotes\" & single 'quotes' & "
            "why not a\nnewline & \rcarriage return at the end?");

    const std::string expectedMessage(
            "This &amp; that with &lt;angles&gt; and &quot;quotes&quot; &amp; "
            "single &apos;quotes&apos; &amp; why not a&#10;newline &amp; "
            "&#13;carriage return at the end?");

    str::escapeForXML(message);
    TEST_ASSERT_EQ(message, expectedMessage);
}

TEST_CASE(test_toStringComplexFloat)
{
    const std::string expected("(1,-2)");

    const std::complex<float> std_cx_float(1.0f, -2.0f);
    auto actual = str::toString(std_cx_float);
    TEST_ASSERT_EQ(actual, expected);

    const types::complex<float> types_cx_float(1.0f, -2.0f);
    actual = str::toString(types_cx_float);
    TEST_ASSERT_EQ(actual, expected);

    const types::zfloat zfloat(1.0f, -2.0f);
    actual = str::toString(zfloat);
    TEST_ASSERT_EQ(actual, expected);
}

TEST_CASE(test_toStringComplexShort)
{
    const std::string expected("(1,-2)");

    #if _MSC_VER
    #pragma warning(disable: 4996) // '...': warning STL4037: The effect of instantiating the template std::complex for any type other than float, double, or long double is unspecified. You can define _SILENCE_NONFLOATING_COMPLEX_DEPRECATION_WARNING to suppress this warning
    #endif
    const std::complex<short> std_cx_short(1, -2);
    auto actual = str::toString(std_cx_short);
    TEST_ASSERT_EQ(actual, expected);

    const types::complex<short> types_cx_short(1, -2);
    actual = str::toString(types_cx_short);
    TEST_ASSERT_EQ(actual, expected);

    const types::zint16_t zint16(1, -2);
    actual = str::toString(zint16);
    TEST_ASSERT_EQ(actual, expected);
}


TEST_MAIN(
    TEST_CHECK(testTrim);
    TEST_CHECK(testData);
    TEST_CHECK(testUpper);
    TEST_CHECK(testLower);
    TEST_CHECK(test_eq_ne);
    TEST_CHECK(testReplace);
    TEST_CHECK(testReplaceAllInfinite);
    TEST_CHECK(testReplaceAllRecurse);
    TEST_CHECK(testContains);
    TEST_CHECK(testNotContains);
    TEST_CHECK(testSplit);
    TEST_CHECK(testIsAlpha);
    TEST_CHECK(testIsAlphaSpace);
    TEST_CHECK(testIsNumeric);
    TEST_CHECK(testIsNumericSpace);
    TEST_CHECK(testIsAlphanumeric);
    TEST_CHECK(testIsWhitespace);
    TEST_CHECK(testContainsOnly);
    TEST_CHECK(testRoundDouble);
    TEST_CHECK(testEscapeForXMLNoReplace);
    TEST_CHECK(testEscapeForXMLKitchenSink);
    TEST_CHECK(test_toStringComplexFloat);
    TEST_CHECK(test_toStringComplexShort);
    )