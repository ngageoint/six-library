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

#include <vector>

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

static constexpr sys::u8string::value_type cast(char ch)
{
    static_assert(sizeof(sys::u8string::value_type) == sizeof(char),
        "sizeof(Char8_T) != sizeof(char)");
    return static_cast<sys::u8string::value_type>(ch);
}
TEST_CASE(test_string_to_u8string_ascii)
{
    {
        const std::string input = "|\x00";  //  ASCII, "|<NULL>"
        const auto actual = str::toUtf8(input);
        const sys::u8string expected{cast('|')}; // '\x00' is the end of the string in C/C++
        TEST_ASSERT(actual == expected);
    }
    constexpr uint8_t start_of_heading = 0x01;
    constexpr uint8_t delete_character = 0x7f;
    for (uint8_t ch = start_of_heading; ch <= delete_character; ch++)  // ASCII
    {
        const std::string input { '|', static_cast<std::string::value_type>(ch), '|'};
        const auto actual = str::toUtf8(input);
        const sys::u8string expected{cast('|'), cast(ch), cast('|')};
        TEST_ASSERT(actual == expected);
    } 
}

TEST_CASE(test_string_to_u8string_windows_1252)
{
    // Windows-1252 only characters must be mapped to UTF-8
    {
        const std::string input = "|\x80|";  // Windows-1252, "|€|"
        const auto actual = str::toUtf8(input);
        const sys::u8string expected{cast('|'), cast('\xE2'), cast('\x82'), cast('\xAC'), cast('|')};  // UTF-8,  "|€|"
        TEST_ASSERT(actual == expected);
    }
    {
        const std::string input = "|\x9F|";  // Windows-1252, "|Ÿ|"
        const auto actual = str::toUtf8(input);
        const sys::u8string expected{cast('|'), cast('\xC5'), cast('\xB8'), cast('|')};  // UTF-8,  "|Ÿ|"
        TEST_ASSERT(actual == expected);
    }
    const std::vector<char> undefined{ '\x81', '\x8d', '\x8f', '\x90', '\x9d' };
    for (const auto& ch : undefined)
    {
        const std::string input{'|', ch, '|'};
        const auto actual = str::toUtf8(input);
        static const sys::u8string expected{cast('|'), cast('\xEF'), cast('\xBF'), cast('\xBD'), cast('|')};  // UTF-8,  "|<REPLACEMENT CHARACTER>|"
        TEST_ASSERT(actual == expected);
    }
}

TEST_CASE(test_string_to_u8string_iso8859_1)
{
    constexpr uint8_t nobreak_space = 0xa0;
    constexpr uint8_t latin_small_letter_y_with_diaeresis = 0xff;  // 'ÿ'
    for (uint16_t ch = nobreak_space; ch <= latin_small_letter_y_with_diaeresis; ch++)  // ISO8859-1
    {
        const std::string input { '|', static_cast<std::string::value_type>(ch), '|'};
        const auto actual = str::toUtf8(input);
        const sys::u8string expected { cast('|'), cast('\xc3'), cast(ch-0x40), cast('|') };  // UTF-8
       TEST_ASSERT(actual == expected);
    }
}

int main(int, char**)
{
    TEST_CHECK(testConvert);
    TEST_CHECK(testBadConvert);
    TEST_CHECK(testEightBitIntToString);
    TEST_CHECK(testCharToString);
    TEST_CHECK(test_string_to_u8string_ascii);
    TEST_CHECK(test_string_to_u8string_windows_1252);
    TEST_CHECK(test_string_to_u8string_iso8859_1);
}
