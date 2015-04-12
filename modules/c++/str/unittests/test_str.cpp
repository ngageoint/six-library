/* =========================================================================
 * This file is part of str-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include <import/str.h>
#include "TestCase.h"

TEST_CASE(testTrim)
{
    std::string s = "  test   ";
    str::trim( s);
    TEST_ASSERT_EQ(s, "test");
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
    TEST_ASSERT_EQ(parts.size(), 6);
    parts = str::split(s, " ", 3);
    TEST_ASSERT_EQ(parts.size(), 3);
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

int main(int argc, char* argv[])
{
    TEST_CHECK( testTrim);
    TEST_CHECK( testUpper);
    TEST_CHECK( testLower);
    TEST_CHECK( testReplace);
    TEST_CHECK( testReplaceAllInfinite);
    TEST_CHECK( testReplaceAllRecurse);
    TEST_CHECK( testContains);
    TEST_CHECK( testNotContains);
    TEST_CHECK( testSplit);
    TEST_CHECK( testIsAlpha);
    TEST_CHECK( testIsAlphaSpace);
    TEST_CHECK( testIsNumeric);
    TEST_CHECK( testIsNumericSpace);
    TEST_CHECK( testIsAlphanumeric);
    TEST_CHECK( testIsWhitespace);
    TEST_CHECK( testContainsOnly);
    TEST_CHECK( testRoundDouble);
}
