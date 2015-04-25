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

#include "sys/Conf.h"
#include "nitf/DateTime.hpp"

nitf::DateTime::DateTime() throw (nitf::NITFException)
{
    nitf_Error error;
    mDateTime = nitf_DateTime_now(&error);
    if (!mDateTime)
    {
        throw nitf::NITFException(&error);
    }
}

nitf::DateTime::DateTime(nitf_DateTime* dateTime) throw(nitf::NITFException) :
    mDateTime(dateTime)
{
    if (!dateTime)
    {
        throw nitf::NITFException(Ctxt("Invalid handle"));
    }
}

nitf::DateTime::DateTime(double timeInMillis) throw (nitf::NITFException)
{
    nitf_Error error;
    mDateTime = nitf_DateTime_fromMillis(timeInMillis, &error);
    if (!mDateTime)
    {
        throw nitf::NITFException(&error);
    }
}

nitf::DateTime::DateTime(const std::string& dateString,
        const std::string& dateFormat) throw (nitf::NITFException)
{
    nitf_Error error;
    mDateTime =
            nitf_DateTime_fromString(dateString.c_str(), dateFormat.c_str(),
                                     &error);
    if (!mDateTime)
    {
        throw nitf::NITFException(&error);
    }
}

nitf::DateTime::DateTime(int year,
                         int month,
                         int dayOfMonth,
                         int hour,
                         int minute,
                         double second)
{
    nitf_Error error;
    mDateTime = nitf_DateTime_fromMillis(0.0, &error);
    if (!mDateTime)
    {
        throw nitf::NITFException(&error);
    }

    setYear(year);
    setMonth(month);
    setDayOfMonth(dayOfMonth);
    setHour(hour);
    setMinute(minute);
    setSecond(second);
}

nitf::DateTime::~DateTime()
{
    nitf_DateTime_destruct(&mDateTime);
}

nitf::DateTime::DateTime(const nitf::DateTime& rhs)
{
    nitf_Error error;
    mDateTime = nitf_DateTime_fromMillis(rhs.getTimeInMillis(), &error);
    if (!mDateTime)
    {
        throw nitf::NITFException(&error);
    }
}

nitf::DateTime & nitf::DateTime::operator=(const nitf::DateTime& rhs)
{
    if (&rhs != this)
    {
        nitf_Error error;
        nitf_DateTime* const dateTime =
                nitf_DateTime_fromMillis(rhs.getTimeInMillis(), &error);
        if (!dateTime)
        {
            throw nitf::NITFException(&error);
        }

        nitf_DateTime_destruct(&mDateTime);
        mDateTime = dateTime;
    }

    return *this;
}

void nitf::DateTime::format(const std::string& format, char* outBuf,
        size_t maxSize) const throw (nitf::NITFException)
{
    nitf_Error error;
    if (!nitf_DateTime_format(mDateTime,
                              format.c_str(),
                              outBuf,
                              maxSize,
                              &error))
    {
        throw nitf::NITFException(&error);
    }
}

void nitf::DateTime::format(const std::string& format,
                            std::string &str) const throw(nitf::NITFException)
{
    str.clear();

    char raw[256];
    raw[255] = '\0';
    this->format(format, raw, 255);
    str.assign(raw);
}

std::string nitf::DateTime::format(const std::string& format) const
    throw(nitf::NITFException)
{
    std::string str;
    this->format(format, str);
    return str;
}

int nitf::DateTime::getYear() const
{
    return mDateTime->year;
}
int nitf::DateTime::getMonth() const
{
    return mDateTime->month;
}
int nitf::DateTime::getDayOfMonth() const
{
    return mDateTime->dayOfMonth;
}
int nitf::DateTime::getDayOfWeek() const
{
    return mDateTime->dayOfWeek;
}
int nitf::DateTime::getDayOfYear() const
{
    return mDateTime->dayOfYear;
}
int nitf::DateTime::getHour() const
{
    return mDateTime->hour;
}
int nitf::DateTime::getMinute() const
{
    return mDateTime->minute;
}
double nitf::DateTime::getSecond() const
{
    return mDateTime->second;
}
double nitf::DateTime::getTimeInMillis() const
{
    return mDateTime->timeInMillis;
}

void nitf::DateTime::setYear(int year)
{
    nitf_Error error;
    if (nitf_DateTime_setYear(mDateTime,
                              year,
                              &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setMonth(int month)
{
    nitf_Error error;
    if (nitf_DateTime_setMonth(mDateTime,
                               month,
                               &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setDayOfMonth(int dayOfMonth)
{
    nitf_Error error;
    if (nitf_DateTime_setDayOfMonth(mDateTime,
                                    dayOfMonth,
                                    &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setHour(int hour)
{
    nitf_Error error;
    if (nitf_DateTime_setHour(mDateTime,
                              hour,
                              &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setMinute(int minute)
{
    nitf_Error error;
    if (nitf_DateTime_setMinute(mDateTime,
                                minute,
                                &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setSecond(double second)
{
    nitf_Error error;
    if (nitf_DateTime_setSecond(mDateTime,
                                second,
                                &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setTimeInMillis(double timeInMillis)
{
    nitf_Error error;
    if (nitf_DateTime_setTimeInMillis(mDateTime,
                                      timeInMillis,
                                      &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
