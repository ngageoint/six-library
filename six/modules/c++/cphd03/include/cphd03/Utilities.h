/* =========================================================================
 * This file is part of cphd03-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * cphd03-c++ is free software; you can redistribute it and/or modify
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


#ifndef __CPHD03_UTILITIES_H__
#define __CPHD03_UTILITIES_H__

#include <str/Manip.h>
#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd03
{
// Return bytes/sample, either 2, 4, or 8 (or 0 if not initialized)
size_t getNumBytesPerSample(cphd::SampleType sampleType);

template<typename T> std::string toString(const T& value)
{
    return str::toString<T>(value);
}

template<> std::string toString(const cphd::CollectionInformation& ci);
}

#endif
