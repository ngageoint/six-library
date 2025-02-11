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

#include <import/nitf.h>
#include "Test.h"

TEST_CASE(test_geo_utils)
{
    const char* DECIMAL_LAT_STR = "-12.345";
    const char* DECIMAL_LON_STR = "+123.456";

    const char* DMS_LAT_STR = "123456S";
    const char* DMS_LON_STR = "1234556E";

    /* Try geographic stuff */
    char ll[10];
    int d, m;
    double s;
    double decimal;
    nitf_Error e;
    if (!nitf_Utils_parseGeographicString(DMS_LAT_STR, &d, &m, &s, &e))
    {
        nitf_Error_print(&e, stdout, "Exiting...");
        TEST_ASSERT(0);
    }
    decimal = nitf_Utils_geographicToDecimal(d, m, s);
    TEST_ASSERT_EQ_INT(d, -12);
    TEST_ASSERT_EQ_INT(m, 34);
    TEST_ASSERT_EQ_FLOAT(s, 56.0);
    nitf_Utils_geographicLatToCharArray(d, m, s, ll);
    TEST_ASSERT_EQ_STR(ll, DMS_LAT_STR);

    TEST_ASSERT((decimal <= -12.582) && (decimal >= -12.583));
    nitf_Utils_decimalLatToGeoCharArray(decimal, ll);
    TEST_ASSERT_EQ_STR(ll, DMS_LAT_STR);

    /* Now convert it back */
    nitf_Utils_decimalToGeographic(decimal, &d, &m, &s);
    TEST_ASSERT_EQ_INT(d, -12);
    TEST_ASSERT_EQ_INT(m, 34);
    TEST_ASSERT_EQ_FLOAT(s, 56.0);


    if (!nitf_Utils_parseGeographicString(DMS_LON_STR, &d, &m, &s, &e))
    {
        nitf_Error_print(&e, stdout, "Exiting...");
        TEST_ASSERT(0);
    }
    decimal = nitf_Utils_geographicToDecimal(d, m, s);
    TEST_ASSERT_EQ_INT(d, 123);
    TEST_ASSERT_EQ_INT(m, 45);
    TEST_ASSERT_EQ_FLOAT(s, 56.0);
    TEST_ASSERT((decimal >= 123.765) && (decimal <= 123.766));

    s = 67.0;
    nitf_Utils_geographicLonToCharArray(d, m, s, ll);
    TEST_ASSERT_EQ_STR(ll, "9999999E");

    nitf_Utils_decimalLonToGeoCharArray(decimal, ll);
    TEST_ASSERT_EQ_STR(ll, "1234556E");

    /* Now convert it back */
    nitf_Utils_decimalToGeographic(decimal, &d, &m, &s);
    TEST_ASSERT_EQ_INT(d, 123);
    TEST_ASSERT_EQ_INT(m, 45);
    TEST_ASSERT_EQ_FLOAT(s, 56.0);

    /* Try decimal stuff */
    if (!nitf_Utils_parseDecimalString(DECIMAL_LAT_STR, &decimal, &e))
    {
        nitf_Error_print(&e, stdout, "Exiting...");
        TEST_ASSERT(0);
    }
    nitf_Utils_decimalLatToCharArray(decimal, ll);
    TEST_ASSERT_EQ_STR(ll, "-12.345");

    /* Try decimal stuff */
    if (!nitf_Utils_parseDecimalString(DECIMAL_LON_STR, &decimal, &e))
    {
        nitf_Error_print(&e, stdout, "Exiting...");
        TEST_ASSERT(0);
    }
    nitf_Utils_decimalLonToCharArray(decimal, ll);
    TEST_ASSERT_EQ_STR(ll, "+123.456");
}

TEST_CASE(test_decimalToGeographic_normal)
{
    /*
    These are some examples run through nrt_Utils_decimalToGeographic and I agree with all of them:    
    */
    int degrees, minutes; double seconds;

    // 0.000000 -> 0 0 0.000000
    nrt_Utils_decimalToGeographic(0.0, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(0, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);

    // -84.500000 -> -84 30 0.000000
    nrt_Utils_decimalToGeographic(-84.500000, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(-84, degrees);
    TEST_ASSERT_EQ_INT(30, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);

    //  -0.100000 -> 0 -6 0.000000
    nrt_Utils_decimalToGeographic(-0.1, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(0, degrees);
    TEST_ASSERT_EQ_INT(-6, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);

    // -0.010000 -> 0 0 -36.000000
    nrt_Utils_decimalToGeographic(-0.01, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(0, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(-36.0, seconds);

    // -0.001000 -> 0 0 -3.600000
    nrt_Utils_decimalToGeographic(-0.001, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(0, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(-3.6, seconds);

    // 180.000000 -> 180 0 0.000000
    nrt_Utils_decimalToGeographic(180.0, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(180, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);

    // -180.000000 -> -180 0 0.000000
    nrt_Utils_decimalToGeographic(-180.0, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(-180, degrees);
    TEST_ASSERT_EQ_INT(0, minutes);
    TEST_ASSERT_EQ_FLOAT(0.0, seconds);

    // 179.950000 -> 179 56 60.000000
    nrt_Utils_decimalToGeographic(179.95, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(179, degrees);
    TEST_ASSERT_EQ_INT(56, minutes);
    TEST_ASSERT_EQ_FLOAT(60.0, seconds);

    // -179.950000 -> -179 56 60.000000
    nrt_Utils_decimalToGeographic(-179.95, &degrees, &minutes, &seconds);
    TEST_ASSERT_EQ_INT(-179, degrees);
    TEST_ASSERT_EQ_INT(56, minutes);
    TEST_ASSERT_EQ_FLOAT(60.0, seconds);
}

TEST_MAIN(
    (void)argc;
    (void)argv;
    CHECK(test_geo_utils);
    CHECK(test_decimalToGeographic_normal);
    )

