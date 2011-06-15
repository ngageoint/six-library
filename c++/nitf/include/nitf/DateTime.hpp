/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2010, General Dynamics - Advanced Information Systems
 *
 * NITRO is free software; you can redistribute it and/or modify
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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_DATETIME_HPP__
#define __NITF_DATETIME_HPP__

#include "nitf/System.hpp"
#include "nitf/NITFException.hpp"
#include "nitf/Object.hpp"

/*!
 *  \file DateTime.hpp
 *  \brief  Contains wrapper implementation for DateTime (UTC)
 */

namespace nitf
{

/*!
 *  \class DateTime
 *  \brief  The C++ wrapper for the nitf_DateTime object
 */
DECLARE_CLASS(DateTime)
{
public:

    //! Copy constructor
    DateTime(const DateTime & x);

    //! Assignment Operator
    DateTime & operator=(const DateTime & x);

    //! Set native object
    DateTime(nitf_DateTime * x);

    DateTime(double timeInMillis) throw(nitf::NITFException);

    DateTime(const std::string& dateString,
            const std::string& dateFormat) throw(nitf::NITFException);

    DateTime() throw(nitf::NITFException);

    ~DateTime();

    void format(const std::string& format,
            char* outBuf, size_t maxSize) const throw(nitf::NITFException);

    void format(const std::string& format,
                std::string &str) const throw(nitf::NITFException);

    std::string format(const std::string& format) const
        throw(nitf::NITFException);

    int getYear() const;
    int getMonth() const;
    int getDayOfMonth() const;
    int getDayOfWeek() const;
    int getDayOfYear() const;
    int getHour() const;
    int getMinute() const;
    double getSecond() const;
    double getTimeInMillis() const;

    void setYear(int);
    void setMonth(int);
    void setDayOfMonth(int);
    void setDayOfWeek(int);
    void setDayOfYear(int);
    void setHour(int);
    void setMinute(int);
    void setSecond(double);
    void setTimeInMillis(double);

private:
    nitf_Error error;
};

}
#endif
