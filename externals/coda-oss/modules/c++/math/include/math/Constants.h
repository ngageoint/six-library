/* =========================================================================
 * This file is part of math-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
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

#ifndef __MATH_CONSTANTS_H__
#define __MATH_CONSTANTS_H__

#include <math.h>
namespace math
{
struct Constants
{
    static constexpr double FEET_TO_METERS = 0.3048;
    static constexpr double METERS_TO_FEET = 1.0 / FEET_TO_METERS;

    static constexpr double RADIANS_TO_DEGREES = 180.0 / M_PI;
    static constexpr double DEGREES_TO_RADIANS = M_PI / 180.0;

    static constexpr double NAUTICAL_MILES_TO_METERS = 1852.0;
    static constexpr double METERS_TO_NAUTICAL_MILES =
                                    1.0 / NAUTICAL_MILES_TO_METERS;
    static constexpr double NAUTICAL_MILES_TO_FEET =
                                    NAUTICAL_MILES_TO_METERS * METERS_TO_FEET;

    static constexpr double SPEED_OF_LIGHT_METERS_PER_SEC = 299792458.0;
    static constexpr double SPEED_OF_LIGHT_FEET_PER_SEC =
            SPEED_OF_LIGHT_METERS_PER_SEC * METERS_TO_FEET;
};
}
#endif
