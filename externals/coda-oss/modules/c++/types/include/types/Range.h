/* =========================================================================
 * This file is part of types-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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
#ifndef __TYPES_RANGE_H__
#define __TYPES_RANGE_H__

#include <limits>

namespace types
{
/*!
 *  \class Range
 *  \brief Holds range information i.e a starting element and number of
 *  elements in a range (exclusive)
 */
struct Range
{
    /*!
     * Initializes to an empty range
     */
    Range() :
        mStartElement(std::numeric_limits<size_t>::max()),
        mNumElements(0)
    {
    }

    /*!
     * \param startElement Start of range
     * \param numElements Number of elements in range
     */
    Range(size_t startElement, size_t numElements) :
        mStartElement(startElement),
        mNumElements(numElements)
    {
    }

    size_t mStartElement;
    size_t mNumElements;

    /*!
     * \return The end element (exclusive)
     */
    size_t endElement() const
    {
        return mStartElement + mNumElements;
    }

    /*!
     * \param element The element to check
     *
     * \return True if this element is contained in this AOI, false otherwise
     */
    bool contains(size_t element) const
    {
        return (element >= mStartElement && element < endElement());
    }

    /*!
     * \param startElement The start element
     * \param numELements The total number of elements to check
     *
     * \return True if [startElement, startElement + numElements) is contained
     * in this AOI, false otherwise
     */
    bool containsAll(size_t startElement, size_t numElements) const
    {
        return (numElements == 0 ||
                (contains(startElement) && contains(startElement + numElements - 1)));
    }

    //! \return True if there are no elements in this range, false otherwise
    bool empty() const
    {
        return (mNumElements == 0);
    }
};
}

#endif
