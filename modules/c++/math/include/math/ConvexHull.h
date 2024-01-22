/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
 *
 * math-c++ is free software; you can redistribute it and/or modify
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

#ifndef __MATH_CONVEX_HULL_H__
#define __MATH_CONVEX_HULL_H__

#include <algorithm>
#include <vector>
#include <limits>

#include <sys/Conf.h>

#include <str/Convert.h>

#include <except/Exception.h>

#include <types/RowCol.h>

namespace math
{
/*!
 *  This exists solely to enforce that T is a signed type.  If it is not,
 *  the confirm() method won't exist and we'll get a compilation error.
 */

template <bool TrueT>
struct MustBeSignedType
{
};

template <>
struct MustBeSignedType<true>
{
    static void confirm()
    {
    }
};

/*!
 *  \class ConvexHull
 *  \brief Uses the Graham scan algorithm to calculate the convex hull of
 *         a batch of points
 *
 *  This is a class rather than a function to simplify the implementation
 *
 *  Due to the implementation, T must be a signed type that can be used to
 *  add, subtract, and multiply without worrying about overflow or
 *  wrapping around.
 *  For example, sys::SSize_T or double.
 *  The class will not compile when instantiated with an unsigned type.
 *
 *  This implementation is based heavily on Mark Nelson's explanation and
 *  sample code at: http://marknelson.us/2007/08/22/convex.
 *  The article also appeared in a Dr. Dobb's article on 9/13/2007:
 *  http://www.ddj.com/architect/201806315.
 */
template <typename T>
class ConvexHull
{
public:
    typedef types::RowCol<T> RowCol;

    /*!
     *  Compute the convex hull.
     *
     *  As per convention, the last point will always be the first point
     *  repeated.
     *
     *  \param rawPoints Input points.  Destructive to input (sorts in
     *                   place to avoid a copy).
     *  \param convexHull [output] Convex hull points
     *
     */
    ConvexHull(std::vector<RowCol>& rawPoints,
               std::vector<RowCol>& convexHull)
    {
        if (rawPoints.size() < 2)
        {
            throw except::Exception(Ctxt(
                "ConvexHull constructor error: must use at least 2 input "
                "points but " + std::to_string(rawPoints.size()) + " were used"));
        }

        // Enforce (at compile time) that T is a signed type
        MustBeSignedType<std::numeric_limits<T>::is_signed>::confirm();

        partitionPoints(rawPoints);
        buildHull(convexHull);
    }

private:
    ConvexHull(const ConvexHull& );
    const ConvexHull& operator=(const ConvexHull& );

private:
    // Sorts on col, then row
    struct SortRowCol
    {
        bool operator()(const RowCol& lhs, const RowCol& rhs) const
        {
            if (lhs.col < rhs.col)
            {
                return true;
            }
            else if (rhs.col < lhs.col)
            {
                return false;
            }

            return (lhs.row < rhs.row);
        }
    };

    /*!
     *  Examines three consecutive points (p0, 01, and p2) and determines
     *  whether p2 has taken a turn to the left or a turn to the right.
     *  This is done by translating the points so that p1 is at the
     *  origin, then taking the cross product of p0 and p2.
     *
     *  Returns:
     *      > 0  p2 has turned right
     *      < 0  p2 has turned left
     *        0  p2 is on a straight line
     *
     */
    static
    sys::SSize_T direction(const RowCol& p0,
                           const RowCol& p1,
                           const RowCol& p2) noexcept
    {
        const T firstTerm = (p0.col - p1.col) * (p2.row - p1.row);
        const T secondTerm = (p2.col - p1.col) * (p0.row - p1.row);
        const T direction = firstTerm - secondTerm;

        if (direction > 0)
        {
            return 1;
        }
        else if (direction < 0)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }

    void partitionPoints(std::vector<RowCol> &rawPoints)
    {
        // Sort the raw points
        // This is done to get the far left and far right points of the
        // hull as well as to add the partitioned points in sorted order.
        std::sort(rawPoints.begin(), rawPoints.end(), SortRowCol());

        // Save off the far left and far right points
        mLeft = rawPoints.front();
        mRight = rawPoints.back();

        // Now run through the remaining raw points (excluding the far
        // left and far right points) and determine whether each point is
        // above or below the line formed by the right and left points.
        // If it is above, the point is moved into
        // 'mUpperPartitionPoints'. Otherwise, it's moved into
        // 'mLowerPartitionPoints'.
        mUpperPartitionPoints.reserve(rawPoints.size() - 2);
        mLowerPartitionPoints.reserve(rawPoints.size() - 2);
        const size_t end = rawPoints.size() - 1;
        for (size_t ii = 1; ii < end; ++ii)
        {
            RowCol const &rawPoint(rawPoints[ii]);
            if (direction(mLeft, mRight, rawPoint) < 0)
            {
                mUpperPartitionPoints.push_back(rawPoint);
            }
            else
            {
                mLowerPartitionPoints.push_back(rawPoint);
            }
        }
    }

    /*!
     *  Builds either the upper or the lower half convex hull.
     *
     * \param factor 1 for the lower hull, -1 for the upper hull
     * \param input Sorted list of points in one of the two halfs.
     *              This vector will be destroyed.
     * \param output [output] The points in the corresponding convex hull
     *
     */
    void buildHalfHull(sys::SSize_T         factor,
                       std::vector<RowCol>& input,
                       std::vector<RowCol>& output)
    {
        // The hull will always start with the left point and end with the
        // right point.  Accordingly, we start by adding the left point as
        // the first point in the output sequence and make sure the right
        // point is the last point in the input sequence.
        input.push_back(mRight);

        output.clear();
        output.push_back(mLeft);

        // The construction loop runs until the input is exhausted
        while (!input.empty())
        {
            // Repeatedly add the leftmost point to the hull, then test to
            // see if a convexity violation has occurred.  If it has, fix
            // things up by removing the next-to-last point in the output
            // sequence until convexity is restored.
            output.push_back(input.front());
            input.erase(input.begin());

            while (output.size() >= 3)
            {
                size_t const last = output.size() - 1;
                sys::SSize_T const dir = direction(output[last - 2],
                                                   output[last],
                                                   output[last - 1]);

                if (factor * dir <= 0)
                {
                    auto b = output.begin();
                    std::advance(b, static_cast<ptrdiff_t>(last) - 1);
                    output.erase(b);
                }
                else
                {
                    break;
                }
            }
        }
    }

    void buildHull(std::vector<RowCol>& convexHull)
    {
        // Building the hull consists of two procedures: building the
        // lower and then the upper hull. The two procedures are nearly
        // identical - the main difference between the two is the test for
        // convexity. When building the upper hull, our rule is that the
        // middle point must always be *above* the line formed by its two
        // closest neighbors. When building the lower hull, the rule is
        // that point must be *below* its two closest neighbors. We pass
        // this information to the building routine as the first
        // parameter, which is either -1 or 1.

        /// @note  This destroys 'mLowerPartitionPoints' and
        ///        'mUpperPartitionPoints
        std::vector<RowCol> lowerHull;
        buildHalfHull(1, mLowerPartitionPoints, lowerHull);

        std::vector<RowCol> upperHull;
        buildHalfHull(-1, mUpperPartitionPoints, upperHull);

        // The lower hull contains the first set of points, so copy this
        // in
        convexHull.clear();
        convexHull.swap(lowerHull);

        // Now copy in the upper hull in reverse order, skipping the last
        // point since it's duplicated in the lower hull.
        /// @note  The last point we add will be equal to the first point
        ///        point since we're not skipping it in the upper hull
        ///        even though it's a duplicate, but this seems to be
        ///        convention.
        for (typename std::vector<RowCol>::reverse_iterator iter =
                 upperHull.rbegin() + 1;
             iter != upperHull.rend();
             ++iter)
        {
            convexHull.push_back(*iter);
        }
    }

private:
    // The leftmost and rightmost points
    RowCol               mLeft;
    RowCol               mRight;

    // Sorted set of upper and lower partitioned points that lie inside
    // 'mLeft' and 'mRight'
    std::vector<RowCol>  mLowerPartitionPoints;
    std::vector<RowCol>  mUpperPartitionPoints;
};
}

#endif

