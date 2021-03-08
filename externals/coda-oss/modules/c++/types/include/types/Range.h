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

#include <algorithm>
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
                (contains(startElement) &&
                 contains(startElement + numElements - 1)));
    }

    /*!
     * \param rhs Range to compare with
     *
     * \return True if the ranges overlap, false otherwise
     */
    bool overlaps(const types::Range& rhs) const
    {
        return (endElement() > rhs.mStartElement &&
                mStartElement < rhs.endElement());
    }

    /*!
     * \param rhs Range to compare with
     * \param[out] overlap Overlapping range (will be empty if there is no
     * overlap)
     *
     * \return True if the ranges overlap, false otherwise
     */
    bool overlaps(const types::Range& rhs,
                  types::Range& overlap) const
    {
        if (overlaps(rhs))
        {
            const size_t end = std::min(endElement(), rhs.endElement());
            overlap.mStartElement = std::max(mStartElement, rhs.mStartElement);
            overlap.mNumElements = end - overlap.mStartElement;
            return true;
        }
        else
        {
            overlap = types::Range();
            return false;
        }
    }

    /*!
     * Determine if a given range touches our range. Touching ranges do not
     * overlap, but can be placed next to one another (irrespective of order)
     * with no missing values in between.
     *
     * If either of the ranges is empty, touching is defined as always false.
     *
     * \param rhs Range to compare with
     *
     * \return True if the ranges touch, false otherwise
     */
    bool touches(const types::Range& rhs) const
    {
        if (empty() || rhs.empty())
        {
            return false;
        }

        return (mStartElement == rhs.endElement()) ||
               (rhs.mStartElement == endElement());
    }

    /*!
     * \param startElementToTest The start element
     * \param numElementsToTest The total number of elements to check
     *
     * \return The number of shared elements
     */
    size_t getNumSharedElements(size_t startElementToTest,
                                size_t numElementsToTest) const
    {
        types::Range overlap;
        overlaps(types::Range(startElementToTest, numElementsToTest), overlap);
        return overlap.mNumElements;
    }

    /*!
     * Given an input range, removes numElementsReq elements from the end of it
     * (if possible), and provides the new range composed of these elements
     *
     * \param inputRange Initial Range object
     * \param numElementsReq The number elements requested to be split from
     *        the end of inputRange
     *
     * \returns The resulting range formed from the removed elements of
     *          inputRange. Has no more than numElementsReq elements,
     *          but may have fewer depending on the size of inputRange.
     */
    Range split(size_t numElementsReq) const;

    //! \return True if there are no elements in this range, false otherwise
    bool empty() const
    {
        return (mNumElements == 0);
    }

    /*!
     * \param rhs Range to compare with
     *
     * \return True if ranges match, false otherwise
     */
    bool operator==(const Range& rhs) const
    {
        return (mStartElement == rhs.mStartElement &&
                mNumElements == rhs.mNumElements);
    }

    /*!
     * \param rhs Range to compare with
     *
     * \return False if ranges match, true otherwise
     */
    bool operator!=(const Range& rhs) const
    {
        return !(*this == rhs);
    }

    /*!
     * Comparator.  Primarily useful so that Range can be used as a key in an
     * STL map.  Sorting first by the start element allows us to iterate
     * through a map in order of when ranges start, which is generally speaking
     * what we want.
     *
     * \param rhs Range to compare with
     *
     * \return True if this < rhs, false otherwise
     */
    bool operator<(const Range& rhs) const
    {
        if (mStartElement < rhs.mStartElement)
        {
            return true;
        }
        else if (rhs.mStartElement < mStartElement)
        {
            return false;
        }
        else
        {
            return (mNumElements < rhs.mNumElements);
        }
    }
};
}

#endif
