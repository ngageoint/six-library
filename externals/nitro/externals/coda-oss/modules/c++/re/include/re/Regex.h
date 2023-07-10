/* =========================================================================
 * This file is part of re-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * re-c++ is free software; you can redistribute it and/or modify
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

#ifndef __RE_REGEX_H__
#define __RE_REGEX_H__

#include <string>
#include <vector>

#include "config/Exports.h"

#if !defined(RE_ENABLE_STD_REGEX)
#include <re/re_config.h>
#endif

#ifdef RE_ENABLE_STD_REGEX
#include <regex>
#else
// This must be defined prior to pcre2.h
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>

#include <sys/Conf.h>
#endif

/*!
 *  \file Regex.h
 *  \brief C++ wrapper for the PCRE2 library or std::regex
 */

namespace re
{
typedef std::vector<std::string> RegexMatch;
/*!
 *  \class Regex
 *  \brief C++ wrapper object for regular expressions.  If enabled,
 *  std::regex from C++11 is used.  Unfortunately, the performance is
 *  significantly slower than PCRE so PCRE is the default.  For further
 *  documentation regarding the underlying PCRE library, especially for flag
 *  information, see http://www.pcre.org.
 */
class CODA_OSS_API Regex
{
public:
    /*!
     *  The default constructor
     */
    Regex(const std::string& pattern = "");

    //!  Destructor
    ~Regex();

    /*!
     *  Copy constructor
     *  \param rhs The Regex to copy from
     */
    Regex(const Regex& rhs);

    /*!
     *  Assignment operator.  Check for self assignment
     *  \param rhs The Regex to copy from
     *  \return This
     */
    Regex& operator=(const Regex& rhs);

    /*!
     *  Destroy all data used for matching.
     */
    void destroy();

    /*!
     *  Set the match pattern
     *  \param pattern  A pattern to match
     *  \throw  exception on error
     */
    Regex& compile(const std::string& pattern);

    /*!
     *  \todo Add non-const reference
     *  A const reference return for the pattern
     *  \return the pattern
     */
    const std::string& getPattern() const
    {
        return mPattern;
    }

    /*!
     *  Match this input string against our pattern and populate the
     *  data structure
     *  \param str  The string to match against our pattern
     *  \param matchObject  RegexMatch container to fill
     *      If false is returned, there was not a match, so this will be empty
     *      If true is returned, there was a match.  In this case, index 0
     *      will be the full match and subsequent indices will be the submatches
     *      (in the case where groups were used)
     *  \return  True on success, False otherwise
     *  \throw  RegexException on fatal error
     */
    bool match(const std::string& str,
               RegexMatch& matchObject);

    bool matches(const std::string& str) const;

    /*!
     *  Search the matchString
     *  \param matchString  The string to try and match
     *  \param startIndex  Starting where?
     *  \return  Matched substring
     *  \throw  RegexException on fatal error
     */
    std::string search(const std::string& matchString,
                       size_t startIndex = 0);

    /*!
     *  Search the matchString and get the sub-expressions, by ref
     *  If there are overlapping matches, all will be provided.
     *  For example, a pattern of "...." with a matchString of "12345"
     *  will return both "1234" and "2345".
     *  \param matchString  The string to match
     *  \param v  The vector to fill with sub-expressions
     *      Will be sized to the number of matches
     *      Each index will contain a match (unlike the matches() method which
     *      uses indices 1+ to provide submatches)
     */
    void searchAll(const std::string& matchString,
                   RegexMatch& v);

    /*!
     *  Split the string by occurrences of the pattern
     *  \param str  The string to split
     *  \param v    The resulting container of matches split from str
     */
    void split(const std::string& str,
               std::vector<std::string>& v);

    /*!
     *  Replace occurrences of the pattern in the string
     *  \param str  The string in which to replace the pattern
     *  \param repl  The replacement
     *  \return  The resulting string
     */
    std::string sub(const std::string& str,
                    const std::string& repl);

    /*!
     *  Backslash all non-alphanumeric characters
     *  \param str  The string to escape
     *  \return  The escaped string
     */
    static
    std::string escape(const std::string& str);

private:
    std::string mPattern;

#ifdef RE_ENABLE_STD_REGEX
    /*!
     *  Replace non-escaped "." with "[\s\S]" to get PCRE2_DOTALL newline
     *  behavior
     *  \param str  The string to modify
     *  \return  The modified string
     */
    std::string replaceDot(const std::string& str) const;

    /*!
     *  Search using std::regex appropriately based on input string:
     *   regexps starting with ^ are forced to match at beginning
     *   regexps ending with $ (and no ^ at beginning) cause exception
     *   regexps bracketed with ^ and $ match beginning and end
     *   regexps with neither ^ or $ search normally
     *   regexps with either ^ or $ in locations besides beginning and end
     *       throw excpetions
     *  \param inputIterBegin  The beginning of the string to search
     *  \param inputIterEnd  The end of the string to search
     *  \param match  The match object for search results
     *  \param matchBeginning  If false, do not match ^ to beginning of string
     *  \return  True on success, otherwise False
     *  \throw  RegexException on error
     */
    bool searchWithContext(std::string::const_iterator inputIterBegin,
                           std::string::const_iterator inputIterEnd,
                           std::smatch& match,
                           bool matchBeginning=true) const;

    //! The regex object
    std::regex mRegex;

#else
    // Internal function for passing flags to pcre2_match()
    std::string search(const std::string& matchString,
                       size_t startIndex,
                       sys::Uint32_T flag,
                       size_t& begin,
                       size_t& end);

    //! The pcre object
    pcre2_code* mPCRE;
#endif
};
}

#endif
