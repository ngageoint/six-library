/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <import/nrt.h>
#include "Test.h"

TEST_CASE(testParseZeroDegrees)
{
    const char* geoString = "001234S";
    int degrees;
    int minutes;
    double seconds;
    nrt_Error error;
    nrt_Utils_parseGeographicString(geoString, &degrees, &minutes, &seconds,
            &error);
    double decimal = nrt_Utils_geographicToDecimal(degrees, minutes, seconds);
    TEST_ASSERT(fabs(decimal - -.2094444) < 1e-6);
}

TEST_CASE(testParseZeroMinutes)
{
    const char* geoString = "0000034W";
    int degrees;
    int minutes;
    double seconds;
    nrt_Error error;
    nrt_Utils_parseGeographicString(geoString, &degrees, &minutes, &seconds,
            &error);
    double decimal = nrt_Utils_geographicToDecimal(degrees, minutes, seconds);
    TEST_ASSERT(fabs(decimal - -.009444) < 1e-6);
}

TEST_CASE(testDecimalToDmsNegativeMinutes)
{
    const double decimal = -0.2094444;
    int degrees;
    int minutes;
    double seconds;
    nrt_Utils_decimalToGeographic(decimal, &degrees, &minutes, &seconds);
    TEST_ASSERT(degrees == 0);
    TEST_ASSERT(minutes == -12);
    TEST_ASSERT(fabs(seconds - 34) < 1);
}

TEST_CASE(testDecimalToDmsNegativeSeconds)
{
    const double decimal = -0.009444;
    int degrees;
    int minutes;
    double seconds;
    nrt_Utils_decimalToGeographic(decimal, &degrees, &minutes, &seconds);
    TEST_ASSERT(degrees == 0);
    TEST_ASSERT(minutes == 0);
    TEST_ASSERT(fabs(seconds - -34) < 1);
}

TEST_CASE(testParseDecimal)
{
    const char* decimalString = "+12.345";
    double decimal;
    nrt_Error error;
    nrt_Utils_parseDecimalString(decimalString, &decimal, &error);
    TEST_ASSERT(fabs(decimal - 12.345) < 1e-6);
}

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    CHECK(testParseZeroDegrees);
    CHECK(testParseZeroMinutes);
    CHECK(testDecimalToDmsNegativeMinutes);
    CHECK(testDecimalToDmsNegativeSeconds);
    CHECK(testParseDecimal);
    return 0;
}

