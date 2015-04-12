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
    Test program for testing set real field

    The optional argument is the length of the field
*/

#include <import/nitf.h>

void testField(nitf_Field *field, char *type, NITF_BOOL plus,
               nitf_Uint32 length, double value);

int main(int argc, char *argv[])
{
    nitf_Field *field;    /* Field for test */
    size_t length;        /* Field length */
    nitf_Error errorObj;  /* Error object */
    nitf_Error *error;    /* Pointer to error object */

    error = &errorObj;

    /* Note:

        NITF real fields are usually NITF_BCS_A because of the decimal place
        or exponent
     */

    if (argc > 1)
        length = atol(argv[1]);
    else
        length = 14;

    field = nitf_Field_construct(length, NITF_BCS_A, error);
    if (field == NULL)
    {
        nitf_Error_print(error, stderr, "Could not create field\n");
        return(1);
    }

    /*  Test fields */

    testField(field, "f", 1, length, 12.3456);
    testField(field, "e", 1, length, 12.3456);
    testField(field, "E", 1, length, 12.3456);

    testField(field, "f", 0, length, 12.3456);
    testField(field, "e", 0, length, 12.3456);
    testField(field, "E", 0, length, 12.3456);

    return(0);
}

void testField(nitf_Field *field, char *type, NITF_BOOL plus,
               nitf_Uint32 length, double value)
{
    NITF_BOOL ret;        /* Return from function */
    nitf_Error errorObj;  /* Error object */
    nitf_Error *error;    /* Pointer to error object */
    nitf_Uint32 i;

    error = &errorObj;

    ret = nitf_Field_setReal(field, type, plus, value, error);
    if (!ret)
    {
        nitf_Error_print(error, stderr, "Error setting field\n");
        return;
    }


    if (plus)
        printf("Test of type %s with sign length = %d value = %lf\n",
               type, length, value);
    else
        printf("Test of type %s without sign length = %d value = %lf\n",
               type, length, value);
    printf("||");
    for (i = 0;i < length;i++) printf("=");
    printf("||\n||");
    nitf_Field_print(field);
    printf("||\n");
    printf("||");
    for (i = 0;i < length;i++) printf("=");
    printf("||\n");

    return;
}
