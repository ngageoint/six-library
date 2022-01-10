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

#include <wchar.h>

#include <vector>
#include <string>
#include <iterator>

#include <std/string>

#include <import/str.h>
#include <str/utf8.h>

#include "TestCase.h"

template<typename T>
std::string to_std_string(const T& value)
{
    // This is OK as UTF-8 can be stored in std::string
    // Note that casting between the string types will CRASH on some
    // implementations. NO: reinterpret_cast<const std::string&>(value)
    return str::c_str<std::string::const_pointer>(value);  // copy
}
template<>
std::string to_std_string(const std::u32string& value)
{
    str::U8string result;
    str::utf32to8(value, result);
    return to_std_string(result);
}
template<typename TActual, typename TExpected>
void test_assert_eq(const std::string& testName,
                           const TActual& actual, const TExpected& expected)
{
    //TEST_ASSERT(actual == expected);
    const auto actual_ = to_std_string(actual);
    const auto expected_ = to_std_string(expected);
    TEST_ASSERT_EQ(actual_, expected_);
}

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

static sys::U8string fromWindows1252(const std::string& s)
{
    // s is Windows-1252 on ALL platforms
    return str::fromWindows1252(s);
}

template<typename T>
static constexpr std::u8string::value_type cast8(T ch)
{
    static_assert(sizeof(std::u8string::value_type) == sizeof(char), "sizeof(Char8_T) != sizeof(char)");
    return static_cast<std::u8string::value_type>(ch);
}
template <typename T>
static constexpr std::u32string::value_type cast32(T ch)
{
    return static_cast<std::u32string::value_type>(ch);
}
TEST_CASE(test_string_to_u8string_ascii)
{
    {
        const std::string input = "|\x00";  //  ASCII, "|<NULL>"
        const auto actual = fromWindows1252(input);
        const std::u8string expected{cast8('|')}; // '\x00' is the end of the string in C/C++
        test_assert_eq(testName, actual, expected);
    }
    constexpr uint8_t start_of_heading = 0x01;
    constexpr uint8_t delete_character = 0x7f;
    for (uint8_t ch = start_of_heading; ch <= delete_character; ch++)  // ASCII
    {
        const std::string input { '|', static_cast<std::string::value_type>(ch), '|'};
        const auto actual = fromWindows1252(input);
        const std::u8string expected8{cast8('|'), cast8(ch), cast8('|')}; 
        test_assert_eq(testName, actual, expected8);
        const std::u32string expected{cast32('|'), cast32(ch), cast32('|')};
        test_assert_eq(testName, actual, expected);
    }
}

TEST_CASE(test_string_to_u8string_windows_1252)
{
    // Windows-1252 only characters must be mapped to UTF-8
    {
        const std::string input = "|\x80|";  // Windows-1252, "|€|"
        const auto actual = fromWindows1252(input);
        const std::u8string expected8{cast8('|'), cast8('\xE2'), cast8('\x82'), cast8('\xAC'), cast8('|')};  // UTF-8,  "|€|"
        test_assert_eq(testName, actual, expected8);
        const std::u32string expected{cast32('|'), 0x20AC, cast32('|')};  // UTF-32,  "|€|"
        test_assert_eq(testName, actual, expected);
    }
    {
        const std::string input = "|\x9F|";  // Windows-1252, "|Ÿ|"
        const auto actual = fromWindows1252(input);
        const std::u8string expected8{cast8('|'), cast8('\xC5'), cast8('\xB8'), cast8('|')};  // UTF-8,  "|Ÿ|"
        test_assert_eq(testName, actual, expected8);
        const std::u32string expected{cast32('|'), 0x0178, cast32('|')};  // UTF-32,  "|Ÿ|"
        test_assert_eq(testName, actual, expected);

    }
    const std::vector<char> undefined{ '\x81', '\x8d', '\x8f', '\x90', '\x9d' };
    for (const auto& ch : undefined)
    {
        const std::string input{'|', ch, '|'};
        const auto actual = fromWindows1252(input);
        static const std::u8string expected8{cast8('|'), cast8('\xEF'), cast8('\xBF'), cast8('\xBD'), cast8('|')};  // UTF-8,  "|<REPLACEMENT CHARACTER>|"
        test_assert_eq(testName, actual, expected8);
        const std::u32string expected{cast32('|'), 0xfffd, cast32('|')};  // UTF-32,  "|<REPLACEMENT CHARACTER>|"
        test_assert_eq(testName, actual, expected);
    }
}


TEST_CASE(test_string_to_u8string_iso8859_1)
{
    constexpr uint8_t nobreak_space = 0xa0;
    constexpr uint8_t latin_small_letter_y_with_diaeresis = 0xff;  // 'ÿ'
    for (uint32_t ch = nobreak_space; ch <= latin_small_letter_y_with_diaeresis; ch++)  // ISO8859-1
    {
        const std::string input { '|', static_cast<std::string::value_type>(ch), '|'};
        const auto actual = fromWindows1252(input);
        const std::u32string expected{cast32('|'), cast32(ch), cast32('|')};
        test_assert_eq(testName, actual, expected);
    }
}

template<typename TString>
static void test_change_case_(const std::string& testName, const TString& lower, const TString& upper)
{
    auto s = upper;
    str::lower(s);
    TEST_ASSERT(s == lower);
    s = lower;
    str::upper(s);
    TEST_ASSERT(s == upper);

    s = upper;
    str::upper(s);
    TEST_ASSERT(s == upper);
    s = lower;
    str::lower(s);
    TEST_ASSERT(s == lower);
}
TEST_CASE(test_change_case)
{
    const std::string ABC = "ABC";
    const std::string abc = "abc";
    test_change_case_(testName, abc, ABC);

    //const std::wstring ABC_w = L"ABC";
    //const std::wstring abc_w = L"abc";
    //test_change_case_(testName, abc_w, ABC_w);

    //// Yes, this can really come up, "non classifié" is French (Canadian) for "unclassified".
    //const std::string DEF_1252{'D', '\xc9', 'F'}; // "DÉF" Windows-1252
    //const auto DEF8 = fromWindows1252(DEF_1252);

    //const std::string def_1252{'d', '\xe9', 'f'};  // "déf" Windows-1252
    //const auto def8 = fromWindows1252(def_1252);

    ////test_change_case_(testName, def, DEF);
    //test_change_case_(testName, def_1252, DEF_1252);
}
TEST_CASE(test_u8string_to_string)
{
    const std::string classificationText_utf_8("NON CLASSIFI\xc3\x89 / UNCLASSIFIED");  // UTF-8 "NON CLASSIFIÉ / UNCLASSIFIED"
    const std::string classificationText_iso8859_1("NON CLASSIFI\xc9 / UNCLASSIFIED");  // ISO8859-1 "NON CLASSIFIÉ / UNCLASSIFIED"
    const auto expected = sys::Platform == sys::PlatformType::Linux ? classificationText_utf_8 : classificationText_iso8859_1;

    const auto utf8 = str::fromUtf8(classificationText_utf_8);
    const auto actual = str::toString(utf8);
    TEST_ASSERT_EQ(expected, actual);
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
    TEST_CHECK(test_change_case);
    TEST_CHECK(test_u8string_to_string);
}
