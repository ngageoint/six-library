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

#include <math.h>
#include <math/Constants.h>

#ifndef __CODA_CPP11
namespace math
{
const double Constants::FEET_TO_METERS = 0.3048;
const double Constants::METERS_TO_FEET = 1.0 / Constants::FEET_TO_METERS;
const double Constants::RADIANS_TO_DEGREES = 180.0 / M_PI;
const double Constants::DEGREES_TO_RADIANS = M_PI / 180.0;
const double Constants::NAUTICAL_MILES_TO_METERS = 1852.0;
const double Constants::METERS_TO_NAUTICAL_MILES =
        1.0 / Constants::NAUTICAL_MILES_TO_METERS;
const double Constants::NAUTICAL_MILES_TO_FEET =
        Constants::NAUTICAL_MILES_TO_METERS * Constants::FEET_TO_METERS;
const double Constants::SPEED_OF_LIGHT_METERS_PER_SEC = 299792458.0;
const double Constants::SPEED_OF_LIGHT_FEET_PER_SEC =
        Constants::SPEED_OF_LIGHT_METERS_PER_SEC * Constants::METERS_TO_FEET;
}

#endif
