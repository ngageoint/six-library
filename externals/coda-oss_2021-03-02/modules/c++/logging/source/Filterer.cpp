/* =========================================================================
 * This file is part of logging-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * logging-c++ is free software; you can redistribute it and/or modify
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

///////////////////////////////////////////////////////////
//  Filterer.cpp
///////////////////////////////////////////////////////////

#include "logging/Filterer.h"


void logging::Filterer::addFilter(logging::Filter* filter)
{
    if (filters.find(filter->getName()) == filters.end())
    {
        filters[filter->getName()] = filter;
    }
}

bool logging::Filterer::filter(const logging::LogRecord* record) const
{
    for (std::map<std::string, logging::Filter*>::const_iterator p = filters.begin();
            p != filters.end(); ++p)
    {
        if (!p->second->filter(record))
            return false;
    }
    return true;
}

void logging::Filterer::removeFilter(logging::Filter* filter)
{
    filters.erase(filter->getName());

}

