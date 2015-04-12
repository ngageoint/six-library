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

public:

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
    //! Return the Daylight Savings Time flag (true = on, false = off)
    bool getDST() const { return mDST == 1; }

    // Setters
    void setMonth(int month);
    void setDayOfMonth(int dayOfMonth);
    void setHour(int hour);
    void setMinute(int minute);
    void setSecond(double second);
    void setTimeInMillis(double time);
    void setYear(int year);
    void setDST(bool isDST);
};

}

#endif
