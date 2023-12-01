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

#pragma once
#ifndef __CPHD_UTILITIES_H__
#define __CPHD_UTILITIES_H__

#include <iostream>
#include <string>
#include <vector>
#include <map>

#include <cphd/Enums.h>
#include <cphd/Types.h>
#include <cphd/Exports.h>

namespace cphd
{


/*
 *  \func getNumBytesPerSample
 *
 *  \brief Returns the number of bytes of the signal array format provided
 *
 *  \param signalArrayFormat An object of SignalArrayFormat described in Enums
 *  that defines the size of a sample in the signal block
 *
 *  \return Returns number of bytes: 2, 4 or 8. (0 if uninitialized)
 */
SIX_CPHD_API size_t getNumBytesPerSample(const SignalArrayFormat& signalArrayFormat);

/*
 *  \func validateFormat
 *
 *  \brief Validate user input format
 *
 *  \param format A format string.
 *   Valid binary formats are listed in CPHD 1.0 spec table 10.2, page 120
 *
 *  \throws except::Exception If format string is not valid binary format
 */
void validateFormat(const std::string& format);

/*
 *  \func getMultipleParamSizes
 *
 *  \brief Parse sizes of multiple params
 *
 *  \param format A format string.
 *   Valid binary formats are listed in CPHD 1.0 spec table 10.2, page 120
 *
 *  \throws except::Exception If format string is not valid binary format
 *
 *  \return Returns a vector of param name to param size pairs
 */
std::vector<std::pair<std::string,size_t> > getMultipleParamSizes(const std::string& format);

/*
 *  \func getFormatSize
 *
 *  \brief Return parameter size based on valid binary formats table 10.2 CPHD1.0 spec
 *
 *  \param format A format string.
 *   Valid binary formats are listed in CPHD 1.0 spec table 10.2, page 120
 *
 *  \throws except::Exception If format string is not valid binary format
 *
 *  \return Returns size of expected parameter
 */
size_t getFormatSize(const std::string& format);

/*
 *  \func isMultipleParam
 *
 *  \brief Checks if format is of multiple param type
 *
 *  \param format A format string.
 *   Valid binary formats are listed in CPHD 1.0 spec table 10.2, page 120
 *
 *  \return Returns true of format is multiple param type
 */
bool isMultipleParam(const std::string& format);

/*
 *  \func isFormatStr
 *
 *  \brief Checks if format is of string type
 *
 *  \param format A format string.
 *   Valid binary formats are listed in CPHD 1.0 spec table 10.2, page 120
 *
 *  \return Returns size specified by string format if true,
    if false returns six::Init::undefined<size_t>
 */
size_t isFormatStr(const std::string& format);

/*
 *  \func keyValueFinder
 *
 *  \brief Parses format string into key value pairs. (Helper for multiple param type)
 *
 *  \param format A format string.
 *   Valid binary formats are listed in CPHD 1.0 spec table 10.2, page 120
 *  \param startPos Offset from start of format string to start parsing
 *  \param endPos Offset from start of format string to end parsing
 *
 *  \throws except::Exception If format string is not valid binary format
 *
 *  \return std::pair<std::string, std::string> Returns pair of param name and param format
 */
std::pair<std::string, std::string> keyValueFinder(const std::string& format, size_t startPos, size_t endPos);
}
#endif
