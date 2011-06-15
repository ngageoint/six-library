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

#include "nitf/DateTime.hpp"

nitf::DateTime::DateTime(const nitf::DateTime & x)
{
    setNative(x.getNative());
}

nitf::DateTime & nitf::DateTime::operator=(const nitf::DateTime & x)
{
    if (&x != this)
        setNative(x.getNative());
    return *this;
}

nitf::DateTime::DateTime(nitf_DateTime * x)
{
    setNative(x);
    getNativeOrThrow();
}

nitf::DateTime::DateTime(double timeInMillis) throw (nitf::NITFException)
{
    setNative(nitf_DateTime_fromMillis(timeInMillis, &error));
    getNativeOrThrow();
    setManaged(false);
}

nitf::DateTime::DateTime(const std::string& dateString,
        const std::string& dateFormat) throw (nitf::NITFException)
{
    setNative(nitf_DateTime_fromString(dateString.c_str(), dateFormat.c_str(),
            &error));
    getNativeOrThrow();
    setManaged(false);
}

nitf::DateTime::DateTime() throw (nitf::NITFException)
{
    setNative(nitf_DateTime_now(&error));
    getNativeOrThrow();
    setManaged(false);
}

nitf::DateTime::~DateTime()
{
}

void nitf::DateTime::format(const std::string& format, char* outBuf,
        size_t maxSize) const throw (nitf::NITFException)
{
    nitf_Error e;
    if (!nitf_DateTime_format(getNativeOrThrow(), format.c_str(), outBuf,
            maxSize, &e))
        throw nitf::NITFException(&e);
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
    return getNativeOrThrow()->year;
}
int nitf::DateTime::getMonth() const
{
    return getNativeOrThrow()->month;
}
int nitf::DateTime::getDayOfMonth() const
{
    return getNativeOrThrow()->dayOfMonth;
}
int nitf::DateTime::getDayOfWeek() const
{
    return getNativeOrThrow()->dayOfWeek;
}
int nitf::DateTime::getDayOfYear() const
{
    return getNativeOrThrow()->dayOfYear;
}
int nitf::DateTime::getHour() const
{
    return getNativeOrThrow()->hour;
}
int nitf::DateTime::getMinute() const
{
    return getNativeOrThrow()->minute;
}
double nitf::DateTime::getSecond() const
{
    return getNativeOrThrow()->second;
}
double nitf::DateTime::getTimeInMillis() const
{
    return getNativeOrThrow()->timeInMillis;
}

void nitf::DateTime::setYear(int year)
{
    if (nitf_DateTime_setYear(getNativeOrThrow(),
                              year,
                              &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setMonth(int month)
{
    if (nitf_DateTime_setMonth(getNativeOrThrow(),
                               month,
                               &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setDayOfMonth(int dayOfMonth)
{
    if (nitf_DateTime_setDayOfMonth(getNativeOrThrow(),
                                    dayOfMonth,
                                    &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setDayOfWeek(int dayOfWeek)
{
    if (nitf_DateTime_setDayOfWeek(getNativeOrThrow(),
                                   dayOfWeek,
                                   &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setDayOfYear(int dayOfYear)
{
    if (nitf_DateTime_setDayOfYear(getNativeOrThrow(),
                                   dayOfYear,
                                   &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setHour(int hour)
{
    if (nitf_DateTime_setHour(getNativeOrThrow(),
                              hour,
                              &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setMinute(int minute)
{
    if (nitf_DateTime_setMinute(getNativeOrThrow(),
                                minute,
                                &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setSecond(double second)
{
    if (nitf_DateTime_setSecond(getNativeOrThrow(),
                                second,
                                &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
void nitf::DateTime::setTimeInMillis(double timeInMillis)
{
    if (nitf_DateTime_setTimeInMillis(getNativeOrThrow(),
                                      timeInMillis,
                                      &error) != NITF_SUCCESS)
    {
        throw nitf::NITFException(&error);
    }
}
