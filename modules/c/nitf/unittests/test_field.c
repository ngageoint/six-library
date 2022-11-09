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

#include <import/nitf.h>
#include "Test.h"

TEST_CASE( testField)
{
    char fhdr_str[NITF_FHDR_SZ + 1];
    char ubin_str[10];
    char hl_str[NITF_HL_SZ + 1];
    double doubleData;
    float floatData;
    nitf_Field *fhdr = NULL, *ubin = NULL, *hl = NULL, *realField = NULL;

    nitf_Error error;
    int32_t int32 = 16801;
    fhdr = nitf_Field_construct(NITF_FHDR_SZ, NITF_BCS_A, &error);
    ubin = nitf_Field_construct(4, NITF_BINARY, &error);
    hl = nitf_Field_construct(NITF_HL_SZ, NITF_BCS_N, &error);
    realField = nitf_Field_construct(NITF_HL_SZ, NITF_BCS_N, &error);

    TEST_ASSERT(fhdr != NULL);
    TEST_ASSERT(ubin != NULL);
    TEST_ASSERT(hl != NULL);
    TEST_ASSERT(realField != NULL);

    printf("%d\n", int32);
    nitf_Field_setRawData(fhdr, "NIT", 3, &error);
    nitf_Field_setRawData(ubin, &int32, 4, &error);
    nitf_Field_setRawData(hl, "000142", 6, &error);
    nitf_Field_setRawData(realField, "142.56", 6, &error);

    TEST_ASSERT(nitf_Field_get(fhdr, fhdr_str, NITF_CONV_STRING, NITF_FHDR_SZ
            + 1, &error));
    printf("FHDR: [%s]\n", fhdr_str);
    TEST_ASSERT_EQ_STR(fhdr_str, "NIT ");

    TEST_ASSERT(nitf_Field_get(hl, hl_str, NITF_CONV_STRING, NITF_HL_SZ + 1,
                               &error));
    printf("HL (str): [%s]\n", hl_str);
    TEST_ASSERT_EQ_STR(hl_str, "000142");

    TEST_ASSERT(nitf_Field_get(hl, &int32, NITF_CONV_INT, 4, &error));
    printf("HL: [%d]\n", int32);
    TEST_ASSERT_EQ_INT(int32, 142);

    TEST_ASSERT(nitf_Field_get(realField, hl_str, NITF_CONV_STRING, NITF_HL_SZ
            + 1, &error));
    printf("REAL (str): [%s]\n", hl_str);
    TEST_ASSERT_EQ_STR(hl_str, "142.56");

    TEST_ASSERT(nitf_Field_get(realField, &doubleData, NITF_CONV_REAL,
                               sizeof(double), &error));
    printf("REAL (double): [%f]\n", doubleData);
    TEST_ASSERT_EQ_FLOAT((float)doubleData, 142.56f);

    TEST_ASSERT(nitf_Field_get(realField, &floatData, NITF_CONV_REAL,
                               sizeof(float), &error));
    printf("REAL (float): [%f]\n", floatData);
    TEST_ASSERT_EQ_FLOAT(floatData, 142.56f);

    TEST_ASSERT(nitf_Field_get(ubin, &int32, NITF_CONV_INT, 4, &error));
    printf("UBIN: [%d]\n", int32);
    TEST_ASSERT_EQ_INT(int32, 16801);

    TEST_ASSERT(nitf_Field_get(ubin, ubin_str, NITF_CONV_STRING, 10, &error));
    printf("UBIN (str): [%s]\n", ubin_str);
    TEST_ASSERT_EQ_STR(ubin_str, "16801");

    nitf_Field_destruct(&fhdr);
    nitf_Field_destruct(&ubin);
    nitf_Field_destruct(&hl);
    nitf_Field_destruct(&realField);

    TEST_ASSERT_NULL(fhdr);
    TEST_ASSERT_NULL(ubin);
    TEST_ASSERT_NULL(hl);
    TEST_ASSERT_NULL(realField);
}

TEST_CASE(setReal)
{
    const size_t length = 14;
    nitf_Error errorObj;  /* Error object */
    nitf_Error* error = &errorObj;    /* Pointer to error object */

    /* Note:

        NITF real fields are usually NITF_BCS_A because of the decimal place
        or exponent
     */
    nitf_Field* field = nitf_Field_construct(length, NITF_BCS_A, error);
    TEST_ASSERT(field != NULL);

    #define BUF_SIZE 256
    char buffer[BUF_SIZE];

    /*  Test fields */
    NITF_BOOL ret = nitf_Field_setReal(field, "f", 1, 12.3456, error);
    TEST_ASSERT(ret);
    nitf_Field_snprint(buffer, BUF_SIZE, field);
    TEST_ASSERT_EQ_STR(buffer, "+12.3456000000");

    ret = nitf_Field_setReal(field, "e", 1, 12.3456, error);
    TEST_ASSERT(ret);
    nitf_Field_snprint(buffer, BUF_SIZE, field);
    TEST_ASSERT_EQ_STR(buffer, "+1.2345600e+01");

    ret = nitf_Field_setReal(field, "E", 1, 12.3456, error);
    TEST_ASSERT(ret);
    nitf_Field_snprint(buffer, BUF_SIZE, field);
    TEST_ASSERT_EQ_STR(buffer, "+1.2345600E+01");


    ret = nitf_Field_setReal(field, "f", 0, 12.3456, error);
    TEST_ASSERT(ret);
    nitf_Field_snprint(buffer, BUF_SIZE, field);
    TEST_ASSERT_EQ_STR(buffer, "12.34560000000");

    ret = nitf_Field_setReal(field, "e", 0, 12.3456, error);
    TEST_ASSERT(ret);
    nitf_Field_snprint(buffer, BUF_SIZE, field);
    TEST_ASSERT_EQ_STR(buffer, "1.23456000e+01");

    ret = nitf_Field_setReal(field, "E", 0, 12.3456, error);
    TEST_ASSERT(ret);
    nitf_Field_snprint(buffer, BUF_SIZE, field);
    TEST_ASSERT_EQ_STR(buffer, "1.23456000E+01");


    field = nitf_Field_construct(8, NITF_BCS_A, error);
    TEST_ASSERT(field != NULL);
    ret = nitf_Field_setReal(field, "f", 1, 12.3456, error);
    TEST_ASSERT(ret);
    nitf_Field_snprint(buffer, BUF_SIZE, field);
    TEST_ASSERT_EQ_STR(buffer, "+12.3456");
    ret = nitf_Field_setReal(field, "f", 0, 12.3456, error);
    TEST_ASSERT(ret);
    nitf_Field_snprint(buffer, BUF_SIZE, field);
    TEST_ASSERT_EQ_STR(buffer, "12.34560");

}

TEST_MAIN(
    (void)argc;
    (void)argv;
    CHECK(testField);
    CHECK(setReal);
    )