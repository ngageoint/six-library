/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
#include "nitf/exports.hpp"

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
struct NITRO_NITFCPP_API DateTime
{
    //! Sets to current date/time
    DateTime() noexcept(false);

    //! Set native object - takes ownership
    DateTime(nitf_DateTime* dateTime);

    /*
     * Construct from a specified date/time
     *
     * \param timeInMillis Number of milliseconds since the epoch (1/1/1970)
     */
    DateTime(double timeInMillis);

    /*!
     * Construct from a string representation of a date
     * \param dateString Date in string form
     * \param dateFormat Description of date format.  This follows the
     * formatters that strftime() uses.  A few common formatters are:
     * Year:         %Y
     * Month:        %m
     * Day of month: %d
     * Hour:         %H
     * Minute:       %M
     * Seconds:      %S
     *
     * For example, the NITF 2.1 format is represented as "%Y%m%d%H%M%S"
     */
    DateTime(const std::string& dateString,
             const std::string& dateFormat);

    /*!
     * Construct from a specified year, month, and day
     *
     * \param year Year
     * \param month Month (1-based)
     * \param dayOfMonth Day of month (1-based)
     * \param hour Hour [0, 23].  Defaults to 0.
     * \param minute Minute [0, 59].  Defaults to 0.
     * \param second Second [0, 60).  Defaults to 0.
     */
    DateTime(int year,
             int month,
             int dayOfMonth,
             int hour = 0,
             int minute = 0,
             double second = 0.0);

    ~DateTime();

    //! Copy constructor
    DateTime(const DateTime& rhs);

    //! Assignment Operator
    DateTime & operator=(const DateTime& rhs);

    bool operator<(const DateTime& rhs) const noexcept
    {
        return (mDateTime->timeInMillis < rhs.mDateTime->timeInMillis);
    }

    bool operator<=(const DateTime& rhs) const noexcept
    {
        return (mDateTime->timeInMillis <= rhs.mDateTime->timeInMillis);
    }

    bool operator>(const DateTime& rhs) const noexcept
    {
        return (mDateTime->timeInMillis > rhs.mDateTime->timeInMillis);
    }

    bool operator>=(const DateTime& rhs) const noexcept
    {
        return (mDateTime->timeInMillis >= rhs.mDateTime->timeInMillis);
    }

    bool operator==(const DateTime& rhs) const noexcept
    {
        return (mDateTime->timeInMillis == rhs.mDateTime->timeInMillis);
    }

    bool operator!=(const DateTime& rhs) const noexcept
    {
        return (mDateTime->timeInMillis != rhs.mDateTime->timeInMillis);
    }

    nitf_DateTime* getNative() noexcept
    {
        return mDateTime;
    }

    const nitf_DateTime* getNative() const noexcept
    {
        return mDateTime;
    }

    /*
     * Produce a string representation of the date/time, formatted as
     * requested
     *
     * \param format Description of date format.  See constructor for details.
     * \param outBuf Buffer to store the formatted date/time in
     * \param maxSize The maximum length of formatted string 'outBuf' can hold
     */
    void format(const std::string& format,
                char* outBuf,
                size_t maxSize) const;

    /*
     * Produce a string representation of the date/time, formatted as
     * requested
     *
     * \param format Description of date format.  See constructor for details.
     * \param str Output string to store the formatted date/time in
     */
    void format(const std::string& format,
                std::string &str) const;

    /*
     * Produce a string representation of the date/time, formatted as
     * requested
     *
     * \param format Description of date format.  See constructor for details.
     *
     * \return Formatted date/time
     */
    std::string format(const std::string& format) const;

    //! Get the year
    int getYear() const noexcept;

    //! Get the month (1-based)
    int getMonth() const noexcept;

    //! Get the day of the month (1-based)
    int getDayOfMonth() const noexcept;

    //! Get the day of the week (0-based with 0 = Saturday)
    int getDayOfWeek() const noexcept;

    //! Get the day of the year (1-based)
    int getDayOfYear() const noexcept;

    //! Get the hour [0, 23]
    int getHour() const noexcept;

    //! Get the minute [0, 59]
    int getMinute() const noexcept;

    //! Get the second [0, 60), including fractional seconds
    double getSecond() const noexcept;

    //! Get the number of milliseconds since the epoch (1/1/1970)
    double getTimeInMillis() const noexcept;

    //! Set the year
    void setYear(int year);

    //! Set the month (1-based)
    void setMonth(int month);

    //! Set the day of the month (1-based)
    void setDayOfMonth(int dayOfMonth);

    //! Set the hour [0, 23]
    void setHour(int hour);

    //! Set the minute [0, 59]
    void setMinute(int minute);

    //! Set the second [0, 60), including fractional seconds
    void setSecond(double second);

    //! Set the time in milliseconds since the epoch (1/1/1970)
    void setTimeInMillis(double timeInMillis);

private:
    nitf_DateTime* mDateTime;
};

}
#endif
