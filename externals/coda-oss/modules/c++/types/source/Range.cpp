/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, Radiant Geospatial Solutions
 *
 * types-c++ is free software; you can redistribute it and/or modify
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
#include <types/Range.h>

namespace types
{
Range Range::split(size_t numElementsReq) const
{
    // If the input range is empty, we can't remove any elements from it...
    if (empty() || numElementsReq == 0)
    {
        return types::Range();
    }

    // If fewer elements are requested than are in inputRange,
    // pull off numElementsReq elements and update the split range
    if (numElementsReq <= mNumElements)
    {
        return types::Range(endElement() - numElementsReq,
                            numElementsReq);
    }
    // Otherwise, if more elements are requested than are in inputRange,
    // set the split range to be input range (denoting that we have taken
    // all of the elements)
    else
    {
        return types::Range(mStartElement, mNumElements);
    }
}
}
