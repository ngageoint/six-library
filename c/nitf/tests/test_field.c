/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

int main()
{
    char *fhdr_str[NITF_FHDR_SZ + 1];
    char ubin_str[10];
    char hl_str[NITF_HL_SZ + 1];
    double doubleData;
    float floatData;

    nitf_Error error;
    nitf_Int32 int32 = 16801;
    nitf_Field *fhdr =
        nitf_Field_construct(NITF_FHDR_SZ, NITF_BCS_A, &error);
    nitf_Field *ubin = nitf_Field_construct(4, NITF_BINARY, &error);
    nitf_Field *hl = nitf_Field_construct(NITF_HL_SZ, NITF_BCS_N, &error);
    nitf_Field *realField =
        nitf_Field_construct(NITF_HL_SZ, NITF_BCS_N, &error);

    printf("%d\n", int32);
    nitf_Field_setRawData(fhdr, "NIT", 3, &error);
    nitf_Field_setRawData(ubin, &int32, 4, &error);
    nitf_Field_setRawData(hl, "000142", 6, &error);
    nitf_Field_setRawData(realField, "142.56", 6, &error);

    if (!nitf_Field_get
            (fhdr, fhdr_str, NITF_CONV_STRING, NITF_FHDR_SZ + 1, &error))
    {
        nitf_Error_print(&error, stdout, "Blah");
        exit(EXIT_FAILURE);
    }
    printf("FHDR: [%s]\n", *fhdr_str);

    if (!nitf_Field_get
            (hl, hl_str, NITF_CONV_STRING, NITF_HL_SZ + 1, &error))
    {
        nitf_Error_print(&error, stdout, "Blah");
        exit(EXIT_FAILURE);
    }
    printf("HL (str): [%s]\n", hl_str);
    if (!nitf_Field_get(hl, &int32, NITF_CONV_INT, 4, &error))
    {
        nitf_Error_print(&error, stdout, "Blah");
        exit(EXIT_FAILURE);
    }
    printf("HL: [%d]\n", int32);

    if (!nitf_Field_get
            (realField, hl_str, NITF_CONV_STRING, NITF_HL_SZ + 1, &error))
    {
        nitf_Error_print(&error, stdout, "Blah");
        exit(EXIT_FAILURE);
    }
    printf("REAL (str): [%s]\n", hl_str);
    if (!nitf_Field_get
            (realField, &doubleData, NITF_CONV_REAL, sizeof(double), &error))
    {
        nitf_Error_print(&error, stdout, "Blah");
        exit(EXIT_FAILURE);
    }
    printf("REAL (double): [%f]\n", doubleData);


    if (!nitf_Field_get
            (realField, &floatData, NITF_CONV_REAL, sizeof(float), &error))
    {
        nitf_Error_print(&error, stdout, "Blah");
        exit(EXIT_FAILURE);
    }
    printf("REAL (float): [%f]\n", floatData);


    if (!nitf_Field_get(ubin, &int32, NITF_CONV_INT, 4, &error))
    {
        nitf_Error_print(&error, stdout, "Blah");
        exit(EXIT_FAILURE);
    }
    printf("UBIN: [%d]\n", int32);

    if (!nitf_Field_get(ubin, ubin_str, NITF_CONV_STRING, 10, &error))
    {
        nitf_Error_print(&error, stdout, "Blah");
        exit(EXIT_FAILURE);
    }
    printf("UBIN (str): [%s]\n", ubin_str);

    return 0;
}
