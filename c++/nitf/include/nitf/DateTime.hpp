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

/*!
 *  \file DateTime.hpp
 *  \brief  Contains wrapper implementation for DateTime (UTC)
 */

namespace nitf
{

/*!
 *  \class DateTime
 *  \brief  The C++ wrapper for the nitf_DateTime object
 *  Note that, unlike most of the C++ bindings, this is a deep copy
 */
class DateTime
{
public:
    //! Sets to current date/time
    DateTime() throw(nitf::NITFException);

    //! Set native object - takes ownership
    DateTime(nitf_DateTime* dateTime) throw(nitf::NITFException);

    DateTime(double timeInMillis) throw(nitf::NITFException);

    DateTime(const std::string& dateString,
             const std::string& dateFormat) throw(nitf::NITFException);

    ~DateTime();

    //! Copy constructor
    DateTime(const DateTime& rhs);

    //! Assignment Operator
    DateTime & operator=(const DateTime& rhs);

    bool operator<(const DateTime& rhs) const
    {
        return (mDateTime->timeInMillis < rhs.mDateTime->timeInMillis);
    }

    bool operator<=(const DateTime& rhs) const
    {
        return (mDateTime->timeInMillis <= rhs.mDateTime->timeInMillis);
    }

    bool operator>(const DateTime& rhs) const
    {
        return (mDateTime->timeInMillis > rhs.mDateTime->timeInMillis);
    }

    bool operator>=(const DateTime& rhs) const
    {
        return (mDateTime->timeInMillis >= rhs.mDateTime->timeInMillis);
    }

    bool operator==(const DateTime& rhs) const
    {
        return (mDateTime->timeInMillis == rhs.mDateTime->timeInMillis);
    }

    bool operator!=(const DateTime& rhs) const
    {
        return (mDateTime->timeInMillis != rhs.mDateTime->timeInMillis);
    }

    nitf_DateTime* getNative()
    {
        return mDateTime;
    }

    const nitf_DateTime* getNative() const
    {
        return mDateTime;
    }

    void format(const std::string& format,
                char* outBuf,
                size_t maxSize) const throw(nitf::NITFException);

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

    void setYear(int year);
    void setMonth(int month);
    void setDayOfMonth(int dayOfMonth);
    void setDayOfWeek(int dayOfWeek);
    void setDayOfYear(int dayOfYear);
    void setHour(int hour);
    void setMinute(int minute);
    void setSecond(double second);
    void setTimeInMillis(double timeInMillis);

private:
    nitf_DateTime* mDateTime;
};

}
#endif
