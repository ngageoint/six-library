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
//  Filterer.h
///////////////////////////////////////////////////////////

#ifndef __LOGGING_FILTERER_H__
#define __LOGGING_FILTERER_H__

#include <string>
#include <map>
#include "logging/Filter.h"
#include "logging/LogRecord.h"

namespace logging
{

/*!
 * \class Filterer
 *
 * A base class for loggers and handlers which allows them to share
 * common code.
 */
class Filterer
{
public:
    Filterer(){}
    virtual ~Filterer(){}

    /*!
     * Adds a Filter to the managed map of Filters. We do NOT take control of
     * the pointer
     */
    void addFilter(Filter* filter);

    bool filter(const LogRecord* record) const;

    //! Removes the specified Filter
    void removeFilter(Filter* filter);

protected:
    std::map<std::string, Filter*> filters;

};

}
#endif
