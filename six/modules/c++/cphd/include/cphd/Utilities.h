/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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

#ifndef __CPHD_UTILITIES_H__
#define __CPHD_UTILITIES_H__

#include <iostream>
#include <string>
#include <vector>
#include <map>

namespace cphd
{

struct FormatType
{
    FormatType()
    {
    }

    FormatType(std::string key, std::string val) :
        mKey(key),
        mVal(val)
    {
    }

    std::string getKey() const
    {
        return mKey;
    }

    std::string getVal() const
    {
        return mVal;
    }

    void setKey(const std::string key)
    {
        mKey = key;
    }

    void setVal(const std::string val)
    {
        mVal = val;
    }

protected:
    std::string mKey;
    std::string mVal;
};

/*
 * Validate user input format
 * If format is String, stringSize will contain string size
 * Returns if valid, throws if not
 */
void validateFormat(std::string format);

/*
 * Parse sizes of multiple params
 * Returns vector of sizes
 * Throws if invalid
 */
std::vector<std::pair<std::string,size_t> > getMultipleParamSizes(std::string format);

/*
 * Parses format string with size values for
 * multiple param formats
 * Returns list of FormatType objects
 * Throws if invalid
 */
std::map<std::string, size_t> getMultipleParams(std::string format);

/*
 * Returns expected size of format
 */
size_t getFormatSize(std::string format);

/*
 * Checks if format string is a concatenation of
 * multiple valid formats
 * Returns true if valid, false if not
 */
bool isMultipleParam(std::string format);

/*
 * Checks if format string is of format "S[1-9][0-9]"
 * If valid, returns size specifed by format,
 * if not, returns six::Undefined<size_t>
 */
size_t isFormatStr(std::string format);

/*
 * Returns a FormatType object with key and value between startPos and endPos
 * Throws exception if format is invalid
 */
FormatType keyValueFinder(std::string format, size_t startPos, size_t endPos);

}
#endif
