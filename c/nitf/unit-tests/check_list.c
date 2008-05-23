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

START_TEST(test_list_create)
{
    nitf_List *list = nitf_List_construct(&error);
    fail_if(list == NULL, "Unable to construct.");
    nitf_List_destruct(&list);
}
END_TEST


Suite *make_list_suite(void)
{
    Suite *suite = suite_create(NITF_FILE);

    TCase *tc_core = tcase_create("Core");
    tcase_add_test(tc_core, test_list_create);
    suite_add_tcase(suite, tc_core);

    return suite;
}
