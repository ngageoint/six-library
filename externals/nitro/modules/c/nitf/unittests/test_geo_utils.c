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
    const char* DMS_LON_STR = "1234567E";

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
    TEST_ASSERT_EQ_STR(ll, "112504S");

    TEST_ASSERT((decimal <= -12.582) && (decimal >= -12.583));
    nitf_Utils_decimalLatToGeoCharArray(decimal, ll);
    TEST_ASSERT_EQ_STR(ll, "112504S");

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
    TEST_ASSERT_EQ_FLOAT(s, 67.0);
    nitf_Utils_geographicLonToCharArray(d, m, s, ll);
    TEST_ASSERT_EQ_STR(ll, "1234607E");
    TEST_ASSERT((decimal >= 123.768) && (decimal <= 123.769));

    nitf_Utils_decimalLonToGeoCharArray(decimal, ll);
    TEST_ASSERT_EQ_STR(ll, "1234607E");

    /* Now convert it back */
    nitf_Utils_decimalToGeographic(decimal, &d, &m, &s);
    TEST_ASSERT_EQ_INT(d, 123);
    TEST_ASSERT_EQ_INT(m, 46);
    TEST_ASSERT_EQ_FLOAT(s, 7.0);

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

TEST_MAIN(
    (void)argc;
    (void)argv;
    CHECK(test_geo_utils);
    )

