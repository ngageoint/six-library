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


#ifndef __SYS_LOCAL_DATE_TIME_H__
#define __SYS_LOCAL_DATE_TIME_H__

#include <sys/DateTime.h>

namespace sys
{

/*!
 *  Representation of a local date/time structure.
 */
class LocalDateTime : public DateTime
{
protected:
    int mDST;

    //! @brief Set members from the tm struct value.
    virtual void fromMillis(const tm& t);

    /**
     * @brief Set the millis value from the members
     */
    virtual void toMillis();

    // ! Given seconds since the epoch, provides the local time
    virtual void getTime(time_t numSecondsSinceEpoch, tm& t) const;

public:
    static const char DEFAULT_DATETIME_FORMAT[];

    /*!
     *  Construct as current date and time (localtime).
     */
    LocalDateTime();
    /*!
     *  Construct with time values.  Date will be today.
     */
    LocalDateTime(int hour, int minute, double second);
    /*!
     *  Construct with date values.  Time will be 00:00:00.
     */
    LocalDateTime(int year, int month, int day);
    /*!
     *  Construct with date and time values.
     */
    LocalDateTime(int year, int month, int day, 
             int hour, int minute, double second);
    /*!
     *  Construct with time in milliseconds.
     */
    LocalDateTime(double timeInMillis);
    /*!
     *  Construct with string/format.
     */
    LocalDateTime(const std::string& time,
            const std::string& format = DEFAULT_DATETIME_FORMAT);

    //! Return the Daylight Savings Time flag (true = on, false = off)
    bool getDST() const { return mDST == 1; }

    //! Set the Daylight Savings Time flag (true = on, false = off)
    void setDST(bool isDST);

    // unhide in the base class format method
    using DateTime::format;

    /*!
     *  The default formatting looks like this:
     *  %y%-M%-d_%H:%m:%s
     *  2011-10-19_11:59:46
     */
    std::string format() const;
};

std::ostream& operator<<(std::ostream& os, const LocalDateTime& dateTime);
std::istream& operator>>(std::istream& is, LocalDateTime& dateTime);
}

#endif
