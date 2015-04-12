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


#include "str/Manip.h"
#include <iostream>
#include <sstream>
#include <algorithm>

void str::trim(std::string & s)
{
    unsigned int i;
    for (i = 0; i < s.length(); i++)
    {
        if (!isspace(s[i]))
            break;
    }
    s.erase(0, i);

    for (i = s.length() - 1; (int) i >= 0; i--)
    {
        if (!isspace(s[i]))
            break;

    }
    if (i + 1 < s.length())
        s.erase(i + 1);
}

bool str::endsWith(const std::string & s, const std::string & match)
{
    int mLen = match.length();
    int sLen = s.length();
    for (int i = 0; i < sLen && i < mLen; ++i)
        if (!(s[sLen - i - 1] == match[mLen - i - 1]))
            return false;
    return sLen >= mLen;
}

bool str::startsWith(const std::string & s, const std::string & match)
{
    int mLen = match.length();
    int sLen = s.length();
    for (int i = 0; i < sLen && i < mLen; ++i)
        if (!(s[i] == match[i]))
            return false;
    return sLen >= mLen;
}


std::vector<std::string> str::split(const std::string& s,
                                    const std::string& splitter)
{
    std::vector<std::string> vec;
    int str_l = (int)s.length();
    int split_l = (int)splitter.length();
    int pos = 0;
    int nextPos;
    while (pos < str_l)
    {
        nextPos = (int)s.find(splitter, pos);
        if (nextPos == std::string::npos)
            nextPos = str_l;
        if (nextPos != pos)
            vec.push_back(s.substr(pos, nextPos - pos));
        pos = nextPos + split_l;
    }
    return vec;
}


void str::lower(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), (int(*)(int)) tolower);
}

void str::upper(std::string& s)
{
    std::transform(s.begin(), s.end(), s.begin(), (int(*)(int)) toupper);
}

