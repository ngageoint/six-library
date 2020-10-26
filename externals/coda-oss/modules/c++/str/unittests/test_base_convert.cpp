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

#include <import/str.h>
#include "TestCase.h"

TEST_CASE(testConvert)
{
    TEST_ASSERT_EQ(str::toType<long long>("0x3BC7", 16), (long long) 0x3BC7);
    TEST_ASSERT_EQ(str::toType<long long>("1101", 2), (long long) 13);
    TEST_ASSERT_EQ(str::toType<long long>("231", 5), (long long) 66);
    TEST_ASSERT_EQ(str::toType<unsigned long long>("0xFFFFFFFFFFFFFFFF", 16),
                   (unsigned long long) 0xFFFFFFFFFFFFFFFF);
    TEST_ASSERT_EQ(str::toType<unsigned long long>("-10", 10),
                   (unsigned long long) -10);
    TEST_ASSERT_EQ(str::toType<short>("13", 4), (short) 7);
}

TEST_CASE(testBadConvert)
{
    TEST_EXCEPTION(str::toType<long long>("Not a number", 10));
    TEST_EXCEPTION(str::toType<long long>("0xFFFFFFFFFFFFFFFF", 16));
    TEST_EXCEPTION(str::toType<short>("0xFFFFF", 16));
}

TEST_CASE(testEightBitIntToString)
{
    TEST_ASSERT_EQ(str::toString<uint8_t>(1), "1");
    TEST_ASSERT_EQ(str::toString<int8_t>(2), "2");
    TEST_ASSERT_EQ(str::toString<int8_t>(-2), "-2");
}

TEST_CASE(testCharToString)
{
    TEST_ASSERT_EQ(str::toString<char>('a'), "a");
    TEST_ASSERT_EQ(str::toString<char>(65), "A");
}

static inline sys::u8string::value_type cast(std::string::value_type ch)
{
    static_assert(sizeof(sys::u8string::value_type) == sizeof(std::string::value_type),
        "sizeof(Char8_T) != sizeof(char)");
    return static_cast<sys::u8string::value_type>(ch);
}
TEST_CASE(test_string_to_u8string)
{
    const std::string input = "|I\xc9|";  // ISO8859-1, "|I�|"
    const auto actual = str::toUtf8(input);
    const sys::u8string expected { cast('|'), cast('I'), cast('\xc3'), cast('\x89'), cast('|') };  // UTF-8,  "|I�|"
    TEST_ASSERT(actual == expected);
}

TEST_CASE(test_string_to_u8string_undefined)
{
    const std::string input = "|\x81|";  // Windows-1252, "|<UNDEFINED>|"
    const auto actual = str::toUtf8(input);
    const sys::u8string expected{cast('|'), cast(' '), cast('|')};  // UTF-8,  "| |"
    TEST_ASSERT(actual == expected);
}

TEST_CASE(test_string_to_u8string_replacement)
{
    const std::string input = "|\x80|";  // Windows-1252, "|�|"
    const auto actual = str::toUtf8(input);
    const sys::u8string expected{cast('|'), cast('\xEF'), cast('\xBF'), cast('\xBD'), cast('|')};  // UTF-8,  "|<REPLACEMENT CHARACTER>|"
    TEST_ASSERT(actual == expected);
}

int main(int, char**)
{
    TEST_CHECK(testConvert);
    TEST_CHECK(testBadConvert);
    TEST_CHECK(testEightBitIntToString);
    TEST_CHECK(testCharToString);
    TEST_CHECK(test_string_to_u8string);
    TEST_CHECK(test_string_to_u8string_undefined);
    TEST_CHECK(test_string_to_u8string_replacement);
}
