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
#include <map>
#if _WIN32
#include <comdef.h>
#endif

#include <import/str.h>
#include <str/EncodedString.h>
#include <str/EncodedStringView.h>
#include <str/Encoding.h>

#include "TestCase.h"

inline static void test_assert_eq(const std::string& testName, const std::u8string& actual, const std::u32string& expected)
{
    TEST_ASSERT_EQ(actual, str::to_u8string(expected));
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
    TEST_ASSERT_EQ(str::toString(static_cast<uint8_t>(1)), "1");
    TEST_ASSERT_EQ(str::toString(static_cast<int8_t>(2)), "2");
    TEST_ASSERT_EQ(str::toString(static_cast<int8_t>(-2)), "-2");
}

TEST_CASE(testCharToString)
{
    TEST_ASSERT_EQ(str::toString('a'), "a");
    TEST_ASSERT_EQ(str::toString(static_cast<char>(65)), "A");
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
        TEST_ASSERT_EQ(actual, expected8);
        const std::u32string expected{U'|', U(ch), U'|'};
        test_assert_eq(testName, actual, expected);
    }
}

static void test_string_to_u8string_windows_1252_(const std::string& testName, const std::string& input_)
{
    const str::W1252string input(str::c_str<str::W1252string>(input_));
    const auto actual = to_u8string(input);

    // No "expected" to test against as the UTF-8 values for these Windows-1252 characters
    // are mapped one-by-one.  However, we can test that UTF-8 to Windows-1252
    // works as that walks through a UTF-8 string which can have 1-, 2-, 3- and 4-bytes
    // for a single code-point.
    const auto w1252 = str::to_w1252string(actual.data(), actual.size());
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
TEST_CASE(test_string_to_u8string_windows_1252)
{
    // Windows-1252 only characters must be mapped to UTF-8
    {
        const std::string input = "|\x80|";  // Windows-1252, "|€|"
        const auto actual = fromWindows1252(input);
        const std::u8string expected8{cast8('|'), cast8('\xE2'), cast8('\x82'), cast8('\xAC'), cast8('|')};  // UTF-8,  "|€|"
        TEST_ASSERT_EQ(actual, expected8);
        const std::u32string expected{U"|\u20AC|"};  // UTF-32,  "|€|"
        test_assert_eq(testName, actual, expected);
    }
    {
        const std::string input = "|\x9F|";  // Windows-1252, "|Ÿ|"
        const auto actual = fromWindows1252(input);
        const std::u8string expected8{cast8('|'), cast8('\xC5'), cast8('\xB8'), cast8('|')};  // UTF-8,  "|Ÿ|"
        TEST_ASSERT_EQ(actual, expected8);
        const std::u32string expected{U"|\u0178|"};  // UTF-32,  "|Ÿ|"
        test_assert_eq(testName, actual, expected);
    }
    {
        const std::vector<char> undefined{ '\x81', '\x8d', '\x8f', '\x90', '\x9d' };
        for (const auto& ch : undefined)
        {
            const std::string input{'|', ch, '|'};
            const auto actual = fromWindows1252(input);
            TEST_ASSERT_TRUE(!actual.empty());
            //const std::u8string expected8{cast8('|'), cast8('\xEF'), cast8('\xBF'), cast8('\xBD'), cast8('|')};  // UTF-8,  "|<REPLACEMENT CHARACTER>|"
            const std::u8string expected8{cast8('|'), cast8(194), cast8(ch), cast8('|')};
            TEST_ASSERT_EQ(actual, expected8);
            //const std::u32string expected{U"|\ufffd|"};  // UTF-32,  "|<REPLACEMENT CHARACTER>|"
            const auto expected = str::EncodedString(expected8).u32string();
            test_assert_eq(testName, actual, expected);
        }    
    }
    {
        //  http://www.unicode.org/Public/MAPPINGS/VENDORS/MICSFT/WINDOWS/CP1252.TXT
        const std::vector<uint8_t> windows1252_characters{
                0x80,  // EURO SIGN
                0x82,  // SINGLE LOW-9 QUOTATION MARK
                0x83,  // LATIN SMALL LETTER F WITH HOOK
                0x84,  // DOUBLE LOW-9 QUOTATION MARK
                0x85,  // HORIZONTAL ELLIPSIS
                0x86,  // DAGGER
                0x87,  // DOUBLE DAGGER
                0x88,  // MODIFIER LETTER CIRCUMFLEX ACCENT
                0x89,  // PER MILLE SIGN
                0x8A,  // LATIN CAPITAL LETTER S WITH CARON
                0x8B,  // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
                0x8C,  // LATIN CAPITAL LIGATURE OE
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
        std::string runningInput1;
        std::string runningInput2;
        for (const auto& ch_ : windows1252_characters)
        {
            const auto ch = static_cast<std::string::value_type>(ch_);
            const std::string input_{'[', ch, ']'};
            test_string_to_u8string_windows_1252_(testName, input_);

            runningInput1 += ch;
            test_string_to_u8string_windows_1252_(testName, runningInput1);

            runningInput2 += input_;
            test_string_to_u8string_windows_1252_(testName, runningInput2);
            test_string_to_u8string_windows_1252_(testName, runningInput1+runningInput2);
            test_string_to_u8string_windows_1252_(testName, runningInput2+runningInput1);
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

static void test_wide_(const std::string& testName, const char* pStr, std::u16string::const_pointer pUtf16, const str::EncodedString& encoded)
{
    // from UTF-16 back to Windows-1252
    const auto w1252 = str::EncodedStringView::details::w1252string(encoded.view());
    const std::string str_w1252 = str::c_str<std::string>(w1252);
    TEST_ASSERT_EQ(str_w1252, pStr);

    #if _WIN32
    // Since we're using UTF-16, on Windows that can be cast to wchar_t
    auto pWide = str::cast<std::wstring::const_pointer>(pUtf16);

    const _bstr_t str(pStr);
    const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
    TEST_ASSERT(encoded.wstring() == std_wstr);
    TEST_ASSERT(std_wstr == pWide);

    const _bstr_t wide_str(pWide);
    const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
    TEST_ASSERT_EQ(encoded.native(), std_str);
    TEST_ASSERT_EQ(std_str, pStr);
    #else
    pUtf16 = pUtf16; // avoid unused-parameter warning
    #endif
}

static void test_Windows1252_ascii(const std::string& testName, const char* pStr, std::u16string::const_pointer pUtf16)
{
    // For both UTF-8 and Windows-1252, ASCII is the same (they only differ for 0x80-0xff).
    const auto view8 = str::EncodedStringView::fromUtf8(pStr);
    TEST_ASSERT_EQ(pStr, view8.native()); // native() is the same on all platforms/encodings for ASCII
    const auto view1252 = str::EncodedStringView::fromWindows1252(pStr);
    TEST_ASSERT_EQ(pStr, view1252.native()); // native() is the same on all platforms/encodings for ASCII

    const str::EncodedString encoded(pStr);
    TEST_ASSERT(encoded.u16string() == pUtf16);
    test_wide_(testName, pStr, pUtf16, encoded);

    const str::EncodedString wide_encoded(pUtf16);
    TEST_ASSERT_EQ(wide_encoded.native(), pStr); // native() is the same on all platforms/encodings for ASCII
    TEST_ASSERT_EQ(view8, wide_encoded);
    TEST_ASSERT_EQ(view1252, wide_encoded);
    test_wide_(testName, pStr, pUtf16, wide_encoded);
}
TEST_CASE(test_ASCII)
{
    // https://en.cppreference.com/w/cpp/language/escape
    constexpr auto escapes = "|\'|\"|\?|\\|\a|\b|\f|\n|\r|\t|\v|";
    constexpr auto u16_escapes = u"|\'|\"|\?|\\|\a|\b|\f|\n|\r|\t|\v|";
    test_Windows1252_ascii(testName, escapes, u16_escapes);

    // https://en.cppreference.com/w/cpp/language/escape
    constexpr auto controls = "|\x01|\x02|\x03|\x04|\x05|\x06|\x07|\x08|\x09|\x0a|\x0b|\x0c|\x0d|\x0e|\x0f"
                              "|\x10|\x11|\x12|\x13|\x14|\x15|\x16|\x17|\x18|\x19|\x1a|\x1b|\x1c|\x1d|\x1e|\x1f";
    constexpr auto u16_controls = u"|\x01|\x02|\x03|\x04|\x05|\x06|\x07|\x08|\x09|\x0a|\x0b|\x0c|\x0d|\x0e|\x0f"
                                  u"|\x10|\x11|\x12|\x13|\x14|\x15|\x16|\x17|\x18|\x19|\x1a|\x1b|\x1c|\x1d|\x1e|\x1f";
    test_Windows1252_ascii(testName, controls, u16_controls);

    // https://en.cppreference.com/w/cpp/language/ascii
    constexpr auto ascii = " !\"#0@AZaz~\x7f";
    constexpr auto u16_ascii = u" !\"#0@AZaz~\x7f";
    test_Windows1252_ascii(testName, ascii, u16_ascii);
}

static void test_Windows1252_(const std::string& testName, const char* pStr, std::u16string::const_pointer pUtf16)
{
    const auto view = str::EncodedStringView::fromWindows1252(pStr);

    const str::EncodedString encoded(view);
    TEST_ASSERT(view.u16string() == pUtf16);
    test_wide_(testName, pStr, pUtf16, encoded);

    const str::EncodedString wide_encoded(pUtf16);
    TEST_ASSERT_EQ(view, wide_encoded);
    test_wide_(testName, pStr, pUtf16, wide_encoded);
}
TEST_CASE(test_Windows1252_WIN32)
{
    // https://en.wikipedia.org/wiki/Windows-1252
    #if _WIN32
    // can convert with bit-twiddling
    constexpr auto w1252_a1_ff = "¡¢þÿ"; // <INVERTED EXCLAMATION MARK><CENT SIGN><LATIN SMALL LETTER THORN><LATIN SMALL LETTER Y WITH DIAERESIS>
    //constexpr auto w1252_a1_ff = "\xa1\xa2\xfe\xff"; 
    constexpr auto u16_w1252_a1_ff = u"\u00a1\u00a2\u00fe\u00ff";
    test_Windows1252_(testName, w1252_a1_ff, u16_w1252_a1_ff);

    constexpr auto w1252 = "€‚ƒ„…†‡ˆ‰Š‹ŒŽ‘’“”•–—˜™š›œžŸ"; // these values must be mapped
    //constexpr auto w1252 = "\x80\x82\x83\x84\x85\x86\x87\x88\x89\x8a\x8b\x8c\x8e" // these values must be mapped
    //    "\x91\x92\x93\x94\x95\x96\x97\x98\x99\x9a\x9b\x9c\x9e\x9f";
    constexpr auto u16_utf8 = u"\u20ac\u201a\u0192\u201e\u2026\u2020\u2021\u02c6\u2030\u0160\u2039\u0152\u017d"
        "\u2018\u2019\u201c\u201d\u2022\u2013\u2014\u02dc\u2122\u0161\u203a\u0153\u017e\u0178";
    test_Windows1252_(testName, w1252, u16_utf8);
    
    // This only works with "relaxed" (i.e., not "strict") conversion; which is what _bstr_t does
    constexpr auto w1252_unassigned = "\x81\x8d\x8f\x90\x9d";
    constexpr auto u16_w1252_unassigned = u"\x81\x8d\x8f\x90\x9d";
    test_Windows1252_(testName, w1252_unassigned, u16_w1252_unassigned);
    #else
    TEST_ASSERT_TRUE(true); // need to use hidden "testName" parameter
    #endif
}

TEST_CASE(test_Windows1252)
{
    // https://en.wikipedia.org/wiki/Windows-1252
    const std::map<std::string::value_type, std::u16string::value_type>
            w1252_to_utf16{
          {'\x80', u'\u20AC'} // EURO SIGN
        , {'\x81', u'\u0081'} // UNDEFINED
        , {'\x82', u'\u201A'} // SINGLE LOW-9 QUOTATION MARK
        , {'\x83', u'\u0192'} // LATIN SMALL LETTER F WITH HOOK
        , {'\x84', u'\u201E'} // DOUBLE LOW-9 QUOTATION MARK
        , {'\x85', u'\u2026'} // HORIZONTAL ELLIPSIS
        , {'\x86', u'\u2020'} // DAGGER
        , {'\x87', u'\u2021'} // DOUBLE DAGGER
        , {'\x88', u'\u02C6'} // MODIFIER LETTER CIRCUMFLEX ACCENT
        , {'\x89', u'\u2030'} // PER MILLE SIGN
        , {'\x8A', u'\u0160'} // LATIN CAPITAL LETTER S WITH CARON
        , {'\x8B', u'\u2039'} // SINGLE LEFT-POINTING ANGLE QUOTATION MARK
        , {'\x8C', u'\u0152'} // LATIN CAPITAL LIGATURE OE
        , {'\x81', u'\u0081'} // UNDEFINED
        , {'\x8E', u'\u017D'} // LATIN CAPITAL LETTER Z WITH CARON
        , {'\x8F', u'\u008F'} // UNDEFINED
        , {'\x90', u'\u0090'} // UNDEFINED
        , {'\x91', u'\u2018'} // LEFT SINGLE QUOTATION MARK
        , {'\x92', u'\u2019'} // RIGHT SINGLE QUOTATION MARK
        , {'\x93', u'\u201C'} // LEFT DOUBLE QUOTATION MARK
        , {'\x94', u'\u201D'} // RIGHT DOUBLE QUOTATION MARK
        , {'\x95', u'\u2022'} // BULLET
        , {'\x96', u'\u2013'} // EN DASH
        , {'\x97', u'\u2014'} // EM DASH
        , {'\x98', u'\u02DC'} // SMALL TILDE
        , {'\x99', u'\u2122'} // TRADE MARK SIGN
        , {'\x9A', u'\u0161'} // LATIN SMALL LETTER S WITH CARON
        , {'\x9B', u'\u203A'} // SINGLE RIGHT-POINTING ANGLE QUOTATION MARK
        , {'\x9C', u'\u0153'} // LATIN SMALL LIGATURE OE
        , {'\x9D', u'\u009D'} // UNDEFINED
        , {'\x9E', u'\u017E'} // LATIN SMALL LETTER Z WITH CARON
        , {'\x9F', u'\u0178'} // LATIN CAPITAL LETTER Y WITH DIAERESIS

        , {'\xA0', u'\u00A0'} // NO-BREAK SPACE
        , {'\xA1', u'\u00A1'} // INVERTED EXCLAMATION MARK
        , {'\xA2', u'\u00A2'} // CENT SIGN
          // ...
        , {'\xFE', u'\u00FE'} // LATIN SMALL LETTER THORN
        , {'\xFF', u'\u00FF'} // LATIN SMALL LETTER Y WITH DIAERESIS
    };
    std::string running_w1252;
    std::u16string running_utf16;
    for (auto&& ch : w1252_to_utf16)
    {
        TEST_ASSERT_LESSER_EQ(static_cast<int>(ch.first), 0xff);
        TEST_ASSERT_LESSER_EQ(static_cast<int>(ch.second), 0xffff);

        const std::string w1252{ch.first};
        const std::u16string utf16{ch.second};
        test_Windows1252_(testName, w1252.c_str(), utf16.c_str());

        running_w1252 += w1252;
        running_utf16 += utf16;
        test_Windows1252_(testName, running_w1252.c_str(), running_utf16.c_str());
    }
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
    TEST_CHECK(test_ASCII);
    TEST_CHECK(test_Windows1252_WIN32);
    TEST_CHECK(test_Windows1252);
    TEST_CHECK(test_EncodedStringView);
    TEST_CHECK(test_EncodedString);
    )
