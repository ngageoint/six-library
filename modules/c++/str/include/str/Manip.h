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

#ifndef __STR_MANIP_H__
#define __STR_MANIP_H__

#include <string>
#include <ctype.h>
#include <vector>
#include "str/Convert.h"

namespace str
{
/**
 *  Trim the white space off the back and front of a string
 *  @param  s  String to trim
 */
void trim(std::string& s);

/**
 *  Checks the end of s with match
 *  @param  s  String to check
 *  @param  match  String to compare with
 *  @return true if it matches, otherwise false
 */
bool endsWith(const std::string& s, const std::string& match);

/**
 *  Checks the start of s with match
 *  @param  s  String to check
 *  @param  s  String to compare with
 *  @return true if it matches, otherwise false
 */
bool startsWith(const std::string& s, const std::string& match);

/**
 *  Splits a string based on a splitter string. Similar to tokenization, except
 *  the splitter string can be of any length.
 *  @param  s         String to check
 *  @param  splitter  String to split upon
 *  @return vector of strings
 */
std::vector<std::string> split(const std::string& s,
        const std::string& splitter);

//! Uses std::transform to convert all chars to lower case
void lower(std::string& s);

//! Uses std::transform to convert all chars to upper case
void upper(std::string& s);

template<typename T>
std::string join(std::vector<T> toks, std::string with)
{
    int len = (int)toks.size();
    std::ostringstream oss;
    int i = 0;
    for (; i < len - 1; i++)
    {
        oss << str::toString<T>(toks[i]) << with;
    }
    oss << str::toString(toks[i]);
    return oss.str();
}


}

#endif
