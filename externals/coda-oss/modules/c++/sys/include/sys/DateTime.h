/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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


#ifndef CODA_OSS_sys_DateTime_h_INCLUDED_
#define CODA_OSS_sys_DateTime_h_INCLUDED_
#pragma once

#include <time.h>
#include <stdint.h>

#include <string>

namespace sys
{

/*!
 *  Representation of a date/time structure.
 */
class DateTime
{
protected:
    int mYear = 0;
    int mMonth = 0;
    int mDayOfMonth = 0;
    int mDayOfWeek = 0;
    int mDayOfYear = 0;
    int mHour = 0;
    int mMinute = 0;
    double mSecond = 0.0;
    double mTimeInMillis = 0.0;

    // Turn a tm struct into a double
    double toMillis(tm t) const;

    /*! 
     * Set the time to right now.  
     * Uses time() or if HAVE_SYS_TIME_H is defined, 
     * gettimeofday() for usec precision.
     */
    void setNow();

    //! @brief Set members from the millis value.
    void fromMillis();

    //! @brief Set members from the tm struct value.
    virtual void fromMillis(const tm& t);

    //! @brief Set the millis value from the members
    virtual void toMillis() = 0;

    //! @brief Provides the time as a 'tm'
    void getTime(tm& t) const;

    //! @brief Given seconds since the epoch, provides the time
    virtual void getTime(time_t numSecondsSinceEpoch, tm& t) const = 0;

public: // for unit-testing
    static void localtime(time_t numSecondsSinceEpoch, tm& t);
    static void gmtime(time_t numSecondsSinceEpoch, tm& t);

public:
    DateTime() = default;
    virtual ~DateTime() {}

    //! Return month {1,12}
    int getMonth() const { return mMonth; }
    //! Return day of month {1,31}
    int getDayOfMonth() const { return mDayOfMonth; }
    //! Return day of week {1,7}
    int getDayOfWeek() const { return mDayOfWeek; }
    //! Return day of year {1,366}
    int getDayOfYear() const { return mDayOfYear; }
    //! Return hour {0,23}
    int getHour() const { return mHour; }
    //! Return minute {0,59}
    int getMinute() const { return mMinute; }
    //! Return second {0,59}
    double getSecond() const { return mSecond; }
    //! Return millis since 1 Jan 1970
    double getTimeInMillis() const { return mTimeInMillis; }
    //! Return the current year
    int getYear() const { return mYear; }
    //! Return the number of seconds since the time epoch
    static int64_t getEpochSeconds() noexcept;

    // ! Given the {1,12} month return the alphabetic equivalent
    static std::string monthToString(int month);
    // ! Given the {1,7} day of the week return the alphabetic equivalent
    static std::string dayOfWeekToString(int dayOfWeek);

    // ! Given the {1,12} month return the abbreviated alphabetic equivalent
    static std::string monthToStringAbbr(int month);
    // ! Given the {1,7} day, return the abbreviated alphabetic equivalent
    static std::string dayOfWeekToStringAbbr(int dayOfWeek);

    // ! Given the alphabetic or abbreviated version return {1,12} equivalent 
    // Acceptable input "August" or "Aug" would return 8
    static int monthToValue(const std::string& month);
    // ! Given the alphabetic or abbreviated version return {1,7} equivalent 
    // Acceptable input "Wednesday" or "Wed" would return 4
    static int dayOfWeekToValue(const std::string& dayOfWeek);

    // Setters
    void setMonth(int month);
    void setDayOfMonth(int dayOfMonth);
    void setHour(int hour);
    void setMinute(int minute);
    void setSecond(double second);
    void setTimeInMillis(double time);
    void setYear(int year);

    //! @brief Set members from a string/format.
    void setTime(const std::string& time, const std::string& format);

    /*!
     *  format the DateTime string
     *  y = year (YYYY)
     *  m = month (MM)
     *  d = day (DD)
     *  H = hour (hh)
     *  M = minute (mm)
     *  S = second (ss)
     */
    std::string format(const std::string& formatStr) const;

    /**
     * @name Logical Operators.
     * @brief Logical comparison operators.
     *
     * @param rhs The object to compare against.
     *
     * @return true if comparison holds, false otherwise.
     */
    //@{
    bool operator<(const DateTime& rhs) const
    {
        return (mTimeInMillis < rhs.mTimeInMillis);
    }

    bool operator<=(const DateTime& rhs) const
    {
        return (mTimeInMillis <= rhs.mTimeInMillis);
    }

    bool operator>(const DateTime& rhs) const
    {
        return (mTimeInMillis > rhs.mTimeInMillis);
    }

    bool operator>=(const DateTime& rhs) const
    {
        return (mTimeInMillis >= rhs.mTimeInMillis);
    }

    bool operator==(const DateTime& rhs) const
    {
        return (mTimeInMillis == rhs.mTimeInMillis);
    }

    bool operator!=(const DateTime& rhs) const
    {
        return !operator==(rhs);
    }
    //@}
};

// Always make our own versions available for unit-testing.  Clients should use
// DateTme methods and implementers DateTime::localtime()/DateTime::gmtime().
namespace details
{
extern int localtime_s(tm*, const time_t*);
extern int gmtime_s(tm*, const time_t*);
}

}

#endif//CODA_OSS_sys_DateTime_h_INCLUDED_
