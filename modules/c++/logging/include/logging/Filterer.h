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

#ifndef CODA_OSS_logging_Filterer_h_INCLUDED_
#define CODA_OSS_logging_Filterer_h_INCLUDED_
#pragma once

#include <string>
#include <map>

#include "config/Exports.h"

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
struct CODA_OSS_API Filterer
{
    Filterer() = default;
    virtual ~Filterer() = default;

    /*!
     * Adds a Filter to the managed map of Filters. We do NOT take control of
     * the pointer
     */
    void addFilter(Filter* filter);
    void addFilter(Filter&);

    virtual bool filter(const LogRecord* record) const;
    virtual bool filter(const LogRecord& record) const
    {
        return filter(&record);
    }

    //! Removes the specified Filter
    void removeFilter(Filter* filter);
    void removeFilter(Filter&);

protected:
    std::map<std::string, Filter*> filters;
};

}
#endif // CODA_OSS_logging_Filterer_h_INCLUDED_

