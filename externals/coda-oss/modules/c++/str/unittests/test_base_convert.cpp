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
#include <std/string>
#include <iterator>

#include <import/str.h>
#include <str/EncodedString.h>
#include <str/Encoding.h>

#include "TestCase.h"

inline static void test_assert_eq(const std::string& testName, const std::u8string& actual, const std::u8string& expected)
{
    TEST_ASSERT_EQ(actual, expected);
}
inline static void test_assert_eq(const std::string& testName, const std::u8string& actual, const std::u32string& expected)
{
    test_assert_eq(testName, actual, str::to_u8string(expected));
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

static std::u8string fromWindows1252(const std::string& s)
{
    // s is Windows-1252 on ALL platforms
    return str::EncodedStringView::fromWindows1252(s).u8string();
}

template<typename T>
static constexpr std::u8string::value_type cast8(T ch)
{
    static_assert(sizeof(std::u8string::value_type) == sizeof(char), "sizeof(Char8_T) != sizeof(char)");
    return static_cast<std::u8string::value_type>(ch);
}
template <typename TChar>
static inline constexpr std::u32string::value_type U(TChar ch)
{
    return static_cast<std::u32string::value_type>(ch);
}

TEST_CASE(test_string_to_u8string_ascii)
{
    {
        const std::string input = "|\x00";  //  ASCII, "|<NULL>"
        const auto actual = fromWindows1252(input);
        const std::u8string expected{cast8('|')}; // '\x00' is the end of the string in C/C++
        TEST_ASSERT_EQ(actual, expected);
    }
    constexpr uint8_t start_of_heading = 0x01;
    constexpr uint8_t delete_character = 0x7f;
    for (uint8_t ch = start_of_heading; ch <= delete_character; ch++)  // ASCII
    {
        const std::string input { '|', static_cast<std::string::value_type>(ch), '|'};
        const auto actual = fromWindows1252(input);
        const std::u8string expected8{cast8('|'), cast8(ch), cast8('|')}; 
        test_assert_eq(testName, actual, expected8);
        const std::u32string expected{U'|', U(ch), U'|'};
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
        const std::u32string expected{U"|\U000020AC|"};  // UTF-32,  "|€|"
        test_assert_eq(testName, actual, expected);
    }
    {
        const std::string input = "|\x9F|";  // Windows-1252, "|Ÿ|"
        const auto actual = fromWindows1252(input);
        const std::u8string expected8{cast8('|'), cast8('\xC5'), cast8('\xB8'), cast8('|')};  // UTF-8,  "|Ÿ|"
        test_assert_eq(testName, actual, expected8);
        const std::u32string expected{U"|\U00000178|"};  // UTF-32,  "|Ÿ|"
        test_assert_eq(testName, actual, expected);
    }
    {
        const std::vector<char> undefined{ '\x81', '\x8d', '\x8f', '\x90', '\x9d' };
        for (const auto& ch : undefined)
        {
            const std::string input{'|', ch, '|'};
            const auto actual = fromWindows1252(input);
            static const std::u8string expected8{cast8('|'), cast8('\xEF'), cast8('\xBF'), cast8('\xBD'), cast8('|')};  // UTF-8,  "|<REPLACEMENT CHARACTER>|"
            test_assert_eq(testName, actual, expected8);
            const std::u32string expected{U"|\U0000fffd|"};  // UTF-32,  "|<REPLACEMENT CHARACTER>|"
            test_assert_eq(testName, actual, expected);
        }    
    }
    {
        //  http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
        const std::vector<uint8_t> windows1252_characters{
                //0x80,  // EURO SIGN
                //0x82,  // SINGLE LOW-9 QUOTATION MARK
                //0x83,  // LATIN SMALL LETTER F WITH HOOK
                //0x84,  // DOUBLE LOW-9 QUOTATION MARK
                //0x85,  // HORIZONTAL ELLIPSIS
                //0x86,  // DAGGER
                //0x87,  // DOUBLE DAGGER
                //0x88,  // MODIFIER LETTER CIRCUMFLEX ACCENT
                //0x89,  // PER MILLE SIGN
                //0x8A,  // LATIN CAPITAL LETTER S WITH CARON
                //0x8B,  // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
                //0x8C,  // LATIN CAPITAL LIGATURE OE
                0x8E,  // LATIN CAPITAL LETTER Z WITH CARON
                0x91,  // LEFT SINGLE QUOTATION MARK
                0x92,  // RIGHT SINGLE QUOTATION MARK
                0x93,  // LEFT DOUBLE QUOTATION MARK
                0x94,  // RIGHT DOUBLE QUOTATION MARK
                0x95,  // BULLET
                0x96,  // EN DASH
                0x97,  // EM DASH
                0x98,  // SMALL TILDE
                0x99,  // TRADE MARK SIGN
                0x9A,  // LATIN SMALL LETTER S WITH CARON
                0x9B,  // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
                0x9C,  // LATIN SMALL LIGATURE OE
                0x9E,  // LATIN SMALL LETTER Z WITH CARON
                0x9F};  // LATIN CAPITAL LETTER Y WITH DIAERESIS
        for (const auto& ch : windows1252_characters)
        {
            const std::string input_ { '|', static_cast<std::string::value_type>(ch), '|'};
            const str::W1252string input(str::c_str<str::W1252string>(input_));
            const auto actual = to_u8string(input);

            // No "expected" to test against as the UTF-8 values for these Windows-1252 characters
            // are mapped one-by-one.  However, we can test that UTF-8 to Windows-1252
            // works as that walks through a UTF-8 string which can have 1-, 2-, 3- and 4-bytes
            // for a single code-point.
            const auto w1252 = str::EncodedStringView::details::w1252string(str::EncodedStringView(actual));
            TEST_ASSERT(input == w1252);

            // Can't compare the values with == because TEST_ASSERT_EQ()
            // wants to do toString() and that doesn't work on Linux as the encoding
            // is wrong (see above).
            //const std::string w1252_ = str::c_str<std::string::const_pointer>(w1252);
            //TEST_ASSERT_EQ(input_, w1252_);
            const str::EncodedStringView inputView(input);
            const str::EncodedStringView w1252View(w1252);
            TEST_ASSERT_EQ(inputView, w1252View);
        }    
    }
}

TEST_CASE(test_string_to_u8string_iso8859_1)
{
    constexpr uint8_t nobreak_space = 0xa0;
    constexpr uint8_t latin_small_letter_y_with_diaeresis = 0xff;  // 'ÿ'
    for (uint32_t ch = nobreak_space; ch <= latin_small_letter_y_with_diaeresis; ch++)  // ISO8859-1
    {
        const std::string input_ { '|', static_cast<std::string::value_type>(ch), '|'};
        const str::W1252string input(str::c_str<str::W1252string>(input_));
        const auto actual = to_u8string(input);
        const std::u32string expected{U'|', U(ch), U'|'};
        test_assert_eq(testName, actual, expected);

        // Can't compare the values with == because TEST_ASSERT_EQ()
        // wants to do toString() and that doesn't work on Linux as the encoding
        // is wrong (see above).
        //std::string actual_;
        //str::details::toString(actual.c_str(), actual_);
        //TEST_ASSERT_EQ(input_, actual_);
        const str::EncodedStringView inputView(input);
        const str::EncodedStringView actualView(actual);
        TEST_ASSERT_EQ(inputView, actualView);
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

// https://en.wikipedia.org/wiki/%C3%89#Character_mappings
static const str::EncodedString& classificationText_utf_8()
{
    static const str::EncodedString retval(str::cast<std::u8string::const_pointer>("A\xc3\x89IOU")); // UTF-8 "AÉIOU"
    return retval;
 }
static const str::EncodedString& classificationText_iso8859_1()
{
    static const str::EncodedString retval(str::cast<str::W1252string::const_pointer>("A\xc9IOU"));  // ISO8859-1 "AÉIOU"    
    return retval;
 }
// UTF-16 on Windows, UTF-32 on Linux
static const wchar_t* classificationText_wide_() { return L"A\xc9IOU"; } // UTF-8 "AÉIOU"
static str::EncodedString classificationText_wide() { return str::EncodedString(classificationText_wide_()); }
static std::string classificationText_platform() { return 
    sys::Platform == sys::PlatformType::Linux ? classificationText_utf_8().native() : classificationText_iso8859_1().native(); }

TEST_CASE(test_u8string_to_string)
{
    {
        const auto utf8 = classificationText_utf_8().u8string();
        const str::EncodedStringView utf8View(utf8);
        const auto actual = utf8View.native();
        TEST_ASSERT_EQ(classificationText_platform(), actual);
    }
    {
        const auto utf8 = classificationText_iso8859_1().u8string();
        const str::EncodedStringView utf8View(utf8);
        const auto actual = utf8View.native();
        TEST_ASSERT_EQ(classificationText_platform(), actual);
    }
}

TEST_CASE(test_u8string_to_u16string)
{
    #if _WIN32
    const auto actual = classificationText_utf_8().u16string();
    const std::wstring s = str::c_str<std::wstring>(actual); // Windows: std::wstring == std::u16string
    TEST_ASSERT(classificationText_wide_() == s);  // _EQ wants to do toString()
    #endif

    TEST_ASSERT_EQ(classificationText_wide(), classificationText_utf_8());
    TEST_ASSERT_EQ(classificationText_wide(), classificationText_iso8859_1());

    TEST_ASSERT(classificationText_wide().u16string() == classificationText_utf_8().u16string()); // _EQ wants to do toString()
    TEST_ASSERT(classificationText_wide().u16string() == classificationText_iso8859_1().u16string()); // _EQ wants to do toString()
}

TEST_CASE(test_u8string_to_u32string)
{
    #if !_WIN32
    const auto actual = classificationText_utf_8().u32string();
    const std::wstring s  = str::c_str<std::wstring>(actual); // Linux: std::wstring == std::u32string
    TEST_ASSERT(classificationText_wide_() == s); // _EQ wants to do toString()
    #endif

    TEST_ASSERT_EQ(classificationText_wide(), classificationText_utf_8());
    TEST_ASSERT_EQ(classificationText_wide(), classificationText_iso8859_1());

    TEST_ASSERT(classificationText_wide().u32string() == classificationText_utf_8().u32string()); // _EQ wants to do toString()
    TEST_ASSERT(classificationText_wide().u32string() == classificationText_iso8859_1().u32string()); // _EQ wants to do toString()
}

static void test_EncodedStringView_(const std::string& testName,
    const str::EncodedStringView& utf_8_view, const str::EncodedStringView& iso8859_1_view)
{
    (void)testName;
    TEST_ASSERT_EQ(iso8859_1_view, iso8859_1_view);
    TEST_ASSERT_EQ(utf_8_view, utf_8_view);
    TEST_ASSERT_EQ(iso8859_1_view, utf_8_view);
    TEST_ASSERT_EQ(utf_8_view, iso8859_1_view);

    TEST_ASSERT_EQ(iso8859_1_view.native(), utf_8_view.native());
    const auto native = classificationText_platform();
    TEST_ASSERT_EQ(iso8859_1_view.native(), native);
    TEST_ASSERT_EQ(utf_8_view.native(), native);

    TEST_ASSERT(utf_8_view == classificationText_utf_8());
    TEST_ASSERT_EQ(utf_8_view, classificationText_utf_8());
    TEST_ASSERT(iso8859_1_view == classificationText_utf_8());
    TEST_ASSERT_EQ(iso8859_1_view, classificationText_utf_8());
    TEST_ASSERT(iso8859_1_view.u8string() == utf_8_view.u8string());

    const auto expected = str::EncodedString::details::string(classificationText_utf_8());
    {
        const auto actual = utf_8_view.asUtf8();
        TEST_ASSERT_EQ(actual, expected);
    }
    {
        const auto actual = iso8859_1_view.asUtf8();
        TEST_ASSERT_EQ(actual, expected);
    }
}
TEST_CASE(test_EncodedStringView)
{
    str::EncodedStringView esv;
    auto copy(esv);
    copy = esv; // assignment

    {
        auto utf_8_view(classificationText_utf_8().view());
        auto iso8859_1_view(classificationText_iso8859_1().view());
        test_EncodedStringView_(testName, utf_8_view, iso8859_1_view);
        
        utf_8_view = classificationText_iso8859_1().view();
        iso8859_1_view = classificationText_utf_8().view();
        test_EncodedStringView_(testName, utf_8_view, iso8859_1_view);
    }
    {
        auto utf_8_view = classificationText_utf_8().view();
        auto iso8859_1_view = classificationText_iso8859_1().view();
        test_EncodedStringView_(testName, utf_8_view, iso8859_1_view);

        utf_8_view = classificationText_iso8859_1().view();
        iso8859_1_view = classificationText_utf_8().view();
        test_EncodedStringView_(testName, utf_8_view, iso8859_1_view);
    }
    {
        str::EncodedStringView utf_8_view;
        utf_8_view = classificationText_iso8859_1().view();
        str::EncodedStringView iso8859_1_view;
        iso8859_1_view = classificationText_utf_8().view();
        test_EncodedStringView_(testName, utf_8_view, iso8859_1_view);
    }
}

TEST_CASE(test_EncodedString)
{
    str::EncodedString es;
    TEST_ASSERT_TRUE(es.empty());
    TEST_ASSERT_TRUE(es.native().empty());
    {
        str::EncodedString es_copy(es);  // copy
        TEST_ASSERT_TRUE(es_copy.empty());
        TEST_ASSERT_TRUE(es_copy.native().empty());
    }
    es = str::EncodedString("abc"); // assignment
    TEST_ASSERT_EQ(es.native(), "abc");
    {
        str::EncodedString es_copy(es);  // copy, again; this time w/o default content
        TEST_ASSERT_EQ(es_copy.native(), "abc");
    }

    str::EncodedString abc(es); // copy, for use below
    TEST_ASSERT_EQ(abc.native(), "abc");
    
    str::EncodedString es2;
    es = std::move(es2);  // move assignment
    TEST_ASSERT_TRUE(es.empty());
    TEST_ASSERT_TRUE(es.native().empty());
    str::EncodedString abc_(abc);  // copy
    es = std::move(abc_); // move assignment, w/o default content
    TEST_ASSERT_EQ(es.native(), "abc");

    str::EncodedString es3(std::move(abc)); // move constructor
    TEST_ASSERT_EQ(es3.native(), "abc");
}
TEST_MAIN(
    TEST_CHECK(testConvert);
    TEST_CHECK(testBadConvert);
    TEST_CHECK(testEightBitIntToString);
    TEST_CHECK(testCharToString);
    TEST_CHECK(test_string_to_u8string_ascii);
    TEST_CHECK(test_string_to_u8string_windows_1252);
    TEST_CHECK(test_string_to_u8string_iso8859_1);
    TEST_CHECK(test_change_case);
    TEST_CHECK(test_u8string_to_string);
    TEST_CHECK(test_u8string_to_u16string);
    TEST_CHECK(test_u8string_to_u32string);
    TEST_CHECK(test_EncodedStringView);
    TEST_CHECK(test_EncodedString);
    )