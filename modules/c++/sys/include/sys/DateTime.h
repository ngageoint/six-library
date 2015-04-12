/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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


#ifndef __SYS_DATETIME_H__
#define __SYS_DATETIME_H__

#include <string>
#include <time.h>

namespace sys
{

/*!
 *  Representation of a date/time structure.
 */
class DateTime
{
protected:
    int mYear;
    int mMonth;
    int mDayOfMonth;
    int mDayOfWeek;
    int mDayOfYear;
    int mHour;
    int mMinute;
    double mSecond;
    double mTimeInMillis;
    int mDST;
    
    // Set members from the millis value
    void fromMillis();
    // Set the millis value from the members
    void toMillis();
    // Turn a tm struct into a double
    double toMillis(tm t) const;
    /*! 
     * Set the time to right now.  
     * Uses time() or if HAVE_SYS_TIME_H is defined, 
     * gettimeofday() for usec precision.
     */
    void setNow();
    // Provides the local time as a 'tm'
    void getLocalTime(tm &localTime) const;
    // Provides the GM time as a 'tm'
    void getGMTime(tm &gmTime) const;

public:
    static const char DEFAULT_DATETIME_FORMAT[];
    static const char FORMAT_ISO_8601[];

    /*!
     *  Construct as current date and time (localtime).
     */
    DateTime();

    ~DateTime(){}

    /*!
     *  Construct with time values.  Date will be today.
     */
    DateTime(int hour, int minute, double second);
    /*!
     *  Construct with date values.  Time will be 00:00:00.
     */
    DateTime(int year, int month, int day);
    /*!
     *  Construct with date and time values.
     */
    DateTime(int year, int month, int day, 
             int hour, int minute, double second);
    /*!
     *  Construct with time in milliseconds.
     */
    DateTime(double timeInMillis);

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
    //! Return millis since 1 Jan 1970 localtime
    double getTimeInMillis() const { return mTimeInMillis; }
    //! Return millis since 1 Jan 1970 GMT
    double getGMTimeInMillis() const;
    //! Return the current year
    int getYear() const { return mYear; }
    //! Return the Timezone (in hours)
    float getTimezoneOffset()
    {
        return (float)(getTimeInMillis() - getGMTimeInMillis()) / 3600000;
    }
    //! Return the Daylight Savings Time flag (true = on, false = off)
    bool getDST() const { return mDST == 1; }

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

    // ! Given seconds since the epoch, provides the local time
    static
    void getLocalTime(time_t numSecondsSinceEpoch, tm &localTime);
    // ! Given seconds since the epoch, provides the GM time
    static
    void getGMTime(time_t numSecondsSinceEpoch, tm &gmTime);

    // Setters
    void setMonth(int month);
    void setDayOfMonth(int dayOfMonth);
    void setHour(int hour);
    void setMinute(int minute);
    void setSecond(double second);
    void setTimeInMillis(double time);
    void setYear(int year);
    void setTimezoneOffset(float offsetInHours);
    void setDST(bool isDST);

    /*!
     *  format the DateTime string
     *  y = year (YYYY)
     *  M = month (MM)
     *  d = day (DD)
     *  H = hour (hh)
     *  m = minute (mm)
     *  s = second (ss)
     *
     *  The default format looks like this:
     *  %y%-M%-d_%H:%m:%s
     *  2011-10-19_11:59:46
     */
    std::string format(const std::string& formatStr = DEFAULT_DATETIME_FORMAT) const;

};

}

#endif
