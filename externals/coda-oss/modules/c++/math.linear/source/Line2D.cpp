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

#include <cmath>
#include <math/linear/Line2D.h>
#include <except/Exception.h>
#include <sys/Conf.h>

namespace math
{
namespace linear
{
Line2D::Line2D(const Point& P1, const Point& P2)
{
    const double dx = P2.row - P1.row;
    const double dy = P2.col - P1.col;

    if ((dy == 0) && (dx == 0))
    {
        throw except::Exception(Ctxt("Cannot create a line when P1 == P2"));
    }
    //Vertical if x values are the same
    if (dx == 0)
    {
        mType = Line2D::VERTICAL;
        mSlope = 0; // undefined
        mXIntercept = P1.row;
        mYIntercept = 0; // undefined
    }
    // Horizontal if y values are the same
    else if (dy == 0)
    {
        mType = Line2D::HORIZONTAL;
        mSlope = 0;
        mYIntercept = P1.col;
        mXIntercept = 0; // undefined
    }
    else {
        mType = Line2D::NORMAL;
        mSlope = dy / dx;
        mYIntercept = P1.col - P1.row * mSlope;
        mXIntercept = x(0.0);
    }
}

Line2D::Line2D(const Point& P, double slope): mSlope(slope)
{
    if (mSlope == 0)
    {
        mType = Line2D::HORIZONTAL;
        mYIntercept = P.col;
        mXIntercept = 0; //undefined
    }
    else {
        mType = Line2D::NORMAL;
        mYIntercept = P.col - P.row * mSlope;
        mXIntercept = x(0.0);
    }
}

double Line2D::getSlope() const
{
    if (mType == Line2D::VERTICAL)
    {
        throw except::Exception(Ctxt("Vertical line, mSlope is undefined"));
    }
    return mSlope;
}

double Line2D::getYIntercept() const
{
    if (mType == Line2D::VERTICAL)
    {
        throw except::Exception(Ctxt("No return value for a vertical line with "
                "undefined mYIntercept"));
    }
    return mYIntercept;
}

double Line2D::getXIntercept() const
{
    if (mType == Line2D::HORIZONTAL)
    {
        throw except::Exception(Ctxt("No return value for a horizontal line "
                "with undefined mXIntercept"));
    }
    return mXIntercept;
}

double Line2D::y(double x) const
{
    if (mType == Line2D::VERTICAL)
    {
        throw except::Exception(Ctxt("Vertical line--cannot return a single"
                " y for given x"));
    }
    if (mType == Line2D::HORIZONTAL)
    {
        return mYIntercept;
    }
    return mSlope * x + mYIntercept;
}

double Line2D::x(double y) const
{
    if (mType == Line2D::HORIZONTAL)
    {
        throw except::Exception(Ctxt("Horizontal line--cannot return a single"
                " x for given y"));
    }
    if (mType == Line2D::VERTICAL)
    {
        return mXIntercept;
    }
    return (y - mYIntercept) / mSlope;
}
bool Line2D::intersection(const Line2D& rhs, Point& P) const
{
    if ((mSlope == rhs.mSlope) && (mType == rhs.mType))
    {
        return false;
    }
    if (mType == Line2D::VERTICAL)
    {
        P.row = mXIntercept;
        P.col = rhs.y(P.row);
    }
    else if (rhs.mType == Line2D::VERTICAL)
    {
        P.row = rhs.mXIntercept;
        P.col = y(P.row);
    }
    else
    {
        P.row = (rhs.mYIntercept - mYIntercept) / (mSlope - rhs.mSlope);
        P.col = y(P.row);
    }
    return true;
}
Line2D::Point Line2D::intersection(const Line2D& rhs) const
{
    Point P(0,0);
    if (!intersection(rhs, P))
    {
        throw except::Exception(Ctxt("Lines do not intersect"));
    }
    else 
    {
        return P;
    }
}

Line2D Line2D::parallelToLine(const Point& P) const
{
    if (mType == Line2D::VERTICAL)
    {
        // create a new vertical line through our point
        Point P2 = P;
        P2.col += 1;
        return Line2D(P, P2);
    }
    // other lines can just take the mSlope
    return Line2D(P, mSlope);
}

Line2D Line2D::perpendicularToLine(const Point& P) const
{
    if (mType == Line2D::HORIZONTAL)
    {
        // create a new vertical line through point P
        Point P2 = P;
        P2.col += 1; // offset in y
        return Line2D(P, P2);
    }
    if (mType == Line2D::VERTICAL)
    {
        // create a new horizontal line through point P
        Point P2 = P;
        P2.row += 1; // offset in x
        return Line2D(P, P2);
    }
    //Other lines can be created from the orthogonal mSlope and the point
    return Line2D(P, (-1.0 / mSlope));
}

double Line2D::distanceToPoint(const Point& P) const
{
    if (mType == Line2D::HORIZONTAL)
    {
        return std::abs(P.col - mYIntercept);
    }
    if (mType == Line2D::VERTICAL)
    {
        return std::abs(P.row - mXIntercept);
    }
    const double dist =
            std::abs(mSlope * P.row - P.col + mYIntercept) /
            std::sqrt(mSlope * mSlope + 1);
    return dist;
}

Line2D::Point Line2D::offsetFromPoint(const Point& P, double distance) const
{
    Point ret = P;
    if (mType == Line2D::HORIZONTAL)
    {
        ret.row += distance;
        return ret;
    }
    if (mType == Line2D::VERTICAL)
    {
        ret.col += distance;
        return ret;
    }
    double theta = std::atan(mSlope);
    ret.row += distance * std::cos(theta);
    ret.col += distance * std::sin(theta);
    return ret;
}
}
}

