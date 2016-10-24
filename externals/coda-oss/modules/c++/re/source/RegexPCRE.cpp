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

// This header includes <re/re_config.h> so must come before the ifndef
#include <re/Regex.h>

#ifndef RE_ENABLE_STD_REGEX

#include <sstream>

#include <re/RegexException.h>

namespace
{
std::string getErrorMessage(int errorCode)
{
    PCRE2_UCHAR buffer[256];
    pcre2_get_error_message(errorCode, buffer, sizeof(buffer));

    return reinterpret_cast<char*>(buffer);
}

class ScopedMatchData
{
public:
    // NOTE: If the reallocation is too expensive, can call
    //       pcre2_match_data_create() and pass in the count.  In this case,
    //       can we reuse a match data object between calls?  We then need to
    //       handle more cases below where numMatches == 0 which indicates the
    //       output vector wasn't large enough like the old PCRE 5.0 code used
    //       to do.
    ScopedMatchData(const pcre2_code* code) :
        mCode(code),
        mMatchData(pcre2_match_data_create_from_pattern(code, NULL))
    {
        if (mMatchData == NULL)
        {
            throw re::RegexException(Ctxt(
                    "pcre2_match_data_create_from_pattern() failed to "
                    "allocate memory"));
        }
    }

    ~ScopedMatchData()
    {
        pcre2_match_data_free(mMatchData);
    }

    pcre2_match_data* get()
    {
        return mMatchData;
    }

    const PCRE2_SIZE* getOutputVector() const
    {
        return pcre2_get_ovector_pointer(mMatchData);
    }

    // Returns the number of matches
    size_t match(const std::string& subject,
                 PCRE2_SIZE startOffset = 0,
                 sys::Uint32_T options = 0)
    {
        // This returns the number of matches
        // But for no matches, it returns PCRE2_ERROR_NOMATCH
        // Other return codes less than 0 indicate an error
        const int returnCode =
                pcre2_match(mCode,
                            reinterpret_cast<PCRE2_SPTR>(subject.c_str()),
                            subject.length(),
                            startOffset,
                            options,
                            mMatchData,
                            NULL); // Match context

        if (returnCode == PCRE2_ERROR_NOMATCH)
        {
            return 0;
        }
        else if (returnCode < 0)
        {
            // Some error occurred
            throw re::RegexException(Ctxt("pcre2_match() failed"));
        }
        else
        {
            // The returnCode value won't include trailing empty
            // matches. By returning the actual size including empty matches
            // we now match the STL and Python versions of regex.
            return pcre2_get_ovector_count(mMatchData);
        }
    }

    std::string getMatch(const std::string& str, size_t idx) const
    {
        const PCRE2_SIZE* const outVector = getOutputVector();

        const size_t index = outVector[idx * 2];
        const size_t end = outVector[idx * 2 + 1];

        // If both index and end are set to PCRE2_UNSET then we did not find
        // a match the index and end values are invalid for the substr call.
        // In this case we need to return an empty string.
        if (index == PCRE2_UNSET && end == PCRE2_UNSET)
        {
            return "";
        }

        if (end > str.length())
        {
            // Presumably this never happens
            std::ostringstream ostr;
            ostr << "Match: Match substring out of range ("
                 << index << ", " << end << ") for string of length "
                 << str.length();
            throw re::RegexException(Ctxt(ostr.str()));
        }

        const size_t subStringLength = end - index;
        return str.substr(index, subStringLength);
    }

private:
    // Noncopyable
    ScopedMatchData(const ScopedMatchData& );
    ScopedMatchData& operator=(const ScopedMatchData& );

private:
    const pcre2_code* const mCode;
    pcre2_match_data* const mMatchData;
};
}

namespace re
{
Regex::Regex(const std::string& pattern) :
    mPattern(pattern), mPCRE(NULL)
{
    if (!mPattern.empty())
    {
        compile(mPattern);
    }
}

void Regex::destroy()
{
    if (mPCRE != NULL)
    {
        pcre2_code_free(mPCRE);
        mPCRE = NULL;
    }
}

Regex::~Regex()
{
    destroy();
}

Regex::Regex(const Regex& rhs) :
    mPattern(rhs.mPattern), mPCRE(NULL)
{
    compile(mPattern);
}

Regex& Regex::operator=(const Regex& rhs)
{
    if (this != &rhs)
    {
        destroy();

        mPattern = rhs.mPattern;

        compile(mPattern);
    }

    return *this;
}

Regex& Regex::compile(const std::string& pattern)
{
    mPattern = pattern;

    destroy();

    // TODO: So, I would like an RAII object for this.  But, this object would
    //       need to call pcre2_compile() in its constructor rather than take
    //       ownership of a pointer (so that it has access to the error code
    //       information on failure) and its copy constructor / assignment
    //       operator would need to hold onto mPattern.  At that point the
    //       class is close to being this Regex class itself.
    static const int FLAGS = PCRE2_DOTALL;
    int errorCode;
    PCRE2_SIZE errorOffset;
    mPCRE = pcre2_compile(reinterpret_cast<PCRE2_SPTR>(mPattern.c_str()),
                          mPattern.length(),
                          FLAGS,
                          &errorCode,
                          &errorOffset,
                          NULL); // Use default compile context

    if (mPCRE == NULL)
    {
        std::ostringstream ostr;
        ostr << "PCRE compilation failed at offset " << errorOffset
             << ": " << getErrorMessage(errorCode);
        throw RegexException(Ctxt(ostr.str()));
    }

    return *this;
}

bool Regex::matches(const std::string& str) const
{
    ScopedMatchData matchData(mPCRE);
    return (matchData.match(str) > 0);
}

bool Regex::match(const std::string& str, RegexMatch& matchObject)
{
    ScopedMatchData matchData(mPCRE);
    const size_t numMatches = matchData.match(str);
    matchObject.resize(numMatches);

    if (numMatches == 0)
    {
        return false;
    }

    for (size_t ii = 0; ii < numMatches; ++ii)
    {
        matchObject[ii] = matchData.getMatch(str, ii);
    }

    return true;
}

std::string Regex::search(const std::string& matchString, size_t startIndex)
{
    size_t begin;
    size_t end;
    return search(matchString, startIndex, 0, begin, end);
}

std::string Regex::search(const std::string& matchString,
                          size_t startIndex,
                          sys::Uint32_T flags,
                          size_t& begin,
                          size_t& end)
{
    ScopedMatchData matchData(mPCRE);
    const size_t numMatches = matchData.match(matchString, startIndex, flags);

    if (numMatches > 0)
    {
        begin = matchData.getOutputVector()[0];
        end = matchData.getOutputVector()[1];
        return matchData.getMatch(matchString, 0);
    }
    else
    {
        begin = end = 0;
        return "";
    }
}

void Regex::searchAll(const std::string& matchString, RegexMatch& v)
{
    size_t startIndex = 0;

    size_t begin;
    size_t end;
    std::string result = search(matchString, startIndex, 0, begin, end);

    while (!result.empty())
    {
        v.push_back(result);

        // We can't set startIndex = end because this won't work when the second
        // match starts inside the first match
        // For example, suppose mPattern = "...." and matchString = "abcde"
        // We need to find both "abcd" and "bcde"
        // The best we can do is start one character past the match we just
        // found
        startIndex = begin + 1;
        result = search(matchString, startIndex, PCRE2_NOTBOL, begin, end);
    }
}

void Regex::split(const std::string& str, std::vector<std::string>& v)
{
    size_t begin;
    size_t end;
    size_t startIndex = 0;
    std::string result = search(str, startIndex, 0, begin, end);
    while (!result.empty())
    {
        // We want to grab from [startIndex, begin)
        const size_t len = begin - startIndex;
        v.push_back(str.substr(startIndex, len));
        startIndex = end;
        result = search(str, startIndex, PCRE2_NOTBOL, begin, end);
    }

    // Push on last bit if there is some
    if (!str.substr(startIndex).empty())
    {
        v.push_back(str.substr(startIndex));
    }
}

std::string Regex::sub(const std::string& str, const std::string& repl)
{
    size_t begin;
    size_t end;
    std::string toReplace = str;
    size_t startIndex = 0;
    std::string result = search(str, startIndex, 0, begin, end);
    while (!result.empty())
    {
        toReplace.replace(begin, result.size(), repl);

        // You can't skip ahead result.size() here because 'repl' may be shorter
        // than 'result'
        startIndex = begin + repl.size();
        result = search(toReplace, startIndex, PCRE2_NOTBOL, begin, end);
    }

    return toReplace;
}
}

#endif
