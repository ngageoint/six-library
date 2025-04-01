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
#ifndef __TYPES_RANGE_LIST_H__
#define __TYPES_RANGE_LIST_H__

#include <vector>
#include <limits>

#include <types/Range.h>

#include "config/Exports.h"

namespace types
{
/*!
 * \class RangeList
 * \brief Maintain an ordered collection of disjoint types::Range objects
 *
 * RangeList is meant to hold, and provide operations on, a set of ordered,
 * disjoint types::Range objects. Range objects are ordered by increasing
 * mStartElement.
 *
 *         +-------------+   +----------+       +-------------+
 *         | range 0     |   | range 1  |       | range 2     |
 *         +-------------+   +----------+       +-------------+
 *   |-------------------------------------------------------------->
 *   0                                                         max(size_t)
 *
 * Many of the operations are O(N), where N is the number of ranges in the
 * collection. As a result, caution should be taken when using this class
 * that N does not grow large enough to begin having a noticeable impact.
 *
 * TODO: Take a look at replacing the internals with some interval list
 *       structure.
 */
class CODA_OSS_API RangeList
{
public:
    /*!
     * Default constructor initializing an empty list
     */
    RangeList() = default;

    /*!
     * Constructor initializing the range list from a single range
     *
     * \param range Initial range
     */
    RangeList(const types::Range& range);

    /*!
     * Constructor initializing the range list from a vector of ranges.
     *
     * Initial ranges do not need to be disjoint, as insert() will be
     * called for each element in 'ranges', and consequently performing any
     * merges that need to occur.
     *
     * \param ranges Initial ranges
     */
    RangeList(const std::vector<types::Range>& ranges);

    /*!
     * Insert a range of size 1 at the given point
     *
     * O(N) operation, where N is the number of ranges already in the list.
     * If point overlaps/touches existing ranges, they will be merged.
     *
     * \param point The point to insert
     */
    void insert(size_t point)
    {
        insert(types::Range(point, 1));
    }

    /*!
     * Insert a range
     *
     * O(N) operation, where N is the number of ranges already in the list.
     * If range overlaps/touches existing ranges, they will be merged.
     * In set notation, the resulting list will be union(L, range), where
     * L is the the original list of ranges.
     *
     * \verbatim
     *
     * original    +-----+         +----------+       +-------------+
     * list        |     |         |          |       |             |
     *             +-----+         +----------+       +-------------+
     * range to               +---------------------------+
     * insert                 |                           |
     *                        +---------------------------+
     * final       +-----+    +-------------------------------------+
     * list        |     |    |                                     |
     *             +-----+    +-------------------------------------+
     *
     * \endverbatim
     *
     * \param range The range to insert
     */
    void insert(const types::Range& range);

    /*!
     * Insert a vector of ranges.
     *
     * O(M*N) operation, M is the size of 'ranges', and
     * N is the number of ranges already in the list.
     *
     * Will merge overlapping/touching ranges.
     *
     * \param ranges Vector of ranges to insert
     */
    void insert(const std::vector<types::Range>& ranges);

    /*!
     * Remove a range of size 1 at the given point
     *
     * O(N) operation, where N is the number of ranges in the list
     * If point lies inside a range, the range will be split in two.
     *
     * \param point The point to remove
     */
    void remove(size_t point)
    {
        remove(types::Range(point, 1));
    }

    /*!
     * Remove a range from the list
     *
     * O(N) operation, where N is the number of ranges already in the list.
     * In set notation, the resulting list will be L - intersection(L, range),
     * where L is the original list of ranges.
     *
     * \verbatim
     *
     * original    +-------------+   +----------+       +-------------+
     * list        |             |   |          |       |             |
     *             +-------------+   +----------+       +-------------+
     * range to               +---------------------------+
     * remove                 |                           |
     *                        +---------------------------+
     * final       +---------+                             +----------+
     * list        |         |                             |          |
     *             +---------+                             +----------+
     *
     * \endverbatim
     *
     * \param range The range to remove
     */
    void remove(const types::Range& range);

    /*!
     * Remove a vector of ranges
     *
     * O(M*N) operation, M is the size of 'ranges', and
     * N is the number of ranges in the list.
     *
     * \param ranges Vector of ranges to remove
     */
    void remove(const std::vector<types::Range>& ranges);

    /*!
     * \returns the number ranges in the list
     */
    size_t getNumRanges() const noexcept
    {
        return mRangeList.size();
    }

    /*!
     * \returns the total number elements as the sum(R.mNumElements) for
     *          all ranges R in the list
     */
    size_t getTotalNumElements() const noexcept;

    /*!
     * \returns true if getTotalNumElements() == 0
     */
    bool empty() const noexcept
    {
        return getTotalNumElements() == 0;
    }

    /*!
     * Get the vector of ranges that compose the range list
     *
     * Note that remove()/insert()/expand() will invalidate the
     * returned reference.
     *
     * \return the vector of ranges composing the range list
     */
    const std::vector<types::Range>& getRanges() const noexcept
    {
        return mRangeList;
    }

    /*!
     * Expand all ranges in the list by the given expansion factor, applied
     * to both the start and end of the range.
     * If the resulting expanded ranges touch/overlap, merging will occur.
     *
     * \param expansion Amount of expansion to apply to both the start
     *                  and end of each range in the list
     * \param maxEndElement Maximum end element for the range list.
     *                      There is an implicit minimum starting element of
     *                      0 (due to size_t), but this allows us to
     *                      specify a maximum end element, particularly useful
     *                      in an image setting, where you may not want to
     *                      go outside of a row or column dimension.
     *                      Default std::numeric_limits<size_t>::max().
     */
    void expand(size_t expansion,
                size_t maxEndElement = std::numeric_limits<size_t>::max());

    /*!
     * Determine a new range list that is the intersection of the current
     * list and another RangeList.
     *
     * \verbatim
     *
     * our         +--------+    +-------+   +--------+  +-+ +-+  +-+
     * list        |        |    |       |   |        |  | | | |  | |
     *             +--------+    +-------+   +--------+  +-+ +-+  +-+
     * other      +----------+     +---+      +-+ +-+   +-------+     +-+
     * list       |          |     |   |      | | | |   |       |     | |
     *            +----------+     +---+      +-+ +-+   +-------+     +-+
     * intersect   +--------+      +---+      +-+ +-+    +-+ +-+
     *             |        |      |   |      | | | |    | | | |
     *             +--------+      +---+      +-+ +-+    +-+ +-+
     *
     * \endverbatim
     *
     * \param other RangeList to intersect with
     * \returns new RangeList that is the intersection of this and other
     */
    RangeList intersect(const RangeList& other) const;

    bool operator==(const RangeList& rhs) const
    {
        const auto& rhsRanges = rhs.getRanges();

        if (mRangeList.size() != rhsRanges.size())
        {
            return false;
        }

        for (size_t idx = 0; idx < mRangeList.size(); idx++)
        {
            if (mRangeList[idx] != rhsRanges[idx])
            {
                return false;
            }
        }
        return true;
    }
    bool operator!=(const RangeList& rhs) const
    {
        return !(*this == rhs);
    }


private:
    using List = std::vector<types::Range>;
    List mRangeList;
};
}

#endif
