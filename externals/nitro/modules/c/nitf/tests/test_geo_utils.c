/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2016, MDA Information Systems LLC
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


#include <import/nitf.h>

#define DECIMAL_LAT_STR "-12.345"
#define DECIMAL_LON_STR "+123.456"

#define DMS_LAT_STR "123456S"
#define DMS_LON_STR "1234567E"

int main(int argc, char**argv)
{

    /* Try geographic stuff */
    char ll[10];
    int d, m;
    double s;
    double decimal;
    nitf_Error e;
    if (!nitf_Utils_parseGeographicString(DMS_LAT_STR, &d, &m, &s, &e))
    {
        nitf_Error_print(&e, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }
    decimal = nitf_Utils_geographicToDecimal(d, m, s);
    printf("DMS: [%s]\n", DMS_LAT_STR);
    printf("\tSeparated: %d %d %f\n", d, m, s);
    nitf_Utils_geographicLatToCharArray(d, m, s, ll);
    printf("\tRe-formatted: [%s]\n", ll);

    printf("\tAs decimal: %f\n", decimal);
    nitf_Utils_decimalLatToGeoCharArray(decimal, ll);
    printf("\tRe-formatted (as decimal): [%s]\n", ll);

    /* Now convert it back */
    nitf_Utils_decimalToGeographic(decimal, &d, &m, &s);
    printf("\tRound trip: %d %d %f\n", d, m, s);




    if (!nitf_Utils_parseGeographicString(DMS_LON_STR, &d, &m, &s, &e))
    {
        nitf_Error_print(&e, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }
    decimal = nitf_Utils_geographicToDecimal(d, m, s);
    printf("DMS: [%s]\n", DMS_LON_STR);
    printf("\tSeparated: %d %d %f\n", d, m, s);
    nitf_Utils_geographicLonToCharArray(d, m, s, ll);
    printf("\tRe-formatted: [%s]\n", ll);
    printf("\tAs decimal: %f\n", decimal);
    nitf_Utils_decimalLonToGeoCharArray(decimal, ll);
    printf("\tRe-formatted (as decimal): [%s]\n", ll);

    /* Now convert it back */
    nitf_Utils_decimalToGeographic(decimal, &d, &m, &s);
    printf("\tRound trip: %d %d %f\n", d, m, s);



    /* Try decimal stuff */
    if (!nitf_Utils_parseDecimalString(DECIMAL_LAT_STR, &decimal, &e))
    {
        nitf_Error_print(&e, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }
    printf("Decimal: [%s]\n", DECIMAL_LAT_STR);
    nitf_Utils_decimalLatToCharArray(decimal, ll);
    printf("\tRe-formatted: [%s]\n", ll);

    /* Try decimal stuff */
    if (!nitf_Utils_parseDecimalString(DECIMAL_LON_STR, &decimal, &e))
    {
        nitf_Error_print(&e, stdout, "Exiting...");
        exit(EXIT_FAILURE);
    }
    printf("Decimal: [%s]\n", DECIMAL_LON_STR);
    nitf_Utils_decimalLonToCharArray(decimal, ll);
    printf("\tRe-formatted: [%s]\n", ll);
    return 0;
}

