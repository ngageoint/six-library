/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

#include <except/Exception.h>
#include <six/Parameter.h>
#include <six/ParameterCollection.h>

namespace six
{
size_t
ParameterCollection::findParameterIndex(const std::string& paramName) const
{
    for(size_t ii = 0; ii < mParams.size(); ++ii)
    {
        if (paramName == mParams[ii].getName())
        {
            return ii;
        }
    }

    throw except::NoSuchKeyException(Ctxt(
        "No parameter with name \"" + paramName
        + "\" found in this collection"));
}

bool
ParameterCollection::containsParameter(const std::string& paramName) const
{
    for(size_t ii = 0; ii < mParams.size(); ++ii)
    {
        if (paramName == mParams[ii].getName())
        {
            return true;
        }
    }

    return false;
}

bool
ParameterCollection::operator==(const ParameterCollection& rhs) const
{
    return mParams == rhs.mParams;
}
}

