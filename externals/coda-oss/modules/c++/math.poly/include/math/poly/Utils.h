/* =========================================================================
 * This file is part of math.poly-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * math.poly-c++ is free software; you can redistribute it and/or modify
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

#ifndef __MATH_POLY_UTILS_H__
#define __MATH_POLY_UTILS_H__

namespace math
{
namespace poly
{
/*!
 * Returns a scaled polynomial such that
 * P'(x, y) = P(x * scale, y * scale)
 *
 * If you had a polynomial in units of feet that you wanted in units of
 * meters, you would pass in a scale value of METERS_TO_FEET (since this
 * is what you would scale all your inputs by if you were using the
 * original polynomial in feet).
 *
 * \param poly The input polynomial.
 * \param scale The factor to apply to both x and y
 *
 * \return Scaled polynomial of the same order as the original polynomial
 */
template <typename PolyT>
PolyT scaleVariable(const PolyT& poly,
                    double scaleX,
                    double scaleY)
{
    PolyT newP = poly;

    double xCoeff = 1.0;
    for (size_t ii = 0; ii <= poly.orderX(); ++ii)
    {
        double yCoeff = 1.0;
        for (size_t jj = 0; jj <= poly.orderY(); ++jj)
        {
            newP[ii][jj] *= xCoeff * yCoeff;
            yCoeff *= scaleY;
        }
        xCoeff *= scaleX;
    }
    return newP;
}

/*!
 * Returns a scaled polynomial such that
 * P'(x) = P(x * scale)
 *
 * If you had a polynomial in units of feet that you wanted in units of
 * meters, you would pass in a scale value of METERS_TO_FEET (since this
 * is what you would scale all your inputs by if you were using the
 * original polynomial in feet).
 *
 * \param poly The input polynomial.
 * \param scale The factor to apply
 *
 * \return Scaled polynomial of the same order as the original polynomial
 */
template <typename PolyT>
PolyT scaleVariable(const PolyT& poly,
                    double scale)
{
    PolyT newP = poly;

    double coeff = 1.0;
    for (size_t ii = 0; ii <= poly.order(); ++ii)
    {
        newP[ii] *= coeff;
        coeff *= scale;
    }
    return newP;
}
}
}
#endif
