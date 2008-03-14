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
#include <check.h>
#include "check_all.h"

nitf_Error error;

START_TEST(test_field_set_BCSA)
{
    char buf[NITF_FHDR_SZ + 1];
    
    nitf_Field *field =
        nitf_Field_construct(NITF_FHDR_SZ, NITF_BCS_A, &error);

    if (!nitf_Field_setRawData(field, "NIT", 3, &error))
        fail("nitf_Field_setRawData error: %s", error.message);

    if (!nitf_Field_get(field, buf, NITF_CONV_STRING, NITF_FHDR_SZ + 1, &error))
        fail("nitf_Field_get error: %s", error.message);
    
    fail_if(strncmp("NIT", buf, 3) != 0, "Unexpected value");
}
END_TEST


START_TEST(test_field_set_BCSN)
{
    nitf_Int32 int32;
    
    nitf_Field *field =
        nitf_Field_construct(NITF_HL_SZ, NITF_BCS_N, &error);
    
    if (!nitf_Field_setInt32(field, 42, &error))
        fail("nitf_Field_setInt32 error: %s", error.message);

    if (!nitf_Field_get(field, &int32, NITF_CONV_INT, 4, &error))
        fail("nitf_Field_get error: %s", error.message);
    
    fail_if(int32 != 42, "Unexpected value: %d", int32);
}
END_TEST


Suite *make_field_suite(void)
{
    Suite *suite = suite_create(NITF_FILE);

    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_field_set_BCSA);
    tcase_add_test(tc_core, test_field_set_BCSN);
    suite_add_tcase(suite, tc_core);

    return suite;
}
