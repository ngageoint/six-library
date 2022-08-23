/* =========================================================================
 * This file is part of math.linear-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * math.linear-c++ is free software; you can redistribute it and/or modify
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

#ifndef __MATH_LINEAR_LINE_2D_H__
#define __MATH_LINEAR_LINE_2D_H__

#include <types/RowCol.h>

namespace math
{
namespace linear
{
class Line2D
{
public:
    typedef types::RowCol<double> Point;
    enum Line2DType { NORMAL, HORIZONTAL, VERTICAL };

    Line2D(const Point& P1, const Point& P2);
    Line2D(const Point& P, double slope);
    double getSlope() const;
    double getYIntercept() const;
    double getXIntercept() const;
    //Evaluate for y given x:
    double y(double x) const;
    //Evaluate for x given y:
    double x(double y) const;
    // Determine intersection of two lines
    Point intersection(const Line2D& rhs) const;
    // Determines if the current line intersects with the other
    // If the lines do intersect, P is altered to be that intersecting point
    bool intersection(const Line2D& rhs, Point& P) const;
    // Create a new line parallel to this line through point P
    Line2D parallelToLine(const Point& P) const;
    // Create a new line perpendicular to this line through point P
    Line2D perpendicularToLine(const Point& P) const;
    // Compute the distance from this line to a point
    double distanceToPoint(const Point& P) const;
    //Return a point that is a distance d from the point P which is on the line
    Point offsetFromPoint(const Point& P, double distance) const;
    //
    bool equals(const Line2D& other) const;

    friend bool operator==(const Line2D& lhs, const Line2D& rhs)
    {
        return lhs.equals(rhs);
    }
private:
    Line2DType mType;
    double mSlope;
    double mYIntercept;
    double mXIntercept;
};
}
}
#endif
