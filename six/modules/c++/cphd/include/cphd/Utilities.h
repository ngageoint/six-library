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

#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{


// Return bytes/sample, either 2, 4, or 8 (or 0 if not initialized)
size_t getNumBytesPerSample(const SignalArrayFormat& signalArrayFormat);

/*
 * Validate user input format
 * If format is String, stringSize will contain string size
 * Returns if valid, throws if not
 */
void validateFormat(const std::string& format);

/*
 * Parse sizes of multiple params
 * Returns vector of sizes
 * Throws if invalid
 */
std::vector<std::pair<std::string,size_t> > getMultipleParamSizes(const std::string& format);

/*
 * Parses format string with size values for
 * multiple param formats
 * Returns list of std::pair<std::string, std::string> objects
 * Throws if invalid
 */
std::map<std::string, size_t> getMultipleParams(const std::string& format);

/*
 * Returns expected size of format
 */
size_t getFormatSize(const std::string& format);

/*
 * Checks if format string is a concatenation of
 * multiple valid formats
 * Returns true if valid, false if not
 */
bool isMultipleParam(const std::string& format);

/*
 * Checks if format string is of format "S[1-9][0-9]"
 * If valid, returns size specifed by format,
 * if not, returns six::Undefined<size_t>
 */
size_t isFormatStr(const std::string& format);

/*
 * Returns a std::pair<std::string, std::string> object with key and value between startPos and endPos
 * Throws exception if format is invalid
 */
std::pair<std::string, std::string> keyValueFinder(const std::string& format, size_t startPos, size_t endPos);

/*
 * Ostream operators for six::sicd::GeoInfo type
 */
std::ostream& operator<< (std::ostream& os, const GeoInfo& g);

/*
 * Ostream operators for six::MatchInformation type
 */
std::ostream& operator<< (std::ostream& os, const MatchInformation& m);

}
#endif
