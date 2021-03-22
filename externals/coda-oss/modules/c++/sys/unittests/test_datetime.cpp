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

#include <sys/LocalDateTime.h>
#include <sys/UTCDateTime.h>
#include <sys/OS.h>

#include "TestCase.h"

namespace
{

TEST_CASE(testDefaultConstructor)
{
    sys::OS os;

    sys::LocalDateTime l1;
    sys::UTCDateTime u1;

    os.millisleep(100);

    sys::LocalDateTime l2;
    sys::UTCDateTime u2;

    TEST_ASSERT(l1.getMonth() >= 1 && l1.getMonth() <= 12);
    TEST_ASSERT(l1.getDayOfMonth() >= 1 && l1.getDayOfMonth() <= 31);
    TEST_ASSERT(l1.getDayOfWeek() >= 1 && l1.getDayOfWeek() <= 7);
    TEST_ASSERT(l1.getDayOfYear() >= 1 && l1.getDayOfYear() <= 366);
    TEST_ASSERT(l1.getHour() >= 0 && l1.getHour() <= 23);
    TEST_ASSERT(l1.getMinute() >= 0 && l1.getMinute() <= 59);
    TEST_ASSERT(l1.getSecond() >= 0 && l1.getSecond() < 60);

    TEST_ASSERT(u1.getMonth() >= 1 && u1.getMonth() <= 12);
    TEST_ASSERT(u1.getDayOfMonth() >= 1 && u1.getDayOfMonth() <= 31);
    TEST_ASSERT(u1.getDayOfWeek() >= 1 && u1.getDayOfWeek() <= 7);
    TEST_ASSERT(u1.getDayOfYear() >= 1 && u1.getDayOfYear() <= 366);
    TEST_ASSERT(u1.getHour() >= 0 && u1.getHour() <= 23);
    TEST_ASSERT(u1.getMinute() >= 0 && u1.getMinute() <= 59);
    TEST_ASSERT(u1.getSecond() >= 0 && u1.getSecond() < 60);

    TEST_ASSERT(l2.getMonth() >= 1 && l2.getMonth() <= 12);
    TEST_ASSERT(l2.getDayOfMonth() >= 1 && l2.getDayOfMonth() <= 31);
    TEST_ASSERT(l2.getDayOfWeek() >= 1 && l2.getDayOfWeek() <= 7);
    TEST_ASSERT(l2.getDayOfYear() >= 1 && l2.getDayOfYear() <= 366);
    TEST_ASSERT(l2.getHour() >= 0 && l2.getHour() <= 23);
    TEST_ASSERT(l2.getMinute() >= 0 && l2.getMinute() <= 59);
    TEST_ASSERT(l2.getSecond() >= 0 && l2.getSecond() < 60);

    TEST_ASSERT(u2.getMonth() >= 1 && u2.getMonth() <= 12);
    TEST_ASSERT(u2.getDayOfMonth() >= 1 && u2.getDayOfMonth() <= 31);
    TEST_ASSERT(u2.getDayOfWeek() >= 1 && u2.getDayOfWeek() <= 7);
    TEST_ASSERT(u2.getDayOfYear() >= 1 && u2.getDayOfYear() <= 366);
    TEST_ASSERT(u2.getHour() >= 0 && u2.getHour() <= 23);
    TEST_ASSERT(u2.getMinute() >= 0 && u2.getMinute() <= 59);
    TEST_ASSERT(u2.getSecond() >= 0 && u2.getSecond() < 60);

    TEST_ASSERT(l1 == l1);
    TEST_ASSERT(l1 != l2);
    TEST_ASSERT(l1 < l2);
    TEST_ASSERT(l2 > l1);
}

TEST_CASE(testParameterizedConstructor)
{
    sys::UTCDateTime u1(0);

    TEST_ASSERT_EQ(u1.getYear(), 1970);
    TEST_ASSERT_EQ(u1.getMonth(), 1);
    TEST_ASSERT_EQ(u1.getHour(), 0);
    TEST_ASSERT_EQ(u1.getMinute(), 0);
    TEST_ASSERT_EQ(u1.getSecond(), 0);
    TEST_ASSERT_EQ(u1.getTimeInMillis(), 0);

    // test H:M:S constructor
    sys::LocalDateTime l2(10, 2, 42.24);
    TEST_ASSERT_EQ(l2.getHour(), 10);
    TEST_ASSERT_EQ(l2.getMinute(), 2);
    TEST_ASSERT_EQ(l2.getSecond(), 42.24);

    // test H:M:S constructor
    sys::UTCDateTime u2(10, 2, 42.24);
    TEST_ASSERT_EQ(u2.getHour(), 10);
    TEST_ASSERT_EQ(u2.getMinute(), 2);
    TEST_ASSERT_EQ(u2.getSecond(), 42.24);

    // test YYYY:MM:DD constructor
    sys::LocalDateTime l3(2000, 1, 1);
    TEST_ASSERT_EQ(l3.getYear(), 2000);
    TEST_ASSERT_EQ(l3.getMonth(), 1);
    TEST_ASSERT_EQ(l3.getDayOfMonth(), 1);
    TEST_ASSERT_EQ(l3.getDayOfYear(), 1);

    // test YYYY:MM:DD constructor
    sys::UTCDateTime u3(2000, 1, 1);
    TEST_ASSERT_EQ(u3.getYear(), 2000);
    TEST_ASSERT_EQ(u3.getMonth(), 1);
    TEST_ASSERT_EQ(u3.getDayOfMonth(), 1);
    TEST_ASSERT_EQ(u3.getDayOfYear(), 1);

    // test YYYY:MM:DD:H:M:S constructor
    sys::LocalDateTime l4(2012, 9, 17, 8, 22, 43.0);
    TEST_ASSERT_EQ(l4.getYear(), 2012);
    TEST_ASSERT_EQ(l4.getMonth(), 9);
    TEST_ASSERT_EQ(l4.getDayOfMonth(), 17);
    TEST_ASSERT_EQ(l4.getDayOfYear(), 261);
    TEST_ASSERT_EQ(l4.getHour(), 8);
    TEST_ASSERT_EQ(l4.getMinute(), 22);
    TEST_ASSERT_EQ(l4.getSecond(), 43.0);

    // test YYYY:MM:DD:H:M:S constructor
    sys::UTCDateTime u4(2012, 9, 17, 8, 22, 43.0);
    TEST_ASSERT_EQ(u4.getYear(), 2012);
    TEST_ASSERT_EQ(u4.getMonth(), 9);
    TEST_ASSERT_EQ(u4.getDayOfMonth(), 17);
    TEST_ASSERT_EQ(u4.getDayOfYear(), 261);
    TEST_ASSERT_EQ(u4.getHour(), 8);
    TEST_ASSERT_EQ(u4.getMinute(), 22);
    TEST_ASSERT_EQ(u4.getSecond(), 43.0);

    // test string/format constructor
    sys::LocalDateTime l5(l4.format());
    TEST_ASSERT_EQ(l5.getYear(), l4.getYear());
    TEST_ASSERT_EQ(l5.getMonth(), l4.getMonth());
    TEST_ASSERT_EQ(l5.getDayOfMonth(), l4.getDayOfMonth());
    TEST_ASSERT_EQ(l5.getDayOfYear(), l4.getDayOfYear());
    TEST_ASSERT_EQ(l5.getHour(), l4.getHour());
    TEST_ASSERT_EQ(l5.getMinute(), l4.getMinute());
    TEST_ASSERT_EQ(l5.getSecond(), l4.getSecond());

    // test string/format constructor
    sys::UTCDateTime u5(u4.format());
    TEST_ASSERT_EQ(u5.getYear(), u4.getYear());
    TEST_ASSERT_EQ(u5.getMonth(), u4.getMonth());
    TEST_ASSERT_EQ(u5.getDayOfMonth(), u4.getDayOfMonth());
    TEST_ASSERT_EQ(u5.getDayOfYear(), u4.getDayOfYear());
    TEST_ASSERT_EQ(u5.getHour(), u4.getHour());
    TEST_ASSERT_EQ(u5.getMinute(), u4.getMinute());
    TEST_ASSERT_EQ(u5.getSecond(), u4.getSecond());
}

static void testDateTimeDetails_(const std::string& testName, const tm& result, const sys::DateTime& dt)
{
    const auto ad = result.tm_year + 1900;  // "years since 1900"
    // this might break in 2038: https://en.wikipedia.org/wiki/Year_2038_problem
    TEST_ASSERT_GREATER_EQ(ad, 1900);
    TEST_ASSERT_GREATER_EQ(ad, 1970);
    TEST_ASSERT_GREATER_EQ(ad, 2021);
    TEST_ASSERT_LESSER_EQ(result.tm_yday, 365);  // "days since January 1"

    TEST_ASSERT_EQ(ad, dt.getYear());
    TEST_ASSERT_EQ(result.tm_yday + 1, dt.getDayOfYear());
}
TEST_CASE(testDateTimeDetails)
{
    const time_t now = time(nullptr);
    {
        tm local;
        sys::DateTime::localtime(now, local);
        testDateTimeDetails_(testName, local, sys::LocalDateTime());
    }
    {
        tm global;
        sys::DateTime::gmtime(now, global);
        testDateTimeDetails_(testName, global, sys::UTCDateTime());
    }
}

TEST_CASE(testGetTimeInMillis)
{
    const sys::LocalDateTime lt;
    const auto result = lt.getTimeInMillis();
    TEST_ASSERT_GREATER_EQ(result, 0.0);
    constexpr auto February_02_2021 = 1612928129.0 * 1000.0;  // in milliseconds
    TEST_ASSERT_GREATER_EQ(result, February_02_2021);

    constexpr auto recent_past = February_02_2021 * 0.999;
    TEST_ASSERT_GREATER_EQ(result, recent_past);

    constexpr auto far_into_the_future = February_02_2021 * 100.0;
    TEST_ASSERT_LESSER_EQ(result, far_into_the_future);
}

}

int main(int, char**)
{
    TEST_CHECK(testDefaultConstructor);
    TEST_CHECK(testParameterizedConstructor);
    TEST_CHECK(testDateTimeDetails);
    TEST_CHECK(testGetTimeInMillis);

    return 0;
}
