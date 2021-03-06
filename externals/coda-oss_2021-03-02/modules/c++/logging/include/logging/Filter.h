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

#ifndef __LOGGING_FILTER_H__
#define __LOGGING_FILTER_H__

#include <string>
#include "logging/LogRecord.h"

namespace logging
{

/*!
 * \class Filter
 *
 * \brief Filter instances are used to perform arbitrary filtering of LogRecords.
 */
class Filter
{
public:
    Filter(std::string name = "") : mName(name){}
    virtual ~Filter(){}

    bool filter(const LogRecord* record) const;
    std::string getName() const { return mName; }

protected:
    std::string mName;
};

}
#endif
