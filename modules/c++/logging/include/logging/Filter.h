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
//  Filter.h
///////////////////////////////////////////////////////////

#ifndef CODA_OSS_logging_Filter_h_INCLUDED_
#define CODA_OSS_logging_Filter_h_INCLUDED_
#pragma once

#include <string>
#include "logging/LogRecord.h"

namespace logging
{

/*!
 * \class Filter
 *
 * \brief Filter instances are used to perform arbitrary filtering of LogRecords.
 */
struct Filter
{
    Filter(std::string name = "") : mName(name){}
    virtual ~Filter() = default;

    virtual bool filter(const LogRecord* record) const;
    virtual bool filter(const LogRecord& record) const
    {
        return filter(&record);
    }

    std::string getName() const
    {
        return mName;
    }

protected:
    std::string mName;
};

}
#endif  // CODA_OSS_logging_Filter_h_INCLUDED_
