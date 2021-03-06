/* =========================================================================
 * This file is part of re-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

// This header includes <re/re_config.h> so must come before the ifdef
#include <re/Regex.h>

#ifdef RE_ENABLE_STD_REGEX

#include <sys/Conf.h>
#include <re/RegexException.h>

namespace re
{
// This is a raw literal, so ignore the R"lit( )lit"
const std::regex Regex::badDotRegex( R"lit(((^|[^\\])(\\\\)*)\.)lit",
                                         std::regex::ECMAScript|std::regex::optimize );

const std::regex Regex::invalidCaret( R"lit([\s\S]*([^\[\\]|[^\\](\\\\)+)\^)lit",
                                          std::regex::ECMAScript|std::regex::optimize );

const std::regex Regex::invalidDollar( R"lit(^([\s\S]*[^\\](\\\\)*)?\$[\s\S]+$)lit",
                                           std::regex::ECMAScript|std::regex::optimize );
    
Regex::Regex(const std::string& pattern) :
    mPattern(pattern)
{
    if (!mPattern.empty())
    {
        compile(mPattern);
    }
}

void Regex::destroy()
{
}

Regex::~Regex()
{
}

Regex::Regex(const Regex& rhs)
{
    mPattern = rhs.mPattern;
    compile(mPattern);
}

Regex& Regex::operator=(const Regex& rhs)
{
    if (this != &rhs)
    {
        mPattern = rhs.mPattern;

        compile(mPattern);
    }

    return *this;
}

Regex& Regex::compile(const std::string& pattern)
{
    // We'll set these first, so that if we throw an exception we'll
    // leave the regex in a compiled state, so if the user REALLY
    // wants to, they can put it in a try/catch block and keep going.

    mPattern = replaceDot(pattern);
    mRegex = std::regex(mPattern, std::regex::ECMAScript|std::regex::optimize);


    // Because VS2015 and gcc handle ^ and $ differently, we'll throw
    // exceptions if they're in the middle of the pattern somewhere

    std::smatch tmpmatch;

    // Look for ^ in the middle, but ignore \^ and [^
    if (std::regex_search(mPattern, tmpmatch, 
                          invalidCaret,
                          std::regex_constants::match_continuous))
    {
        std::string msg(
            "'^' in mid-string is not handled the same by gcc and VS2015!");
        msg += " So we don't allow it :(";
        throw RegexException(Ctxt(msg));
    }

    // Look for $ in the middle, but ignore \$
    if (std::regex_match(mPattern, tmpmatch,
                         invalidDollar))
    {
        std::string msg(
            "'$' in mid-string is not handled the same by gcc and VS2015!");
        msg += " So we don't allow it :(";
        throw RegexException(Ctxt(msg));
    }

    // Also throw on trailing $ with no leading ^, since that won't be
    // uniformly handled either
    if (!mPattern.empty() && mPattern.front() != '^' && mPattern.back() == '$')
    {
        std::string msg("Trailing '$' will not be handled correctly!");
        msg += " Try adding a '^' at the beginning and matching the entire string.";
        throw RegexException(Ctxt(msg));
    }

    return *this;
}

bool Regex::matches(const std::string& str) const
{
    std::smatch matches;
    return searchWithContext(str.cbegin(), str.cend(), matches);
}

bool Regex::match(const std::string& str, RegexMatch& matchObject)
{
    std::smatch matches;
    bool result = searchWithContext(str.cbegin(), str.cend(), matches);

    // copy resulting substrings into matchObject
    matchObject.resize(matches.size());

    // This causes a crash for some reason
    //std::copy(matches.begin(), matches.end(), matchObject.begin());

    for (size_t ii = 0; ii < matches.size(); ++ii)
    {
        matchObject[ii] = matches[ii].str();
    }

    return result;
}

std::string Regex::search(const std::string& matchString, size_t startIndex)
{
    std::smatch matches;

    // search the string starting at index "startIndex"
    bool result = searchWithContext(matchString.begin() + startIndex,
                                    matchString.end(), matches);
    
    // if successful, return the substring matching the regex,
    // otherwise return empty string
    if (result && !matches.empty())
    {
        return matches[0].str();
    }
    else
    {
        return "";
    }
}

void Regex::searchAll(const std::string& matchString, RegexMatch& v)
{
    std::smatch match;
    size_t startIndex = 0;
    bool matchBeginning = true;

    // search the string starting at index "startIndex"
    while (searchWithContext(matchString.begin()+startIndex, 
                             matchString.end(), match, matchBeginning))
    {
        v.push_back(match[0].str());
        startIndex += (match.position(0) + 1); // advance one char beyond this match
        matchBeginning = false; // don't match BOL after first match
    }
}

void Regex::split(const std::string& str, std::vector<std::string> & v)
{
    size_t idx = 0;
    bool matchBeginning = true;
    std::smatch match;

    while (searchWithContext(str.begin()+idx, str.end(), match, matchBeginning))
    {
        v.push_back( str.substr(idx, match.position()) );
        idx += (match.position() + match.length());
        matchBeginning = false; // don't match BOL after first match
    }

    // Push on last bit if there is some
    if (!str.substr(idx).empty())
    {
        v.push_back(str.substr(idx));
    }
}

std::string Regex::sub(const std::string& str, const std::string& repl)
{
    std::string toReplace = str;

    size_t idx = 0;
    bool matchBeginning = true;
    std::smatch match;

    while (searchWithContext(toReplace.cbegin()+idx, toReplace.cend(), match,
                             matchBeginning))
    {
        toReplace.replace(idx + match.position(), match.length(), repl);
        idx += (match.position() + repl.length());
        matchBeginning = false; // don't match BOL after first match
    }

    return toReplace;
}

std::string Regex::replaceDot(const std::string& str) const
{
    // Match beginning-of-string or a non-\ character,
    // followed by 0 or more "\\",
    // followed by "."
    // This makes sure we're not grabbing "\."

    // Replace just the "." with "[\s\S]"
    std::string newstr = std::regex_replace(str, badDotRegex, "$1[\\s\\S]");
    return newstr;
}

bool Regex::searchWithContext(std::string::const_iterator inputIterBegin,
                              std::string::const_iterator inputIterEnd,
                              std::smatch& match,
                              bool matchBeginning) const
{
    bool b(false);
    auto flags = std::regex_constants::match_default;
    if (!matchBeginning)
    {
        flags |= std::regex_constants::match_not_bol;
    }

    // Now we look for our 4 cases:
    // 1) "^...$" -> use std::regex_match() to force match at beginning and end
    // 2) "^..."  -> use std::regex_constants::continuous_match to force match at start
    // 3) "...$"  -> throw exception
    // 4) "..."   -> use plain std::regex_search()
    if (!mPattern.empty() && mPattern.front() == '^')
    {
        if (mPattern.length() >= 2 && mPattern.back() == '$')
        {
            b = std::regex_match(inputIterBegin, inputIterEnd, 
                                 match, mRegex, flags);
        }
        else
        {
            flags |= std::regex_constants::match_continuous;
            b = std::regex_search(inputIterBegin, inputIterEnd,
                                  match, mRegex, flags);
        }
    }
    else
    {
        b = std::regex_search(inputIterBegin, inputIterEnd,
                              match, mRegex, flags);
    }

    return b;
}
}

#endif
