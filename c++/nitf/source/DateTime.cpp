/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

nitf::DateTime::DateTime(double timeInMillis) throw(nitf::NITFException)
{
    setNative(nitf_DateTime_fromMillis(timeInMillis, &error));
    getNativeOrThrow();
    setManaged(false);
}

nitf::DateTime::DateTime(const std::string& dateString,
        const std::string& dateFormat) throw(nitf::NITFException)
{
    setNative(nitf_DateTime_fromString(dateString.c_str(),
            dateFormat.c_str(), &error));
    getNativeOrThrow();
    setManaged(false);
}

nitf::DateTime::DateTime() throw(nitf::NITFException)
{
    setNative(nitf_DateTime_now(&error));
    getNativeOrThrow();
    setManaged(false);
}

nitf::DateTime::~DateTime()
{
}

void nitf::DateTime::format(const std::string& format, char* outBuf,
        size_t maxSize) throw(nitf::NITFException)
{
    if (!nitf_DateTime_format(getNativeOrThrow(), format.c_str(),
            outBuf, maxSize, &error))
        throw nitf::NITFException(&error);
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
	getNativeOrThrow()->year = year;
}
void nitf::DateTime::setMonth(int month)
{
	getNativeOrThrow()->month = month;
}
void nitf::DateTime::setDayOfMonth(int dayOfMonth)
{
	getNativeOrThrow()->dayOfMonth = dayOfMonth;
}
void nitf::DateTime::setDayOfWeek(int dayOfWeek)
{
	getNativeOrThrow()->dayOfWeek = dayOfWeek;
}
void nitf::DateTime::setDayOfYear(int dayOfYear)
{
	getNativeOrThrow()->dayOfYear = dayOfYear;
}
void nitf::DateTime::setHour(int hour)
{
	getNativeOrThrow()->hour = hour;
}
void nitf::DateTime::setMinute(int minute)
{
	getNativeOrThrow()->minute = minute;
}
void nitf::DateTime::setSecond(double second)
{
	getNativeOrThrow()->second = second;
}
void nitf::DateTime::setTimeInMillis(double timeInMillis)
{
	getNativeOrThrow()->timeInMillis = timeInMillis;
}
