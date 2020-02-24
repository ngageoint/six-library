/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
#include "six/sicd/Timeline.h"

namespace six
{
namespace sicd
{
TimelineSet::TimelineSet() :
    tStart(Init::undefined<double>()),
    tEnd(Init::undefined<double>()),
    interPulsePeriodStart(Init::undefined<int>()),
    interPulsePeriodEnd(Init::undefined<int>())
{
}

bool TimelineSet::operator==(const TimelineSet& rhs) const
{
    return (tStart == rhs.tStart &&
        tEnd == rhs.tEnd &&
        interPulsePeriodStart == rhs.interPulsePeriodStart &&
        interPulsePeriodEnd == rhs.interPulsePeriodEnd &&
        interPulsePeriodPoly == rhs.interPulsePeriodPoly);
}

Timeline::Timeline() :
    collectStart(Init::undefined<DateTime>()),
    collectDuration(Init::undefined<double>())
{
}
}
}
