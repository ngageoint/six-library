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
    double decimal;
    nrt_Utils_parseGeographicString(geoString, &degrees, &minutes, &seconds,
            &error);
    decimal = nrt_Utils_geographicToDecimal(degrees, minutes, seconds);
    TEST_ASSERT(fabs(decimal - -.2094444) < 1e-6);
}

TEST_CASE(testParseZeroMinutes)
{
    const char* geoString = "0000034W";
    int degrees;
    int minutes;
    double seconds;
    nrt_Error error;
    double decimal;
    nrt_Utils_parseGeographicString(geoString, &degrees, &minutes, &seconds,
            &error);
    decimal = nrt_Utils_geographicToDecimal(degrees, minutes, seconds);
    TEST_ASSERT(fabs(decimal - -.009444) < 1e-6);
}

TEST_CASE(testParseZeroMinutesEast)
{
    const char* geoString = "0000034E";
    int degrees;
    int minutes;
    double seconds;
    nrt_Error error;
    double decimal;
    nrt_Utils_parseGeographicString(geoString, &degrees, &minutes, &seconds,
            &error);
    decimal = nrt_Utils_geographicToDecimal(degrees, minutes, seconds);
    TEST_ASSERT(fabs(decimal - .009444) < 1e-6);
}

TEST_CASE(testParseErrorGeographicString)
{
    int degrees;
    int minutes;
    double seconds;
    nrt_Error error;
    NRT_BOOL result = nrt_Utils_parseGeographicString("999999N", &degrees, &minutes, &seconds,
        &error);
    TEST_ASSERT_EQ_INT((NRT_BOOL)NRT_FALSE, result);

    result = nrt_Utils_parseGeographicString("9999999E", &degrees, &minutes, &seconds,
        &error);
    TEST_ASSERT_EQ_INT((NRT_BOOL)NRT_FALSE, result);
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

TEST_CASE(testDecimalToDmsPositiveMinutes)
{
    const double decimal = 0.2094444;
    int degrees;
    int minutes;
    double seconds;
    nrt_Utils_decimalToGeographic(decimal, &degrees, &minutes, &seconds);
    TEST_ASSERT(degrees == 0);
    TEST_ASSERT(minutes == 12);
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

TEST_CASE(testDecimalToDmsPositiveSeconds)
{
    const double decimal = 0.009444;
    int degrees;
    int minutes;
    double seconds;
    nrt_Utils_decimalToGeographic(decimal, &degrees, &minutes, &seconds);
    TEST_ASSERT(degrees == 0);
    TEST_ASSERT(minutes == 0);
    TEST_ASSERT(fabs(seconds - 34) < 1);
}

TEST_CASE(testParseDecimal)
{
    const char* decimalString = "+12.345";
    double decimal;
    nrt_Error error;
    nrt_Utils_parseDecimalString(decimalString, &decimal, &error);
    TEST_ASSERT(fabs(decimal - 12.345) < 1e-6);
}

TEST_CASE(testDmsToCharArrayPositiveDMS)
{
    char lonCharArray[9];
    char latCharArray[8];

    nrt_Utils_geographicLonToCharArray(1, -1, 10, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(1, -1, 10, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(1, 0, 70, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(1, 0, 70, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(1, -1, 70, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(1, -1, 70, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(1, 60, 70, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(1, 60, 70, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
}

TEST_CASE(testDmsToCharArrayNegativeDMS)
{
    char lonCharArray[9];
    char latCharArray[8];

    nrt_Utils_geographicLonToCharArray(-1, -1, 10, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(-1, -1, 10, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(-1, 0, 70, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(-1, 0, 70, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(-1, -1, 70, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(-1, -1, 70, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(-1, 60, 70, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(-1, 60, 70, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
}

TEST_CASE(testDmsToCharArrayNegativeDegrees)
{
    char lonCharArray[9];
    char latCharArray[8];

    nrt_Utils_geographicLonToCharArray(-1, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "0010000W");
    nrt_Utils_geographicLatToCharArray(-1, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "010000S");

    nrt_Utils_geographicLatToCharArray(-89, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "890000S");
    nrt_Utils_geographicLatToCharArray(-90, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "900000S");
    nrt_Utils_geographicLatToCharArray(-91, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(-179, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "1790000W");
    nrt_Utils_geographicLonToCharArray(-180, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "1800000W");
    nrt_Utils_geographicLonToCharArray(-181, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");

    nrt_Utils_geographicLatToCharArray(-179, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
    nrt_Utils_geographicLatToCharArray(-180, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
    nrt_Utils_geographicLatToCharArray(-181, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(-359, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLonToCharArray(-360, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLonToCharArray(-361, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");

    nrt_Utils_geographicLatToCharArray(-359, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
    nrt_Utils_geographicLatToCharArray(-360, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
    nrt_Utils_geographicLatToCharArray(-361, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
}

TEST_CASE(testDmsToCharArrayPositiveDegrees)
{
    char lonCharArray[9];
    char latCharArray[8];

    nrt_Utils_geographicLonToCharArray(0, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "0000000E");
    nrt_Utils_geographicLatToCharArray(0, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "000000N");

    nrt_Utils_geographicLonToCharArray(1, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "0010000E");
    nrt_Utils_geographicLatToCharArray(1, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "010000N");

    nrt_Utils_geographicLatToCharArray(89, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "890000N");
    nrt_Utils_geographicLatToCharArray(90, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "900000N");
    nrt_Utils_geographicLatToCharArray(91, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(179, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "1790000E");
    nrt_Utils_geographicLonToCharArray(180, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "1800000E");
    nrt_Utils_geographicLonToCharArray(181, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");

    nrt_Utils_geographicLatToCharArray(179, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
    nrt_Utils_geographicLatToCharArray(180, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
    nrt_Utils_geographicLatToCharArray(181, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(359, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLonToCharArray(360, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLonToCharArray(361, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");

    nrt_Utils_geographicLatToCharArray(359, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
    nrt_Utils_geographicLatToCharArray(360, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
    nrt_Utils_geographicLatToCharArray(361, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
}

TEST_CASE(testDmsToCharArrayNegativeMinutes)
{
    char lonCharArray[9];
    char latCharArray[8];

    nrt_Utils_geographicLonToCharArray(0, -1, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "0000100W");
    nrt_Utils_geographicLatToCharArray(0, -1, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "000100S");

    nrt_Utils_geographicLonToCharArray(0, -1, -10, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(0, -1, -10, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(0, -61, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(0, -61, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(0, -1, 10, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "0000110W"); 
    nrt_Utils_geographicLatToCharArray(0, -1, 10, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "000110S");
}

TEST_CASE(testDmsToCharArrayPositiveMinutes)
{
    char lonCharArray[9];
    char latCharArray[8];

    nrt_Utils_geographicLonToCharArray(0, 1, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "0000100E");
    nrt_Utils_geographicLatToCharArray(0, 1, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "000100N");

    nrt_Utils_geographicLonToCharArray(0, 1, 13, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "0000113E");
    nrt_Utils_geographicLatToCharArray(0, 1, 13, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "000113N");

    nrt_Utils_geographicLonToCharArray(0, 61, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(0, 61, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(0, 60, -2, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(0, 60, -2, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
}

TEST_CASE(testDmsToCharArrayNegativeSeconds)
{
    char lonCharArray[9];
    char latCharArray[8];

    nrt_Utils_geographicLonToCharArray(0, 0, -13, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "0000013W");

    nrt_Utils_geographicLatToCharArray(0, 0, -13, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "000013S");

    nrt_Utils_geographicLonToCharArray(0, 1, -73, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");

    nrt_Utils_geographicLatToCharArray(0, 1, -73, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(0, 0, -73, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");

    nrt_Utils_geographicLatToCharArray(0, 0, -73, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
}

TEST_CASE(testDmsToCharArrayPositiveSeconds)
{
    char lonCharArray[9];
    char latCharArray[8];

	nrt_Utils_geographicLonToCharArray(0, 0, 13, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "0000013E");
	nrt_Utils_geographicLatToCharArray(0, 0, 13, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "000013N");

    nrt_Utils_geographicLonToCharArray(0, 0, 73, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(0, 0, 73, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");

    nrt_Utils_geographicLonToCharArray(0, -1, 73, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "9999999E");
    nrt_Utils_geographicLatToCharArray(0, -1, 73, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "999999N");
}

TEST_CASE(testDmsToCharArrayZero)
{
    char lonCharArray[9];
    nrt_Utils_geographicLonToCharArray(0, 0, 0, lonCharArray);
    TEST_ASSERT_EQ_STR(lonCharArray, "0000000E");

    char latCharArray[8];
    nrt_Utils_geographicLatToCharArray(0, 0, 0, latCharArray);
    TEST_ASSERT_EQ_STR(latCharArray, "000000N");
}

TEST_CASE(test_matlab_dms)
{
    // https://www.mathworks.com/help/map/angle-representations-and-angular-units.html#bragfhi
    int degrees, minutes; double seconds;
    nrt_Utils_decimalToGeographic(-57.29578 /* -1 radian */, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(-57, degrees);
    TEST_ASSERT_EQ_INT(17, minutes);
    TEST_ASSERT_EQ_FLOAT(44.808, seconds);

    nrt_Utils_decimalToGeographic(-57 /* ~ -1 radian */, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(-57, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);

    nrt_Utils_decimalToGeographic(57.29578 /* 1 radian */, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(57, degrees);
    TEST_ASSERT_EQ_INT(17, minutes);
    TEST_ASSERT_EQ_FLOAT(44.808, seconds);

    nrt_Utils_decimalToGeographic(57 /* ~ -1 radian */, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(57, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);

    // https://www.mathworks.com/help/map/angle-representations-and-angular-units.html#bragfhi
    // "Rather than storing the sign in a separate element, degrees2dm applies to the first nonzero element in each row."
    nrt_Utils_decimalToGeographic(1.0, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(1, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);
    nrt_Utils_decimalToGeographic(0.1, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(0, degrees);
    TEST_ASSERT_EQ_INT(6, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);
    nrt_Utils_decimalToGeographic(-0.1, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(0, degrees);
    TEST_ASSERT_EQ_INT(-6, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);
    nrt_Utils_decimalToGeographic(0.01, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(0, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(36.0, seconds);
    nrt_Utils_decimalToGeographic(-0.01, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(0, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(-36.0, seconds);
    nrt_Utils_decimalToGeographic(0.001, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(0, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(3.6, seconds);
    nrt_Utils_decimalToGeographic(-0.001, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(0, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(-3.6, seconds);

    nrt_Utils_decimalToGeographic(0.0, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(0, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);
    nrt_Utils_decimalToGeographic(360.0, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(360, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);
    nrt_Utils_decimalToGeographic(-360.0, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(-360, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);
    nrt_Utils_decimalToGeographic(361.0, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(361, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);
    nrt_Utils_decimalToGeographic(-361.0, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(-361, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);
}


TEST_MAIN(
    (void)argc;
    (void)argv;
    CHECK(testParseZeroDegrees);
    CHECK(testParseZeroMinutes);
    CHECK(testParseErrorGeographicString);
    CHECK(testParseZeroMinutesEast);
    CHECK(testDecimalToDmsNegativeMinutes);
    CHECK(testDecimalToDmsPositiveMinutes);
    CHECK(testDecimalToDmsNegativeSeconds);
    CHECK(testDecimalToDmsPositiveSeconds);
    CHECK(testParseDecimal);
    CHECK(testDmsToCharArrayNegativeDMS);
    CHECK(testDmsToCharArrayPositiveDMS);
    CHECK(testDmsToCharArrayNegativeDegrees);
    CHECK(testDmsToCharArrayPositiveDegrees);
    CHECK(testDmsToCharArrayNegativeMinutes);
    CHECK(testDmsToCharArrayPositiveMinutes);
    CHECK(testDmsToCharArrayNegativeSeconds);
    CHECK(testDmsToCharArrayPositiveSeconds);
    CHECK(testDmsToCharArrayZero);
    CHECK(test_matlab_dms);
    )

