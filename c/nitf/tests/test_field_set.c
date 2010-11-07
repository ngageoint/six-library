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

/*
    Test program for testing field set functions

  This program reads a sets some fields in an image subheader


*/

#include <import/nitf.h>

#define STR_LEN 100             /* Test string max length */

int main(int argc, char *argv[])
{
    nitf_FileHeader *fhdr;      /* File header supplying fields */
    nitf_ImageSubheader *subhdr;        /* Subheader supplying fields */
    nitf_Uint32 valueU32Before; /* Value buffer */
    nitf_Uint32 valueU32After;  /* Value buffer */
    nitf_Uint64 valueU64Before; /* Value buffer */
    nitf_Uint64 valueU64After;  /* Value buffer */
    char *valueStrBefore;       /* Value buffer */
    /* Value buffer */
    static char valueStrAfter[STR_LEN + 2];
    nitf_Uint32 valueStrLen;    /* Value buffer */
    static nitf_Error errorObj; /* Error object for messages */
    nitf_Error *error;          /* Pointer to the error object */

    error = &errorObj;

    fhdr = nitf_FileHeader_construct(error);
    if (subhdr == NULL)
    {
        nitf_Error_print(error, stdout, "Error creating image subheader");
        exit(EXIT_FAILURE);
    }

    subhdr = nitf_ImageSubheader_construct(error);
    if (subhdr == NULL)
    {
        nitf_Error_print(error, stdout, "Error creating image subheader");
        exit(EXIT_FAILURE);
    }

    subhdr = nitf_ImageSubheader_construct(error);
    if (subhdr == NULL)
    {
        nitf_Error_print(error, stdout, "Error creating image subheader");
        exit(EXIT_FAILURE);
    }

    /*    Set some fields (should work) */

    valueU32Before = 12345;
    if (nitf_Field_setUint32(subhdr->NITF_XBANDS, valueU32Before, error))
        fprintf(stdout,
                "Set of XBANDS via nitf_Field_setUint32 worked as expected\n");
    else
        nitf_Error_print(error, stdout,
                         "Unexpected error setting XBANDS via nitf_Field_setUint32");

    nitf_Field_get(subhdr->NITF_XBANDS, (NITF_DATA *) & valueU32After,
                   NITF_CONV_UINT, NITF_INT32_SZ, error);
    fprintf(stdout,
            "Set of XBANDS via nitf_Field_setUint32 original %d readback %d\n",
            valueU32Before, valueU32After);

    valueU32Before = 1234;
    if (nitf_Field_setUint32(subhdr->NITF_COMRAT, valueU32Before, error))
        fprintf(stdout,
                "Set of COMRAT via nitf_Field_setUint32 worked as expected\n");
    else
        nitf_Error_print(error, stdout,
                         "Unexpected error setting COMRAT via nitf_Field_setUint32");

    nitf_Field_get(subhdr->NITF_COMRAT, (NITF_DATA *) & valueU32After,
                   NITF_CONV_UINT, NITF_INT32_SZ, error);
    fprintf(stdout,
            "Set of COMRAT via nitf_Field_setUint32 original %d readback %d\n",
            valueU32Before, valueU32After);

    valueU64Before = 11234567890ll;
    if (nitf_Field_setUint64(fhdr->NITF_FL, valueU64Before, error))
        fprintf(stdout,
                "Set of FL via nitf_Field_setUint64 worked as expected\n");
    else
        nitf_Error_print(error, stdout,
                         "Unexpected error setting FL via nitf_Field_setUint64");

    nitf_Field_get(fhdr->NITF_FL, (NITF_DATA *) & valueU64After,
                   NITF_CONV_UINT, NITF_INT64_SZ, error);
    fprintf(stdout,
            "Set of FL via nitf_Field_setUint64 original %lu readback %lu\n",
            valueU64Before, valueU64After);

    valueStrBefore = "TestStr";
    if (nitf_Field_setString(subhdr->NITF_IID2, valueStrBefore, error))
        fprintf(stdout,
                "Set of IID2 via nitf_Field_setString worked as expected\n");
    else
        nitf_Error_print(error, stdout,
                         "Unexpected error setting IID2 via nitf_Field_setString");

    nitf_Field_get(subhdr->NITF_IID2, (NITF_DATA *) valueStrAfter,
                   NITF_CONV_STRING, STR_LEN, error);
    fprintf(stdout,
            "Set of IID2 via nitf_Field_setString original %s readback %s\n",
            valueStrBefore, valueStrAfter);

    valueStrBefore = "1234";
    if (nitf_Field_setString(subhdr->NITF_NCOLS, valueStrBefore, error))
        fprintf(stdout,
                "Set of NCOLS via nitf_Field_setString worked as expected\n");
    else
        nitf_Error_print(error, stdout,
                         "Unexpected error setting NCOLS via nitf_Field_setString");

    nitf_Field_get(subhdr->NITF_NCOLS, (NITF_DATA *) valueStrAfter,
                   NITF_CONV_STRING, STR_LEN, error);
    fprintf(stdout,
            "Set of NCOLS via nitf_Field_setString original %s readback %s\n",
            valueStrBefore, valueStrAfter);

    /*    Set some fields (should fail) */

    valueU32Before = 123;
    if (nitf_Field_setUint32(fhdr->NITF_FBKGC, valueU32Before, error))
        fprintf(stdout,
                "Set of FBKGC via nitf_Field_setUnit32 worked expected error\n");
    else
        nitf_Error_print(error, stdout,
                         "Expected error setting FBKGC via nitf_Field_setUint32");

    valueStrBefore = "123";
    if (nitf_Field_setString(fhdr->NITF_FBKGC, valueStrBefore, error))
        fprintf(stdout,
                "Set of FBKGC via nitf_Field_setString worked expected error\n");
    else
        nitf_Error_print(error, stdout,
                         "Expected error setting FBKGC via nitf_Field_setString");

    valueStrBefore = "12g";
    if (nitf_Field_setString(subhdr->NITF_NCOLS, valueStrBefore, error))
        fprintf(stdout,
                "Set of NCOLS via nitf_Field_setString worked expected error\n");
    else
        nitf_Error_print(error, stdout,
                         "Expected error setting NCOLS via nitf_Field_setString");

    valueU32Before = 3;
    if (nitf_Field_setUint32(subhdr->NITF_NBANDS, valueU32Before, error))
        fprintf(stdout,
                "Set of NBANDS via nitf_Field_setUint32 worked expected error\n");
    else
        nitf_Error_print(error, stdout,
                         "Expected error setting NBANDS via nitf_Field_setUint32");

    return 0;

}
