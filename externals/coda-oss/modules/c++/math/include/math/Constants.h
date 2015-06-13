/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __MATH_CONSTANTS_H__
#define __MATH_CONSTANTS_H__

namespace math
{
struct Constants
{
    static const double FEET_TO_METERS;
    static const double METERS_TO_FEET;

    static const double RADIANS_TO_DEGREES;
    static const double DEGREES_TO_RADIANS;

    static const double NAUTICAL_MILES_TO_METERS;
    static const double METERS_TO_NAUTICAL_MILES;
    static const double NAUTICAL_MILES_TO_FEET;

    static const double SPEED_OF_LIGHT_METERS_PER_SEC;
    static const double SPEED_OF_LIGHT_FEET_PER_SEC;
};
}

#endif
