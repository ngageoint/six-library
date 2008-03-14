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


int main (int argc, char **argv)
{
    char logFile[NITF_MAX_PATH];
    int numFailed;
    
    /* create the runner */
    SRunner *runner = srunner_create(make_list_suite());
    srunner_add_suite(runner, make_field_suite());
    
    /* let's log to XML */
    strcpy(logFile, NITF_FILE);
    strcat(logFile, ".test.xml");
    srunner_set_xml(runner, logFile);
    
    srunner_run_all(runner, CK_NORMAL);
    numFailed = srunner_ntests_failed(runner);
    srunner_free(runner);
    return (numFailed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
