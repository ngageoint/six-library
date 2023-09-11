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

#include <config/compiler_extensions.h>
#include <import/str.h>
#include <str/Encoding.h>
#include <gsl/gsl.h>
#include <sys/OS.h>

#include "TestCase.h"

inline static void test_assert_eq(const std::string& testName, const std::u8string& actual, const std::u32string& expected)
{
    TEST_ASSERT_EQ(actual, str::to_u8string(expected));
}

TEST_CASE(testConvert)
{
    TEST_ASSERT_EQ(str::toType<long long>("0x3BC7", 16), gsl::narrow<long long>(0x3BC7));
    TEST_ASSERT_EQ(str::toType<long long>("1101", 2), gsl::narrow<long long>(13));
    TEST_ASSERT_EQ(str::toType<long long>("231", 5), gsl::narrow<long long>(66));
    TEST_ASSERT_EQ(str::toType<unsigned long long>("0xFFFFFFFFFFFFFFFF", 16),
                   gsl::narrow<unsigned long long>(0xFFFFFFFFFFFFFFFF));
    TEST_ASSERT_EQ(str::toType<unsigned long long>("-10", 10),
                   gsl::narrow_cast<unsigned long long>(-10));
    TEST_ASSERT_EQ(str::toType<short>("13", 4), gsl::narrow<short>(7));
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

template<typename T>
static constexpr std::u8string::value_type cast8(T ch)
{
    using u8ch_t = std::u8string::value_type;
    static_assert(sizeof(u8ch_t) == sizeof(char), "sizeof(Char8_T) != sizeof(char)");
    return static_cast<u8ch_t>(ch);
}

TEST_CASE(test_string_to_u8string_ascii)
{
    {
        const std::string input = "|\x00";  //  ASCII, "|<NULL>"
        const auto actual = str::to_u8string<str::W1252string>(input);
        const std::u8string expected{cast8('|')}; // '\x00' is the end of the string in C/C++
        TEST_ASSERT_EQ(actual, expected);
    }
    constexpr uint8_t start_of_heading = 0x01;
    constexpr uint8_t delete_character = 0x7f;
    for (uint8_t ch = start_of_heading; ch <= delete_character; ch++)  // ASCII
    {
        const std::string input { '|', static_cast<std::string::value_type>(ch), '|'};
        const auto actual = str::to_u8string<str::W1252string>(input);
        const std::u8string expected8{cast8('|'), cast8(ch), cast8('|')}; 
        TEST_ASSERT_EQ(actual, expected8);
        const std::u32string expected{U'|', ch, U'|'};
        test_assert_eq(testName, actual, expected);
    }
}

static void test_string_to_u8string_windows_1252_(const std::string& testName, const std::string& input_)
{
    const auto input(str::str<str::W1252string>(input_));
    const auto actual = str::to_u8string(input);

    // No "expected" to test against as the UTF-8 values for these Windows-1252 characters
    // are mapped one-by-one.  However, we can test that UTF-8 to Windows-1252
    // works as that walks through a UTF-8 string which can have 1-, 2-, 3- and 4-bytes
    // for a single code-point.
    const auto w1252 = str::to_w1252string(actual.data(), actual.size());
    TEST_ASSERT(input == w1252);
}
TEST_CASE(test_string_to_u8string_windows_1252)
{
    // Windows-1252 only characters must be mapped to UTF-8
    {
        const std::string input = "|\x80|";  // Windows-1252, "|€|"
        const auto actual = str::to_u8string<str::W1252string>(input);
        const std::u8string expected8{cast8('|'), cast8('\xE2'), cast8('\x82'), cast8('\xAC'), cast8('|')};  // UTF-8,  "|€|"
        TEST_ASSERT_EQ(actual, expected8);
        const std::u32string expected{U"|\u20AC|"};  // UTF-32,  "|€|"
        test_assert_eq(testName, actual, expected);
    }
    {
        const std::string input = "|\x9F|";  // Windows-1252, "|Ÿ|"
        const auto actual = str::to_u8string<str::W1252string>(input);
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
            const auto actual = str::to_u8string<str::W1252string>(input);
            TEST_ASSERT_TRUE(!actual.empty());
            //const std::u8string expected8{cast8('|'), cast8('\xEF'), cast8('\xBF'), cast8('\xBD'), cast8('|')};  // UTF-8,  "|<REPLACEMENT CHARACTER>|"
            const std::u8string expected8{cast8('|'), cast8(194), cast8(ch), cast8('|')};
            TEST_ASSERT_EQ(actual, expected8);
            //const std::u32string expected{U"|\ufffd|"};  // UTF-32,  "|<REPLACEMENT CHARACTER>|"
            const auto expected = str::to_u32string(expected8);
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
        const auto input(str::str<str::W1252string>(input_));
        const auto actual = to_u8string(input);
        const std::u32string expected{U'|', ch, U'|'};
        test_assert_eq(testName, actual, expected);

        TEST_ASSERT(str::to_u8string(input) == actual);
        TEST_ASSERT(input == str::to_w1252string(actual));
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
    //const auto DEF8 = from_windows1252(DEF_1252);

    //const std::string def_1252{'d', '\xe9', 'f'};  // "déf" Windows-1252
    //const auto def8 = from_windows1252(def_1252);

    ////test_change_case_(testName, def, DEF);
    //test_change_case_(testName, def_1252, DEF_1252);
}

// https://en.wikipedia.org/wiki/%C3%89#Character_mappings
static const coda_oss::u8string& classificationText_u8()
{
    static const auto retval(str::make_string<std::u8string>("A\xc3\x89IOU")); // UTF-8 "AÉIOU"
    return retval;
 }

static const str::W1252string& classificationText_w1252()
 {
    static const auto retval(str::make_string<str::W1252string>("A\xc9IOU"));  // ISO8859-1 "AÉIOU"    
    return retval;
 }

 static auto toString(const coda_oss::u8string& s)
 {
     return str::to_native(s);
 }

// UTF-16 on Windows, UTF-32 on Linux
static const wchar_t* classificationText_wide_() { return L"A\x00c9IOU"; } // "wide characters" "AÉIOU"
static std::u16string classificationText_u16() { return u"A\u00c9IOU"; } // UTF-16 "AÉIOU"
static std::u32string classificationText_u32() { return U"A\u00c9IOU"; } // UTF-32 "AÉIOU"

static std::string classificationText_platform() { return 
    sys::Platform == sys::PlatformType::Linux ? toString(classificationText_u8()) : str::testing::to_string(classificationText_w1252()); }

TEST_CASE(test_u8string_to_string)
{
    auto actual = toString(classificationText_u8());
    TEST_ASSERT_EQ(classificationText_platform(), actual);

    actual = str::testing::to_string(classificationText_w1252()); 
    TEST_ASSERT_EQ(classificationText_platform(), actual);
}

static auto w1252FromNative(const std::wstring& s)
{
    return str::to_w1252string(str::u8FromNative(s));
}
static auto toWString(const str::W1252string& s)
{
    return str::testing::to_wstring(s);
}
static auto toWString(const coda_oss::u8string& s)
{
    return str::details::to_wstring(s);
}

TEST_CASE(test_u8string_to_u16string)
{
    const auto actual = classificationText_u16();
    const std::wstring wide(classificationText_wide_());
    #if _WIN32
    const auto s = str::str<std::wstring>(actual); // Windows: std::wstring == std::u16string
    TEST_ASSERT(wide == s);  // _EQ wants to do toString()
    #endif

    const auto u8 = classificationText_u8();
    TEST_ASSERT(str::u8FromNative(wide) == u8);
    TEST_ASSERT(wide == toWString(u8));
    
    const auto w1252 = str::c_str<str::W1252string>(classificationText_w1252());
    TEST_ASSERT(w1252FromNative(wide) == w1252);
    TEST_ASSERT(wide == toWString(w1252));

    TEST_ASSERT(classificationText_u16() == actual);  // _EQ wants to do toString()
    TEST_ASSERT(classificationText_u16() == to_u16string(w1252)); // _EQ wants to do toString()
}

TEST_CASE(test_u8string_to_u32string)
{
    const auto actual = classificationText_u32();
#if !_WIN32
    const auto s  = str::str<std::wstring>(actual); // Linux: std::wstring == std::u32string
    TEST_ASSERT(classificationText_wide_() == s); // _EQ wants to do toString()
    #endif
    
    const std::wstring wide(classificationText_wide_());
    const auto u8 = classificationText_u8();
    TEST_ASSERT(str::u8FromNative(wide) == u8);
    TEST_ASSERT(wide == toWString(u8));

    const auto w1252 = str::c_str<str::W1252string>(classificationText_w1252());
    TEST_ASSERT(w1252FromNative(wide) == w1252);
    TEST_ASSERT(wide == toWString(w1252));

    TEST_ASSERT(classificationText_u32() == actual);  // _EQ wants to do toString()
    TEST_ASSERT(classificationText_u32() == to_u32string(w1252)); // _EQ wants to do toString()
}

static auto toWString(const std::u16string& s)
{
    return toWString(str::to_u8string(s));
}
static auto toWString(const std::string& s)
{
    return str::details::to_wstring(s);
}
static auto toString(const std::u16string& s)
{
    return str::details::to_string(str::to_u8string(s));
}

static void test_wide_(const std::string& testName, const char* pStr, std::u16string::const_pointer pUtf16,
    const std::wstring& wstring, const std::string& native, const str::W1252string& w1252)
{
    // from UTF-16 back to Windows-1252
    const auto str_w1252 = str::str<std::string>(w1252);
    TEST_ASSERT_EQ(str_w1252, pStr);

    #if _WIN32
    // Since we're using UTF-16, on Windows that can be cast to wchar_t
    const auto wide = str::make_string<std::wstring>(pUtf16);

    const _bstr_t str(pStr);
    const std::wstring std_wstr(static_cast<const wchar_t*>(str)); // Windows-1252 -> UTF-16
    TEST_ASSERT(wstring == std_wstr);
    TEST_ASSERT(std_wstr == wide);

    const _bstr_t wide_str(wide.c_str());
    const std::string std_str(static_cast<const char*>(wide_str)); //  UTF-16 -> Windows-1252
    TEST_ASSERT_EQ(native, std_str);
    TEST_ASSERT_EQ(std_str, pStr);
    #else
    CODA_OSS_mark_symbol_unused(pUtf16);
    CODA_OSS_mark_symbol_unused(wstring);
    CODA_OSS_mark_symbol_unused(native);
    #endif
}
static void test_Windows1252_ascii(const std::string& testName, const char* pStr, std::u16string::const_pointer pUtf16)
{
    // For both UTF-8 and Windows-1252, ASCII is the same (they only differ for 0x80-0xff).
    const auto u8 =  str::str<std::string>(str::to_u8string<coda_oss::u8string>(pStr));
    TEST_ASSERT_EQ(pStr, u8); // native() is the same on all platforms/encodings for ASCII
    {
        const auto w1252 = str::make_string<str::W1252string>(pStr);
        const auto str1252 = str::testing::to_string(w1252);
        TEST_ASSERT_EQ(pStr, str1252);  // native() is the same on all platforms/encodings for ASCII
    }

    const auto u16 = str::to_u16string(str::u8FromNative(pStr));
    TEST_ASSERT(u16 == pUtf16);
    auto wstring = toWString(pStr);
    std::string native = pStr;
    auto w1252 = str::make_string<str::W1252string>(pStr);
    test_wide_(testName, pStr, pUtf16, wstring, native, w1252);

    native = toString(pUtf16);
    TEST_ASSERT_EQ(native, pStr); // native() is the same on all platforms/encodings for ASCII
    wstring = toWString(pUtf16);
    test_wide_(testName, pStr, pUtf16, wstring, native, w1252);
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

static auto to_w1252string(const std::u16string& s)
{
    return str::to_w1252string(str::to_u8string(s));
}
inline static auto toString(const std::wstring& s)
{
    return str::details::to_string(s);
}

static void test_Windows1252_(const std::string& testName, const char* pStr, std::u16string::const_pointer pUtf16)
{
    const auto u16 = str::to_u16string(str::to_u8string<str::W1252string>(pStr));
    TEST_ASSERT(u16 == pUtf16);
    auto wstring = toWString(str::to_u8string<str::W1252string>(pStr));
    auto s = toString(str::to_u8string<str::W1252string>(pStr));
    auto w1252 = str::make_string<str::W1252string>(pStr);
    test_wide_(testName, pStr, pUtf16, wstring, s, w1252);

    wstring = toWString(pUtf16);
    s = toString(pUtf16);
    w1252 = to_w1252string(pUtf16);
    test_wide_(testName, pStr, pUtf16, wstring, s, w1252);
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

    #if _WIN32
    // This only works on Windows because the "assume encoding" APIs are used.
    for (auto&& ch : w1252_to_utf16)
    {
        const std::string expected(1, ch.first);
        const std::wstring input(1, ch.second); // `std::wstring` is UTF-16 on Windows
        const auto actual = toString(input);
        TEST_ASSERT_EQ(expected, actual);
    }
    #endif
}

static void test_Encodeding_(const std::string& testName, const coda_oss::u8string& classificationText_u8,
    const std::string& utf_8, const std::string& iso8859_1,
    const coda_oss::u8string& utf_8_u8, const coda_oss::u8string& iso8859_1_u8,
    const std::string& utf_8_view, const std::string& iso8859_1_view)
{
    TEST_ASSERT_EQ(iso8859_1, utf_8);
    static const auto s = classificationText_platform();
    TEST_ASSERT_EQ(iso8859_1, s);
    TEST_ASSERT_EQ(utf_8, s);

    TEST_ASSERT(iso8859_1_u8 == utf_8_u8);

    const auto expected = str::c_str<std::string>(classificationText_u8);
    TEST_ASSERT_EQ(utf_8_view, expected);
    TEST_ASSERT_EQ(iso8859_1_view, expected);
}
TEST_CASE(test_Encoding)
{
    const auto utf_8 = toString(classificationText_u8());
    const auto iso8859_1 = str::testing::to_string(classificationText_w1252());
    const auto utf_8_u8 = classificationText_u8();
    const auto iso8859_1_u8 = str::to_u8string(classificationText_w1252());
    const auto utf_8_view = str::str<std::string>(classificationText_u8());
    const auto iso8859_1_view =  str::str<std::string>(str::to_u8string(classificationText_w1252()));

    test_Encodeding_(testName, classificationText_u8(),
        utf_8, iso8859_1,
        utf_8_u8, iso8859_1_u8,
        utf_8_view, iso8859_1_view);
    test_Encodeding_(testName, classificationText_u8(),
        iso8859_1, utf_8,
        iso8859_1_u8, utf_8_u8,
        iso8859_1_view, utf_8_view);
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
    TEST_CHECK(test_Encoding);
    )
