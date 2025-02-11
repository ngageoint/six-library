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
#include <algorithm>

#include <types/RangeList.h>

namespace
{
std::vector<types::Range>
difference(const types::Range& orig, const types::Range& overlap)
{
    // If the original range is fully contained by the overlap,
    // the difference is the empty set
    if (overlap.containsAll(orig.mStartElement, orig.mNumElements))
    {
        return {};
    }

    std::vector<types::Range> result;

    // Create left range
    if (orig.mStartElement < overlap.mStartElement)
    {
        result.emplace_back(
                orig.mStartElement,
                overlap.mStartElement - orig.mStartElement);
    }

    // Create right range
    if (orig.endElement() > overlap.endElement())
    {
        result.emplace_back(
                overlap.endElement(),
                orig.endElement() - overlap.endElement());
    }
    return result;
}
}

namespace types
{
RangeList::RangeList(const types::Range& range) :
    mRangeList(0)
{
    insert(range);
}

RangeList::RangeList(const std::vector<types::Range>& ranges) :
    mRangeList(0)
{
    for (const auto& inputRange : ranges)
    {
        insert(inputRange);
    }
}

size_t RangeList::getTotalNumElements() const
{
    size_t count = 0;
    for (const auto& range : mRangeList)
    {
        count += range.mNumElements;
    }
    return count;
}

void RangeList::insert(const std::vector<types::Range>& ranges)
{
    for (const auto& range : ranges)
    {
        insert(range);
    }
}

void RangeList::insert(const types::Range& range)
{
    if (range.empty())
    {
        return;
    }

    if (mRangeList.empty())
    {
        mRangeList.push_back(range);
        return;
    }

    std::vector<types::Range> newList;
    newList.reserve(mRangeList.size() + 1);

    // TODO: This can be sped up with a log(N) binary search and an insert
    //       into the newList from mRangeList
    size_t ii = 0;
    for (; ii < mRangeList.size(); ++ii)
    {
        if (mRangeList[ii].endElement() < range.mStartElement)
        {
            newList.push_back(mRangeList[ii]);
        }
        else
        {
            break;
        }
    }

    newList.push_back(range);

    for (; ii < mRangeList.size(); ++ii)
    {
        const types::Range& oldRange = mRangeList[ii];
        types::Range& backRange = newList.back();

        if (backRange.overlaps(oldRange) || backRange.touches(oldRange))
        {
            const size_t start = std::min<size_t>(backRange.mStartElement,
                                                  oldRange.mStartElement);

            const size_t end = std::max<size_t>(backRange.endElement(),
                                                oldRange.endElement());

            backRange.mStartElement = start;
            backRange.mNumElements = end - start;
        }
        else
        {
            newList.push_back(oldRange);
        }
    }

    mRangeList.swap(newList);
}

void RangeList::remove(const std::vector<types::Range>& ranges)
{
    for (const auto& range : ranges)
    {
        remove(range);
    }
}

void RangeList::remove(const types::Range& range)
{
    if (range.empty())
    {
        return;
    }

    std::vector<types::Range> newList;
    newList.reserve(mRangeList.size() + 1);

    for (const auto& oldRange : mRangeList)
    {
        types::Range overlap;
        const bool isOverlapping = oldRange.overlaps(range, overlap);

        if (isOverlapping)
        {
            for (const auto& diff : difference(oldRange, overlap))
            {
                newList.push_back(diff);
            }
        }
        else
        {
            newList.push_back(oldRange);
        }
    }

    mRangeList.swap(newList);
}

void RangeList::expand(size_t expansion, size_t maxEndElement)
{
    if (expansion == 0)
    {
        return;
    }

    const List oldRanges = getRanges();
    mRangeList.clear();

    for (const auto& range : oldRanges)
    {
        const size_t start =
                (range.mStartElement >= expansion) ?
                range.mStartElement - expansion : 0;

        const size_t end =
                std::min(range.endElement() + expansion, maxEndElement);

        insert(types::Range(start, end - start));
    }
}

RangeList RangeList::intersect(const RangeList& other) const
{
    RangeList output;
    auto iterA = std::begin(getRanges());
    auto iterB = std::begin(other.getRanges());
    const auto endIterA = std::end(getRanges());
    const auto endIterB = std::end(other.getRanges());

    while ((iterA != endIterA) && (iterB != endIterB))
    {
        const size_t start =
                std::max<size_t>(iterA->mStartElement, iterB->mStartElement);
        const size_t end =
                std::min<size_t>(iterA->endElement(), iterB->endElement());

        if (start < end)
        {
            output.insert(types::Range(start, end - start));
        }

        if (iterA->endElement() < iterB->endElement())
        {
            ++iterA;
        }
        else
        {
            ++iterB;
        }
    }

    return output;
}
}
