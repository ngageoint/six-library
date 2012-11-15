/* =========================================================================
 * This file is part of six.sicd-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

using namespace six;
using namespace six::sicd;

TimelineSet::TimelineSet()
{
    tStart = Init::undefined<double>();
    tEnd = Init::undefined<double>();
    interPulsePeriodStart = Init::undefined<int>();
    interPulsePeriodEnd = Init::undefined<int>();
}
InterPulsePeriod::~InterPulsePeriod()
{
    for (unsigned int i = 0; i < sets.size(); ++i)
    {
        TimelineSet* set = sets[i];
        delete set;
    }
}
InterPulsePeriod* InterPulsePeriod::clone() const
{
    InterPulsePeriod* ipp = new InterPulsePeriod;
    
    for (unsigned int i = 0; i < sets.size(); ++i)
    {
        TimelineSet* set = sets[i];
        ipp->sets.push_back( set->clone() );
    }
    return ipp;
    
}
Timeline::~Timeline()
{
    if (interPulsePeriod)
        delete interPulsePeriod;
}

Timeline* Timeline::clone() const
{
    Timeline* tl = new Timeline(*this);
    if (interPulsePeriod)
        tl->interPulsePeriod = interPulsePeriod->clone();
    return tl;
}
