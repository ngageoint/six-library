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
*   Simple example DES for testing.

  This file defines a very simple Data Extension Segment. This example is
  used for test of the UserSegment object and can be used aa an example
  and starting point for the development of DES.

  This example defines a "standard" DE segment which means the user 
  header can be implemented via a TRE object and there are no out of
  segment dependencies

  The DES ID will be TEST_DES

  The user header will have three fields

    NITF_TEST_DES_COUNT     - Number of data values
    NITF_TEST_DES_START     - Start value in ramp
    NITF_TEST_DES_INCREMENT - Increment between values in ramp

  The data is an 8-bit ramp defined by the three values. in testing, the
  ramp will be setup to contain printable values.

  This example includes a plug-in interface
*/

#include <import/nitf.h>

/*    TRE access helper (-1 is returned on error) */


/*    TRE description for user header */

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_N, 2, "Number of data values", "TEST_DES_COUNT" },
    {NITF_BCS_N, 3, "Start value in ramp", "TEST_DES_START" },
    {NITF_BCS_N, 2, "Increment between values in ramp", "TEST_DES_INCREMENT" },
    {NITF_END, 0, NULL, NULL}
};


static char *ident[] = 
{
    NITF_PLUGIN_TRE_KEY,
    "TEST DES",
    "TEST_DES",
    NULL
};

static nitf_TREDescriptionInfo descriptions[] = {
    { "TEST DES", description, NITF_TRE_DESC_NO_LENGTH },
    { "TEST_DES", description, NITF_TRE_DESC_NO_LENGTH },
    { NULL, NULL, NITF_TRE_DESC_NO_LENGTH }
};

static nitf_TREDescriptionSet descriptionSet = { 0, descriptions };
static nitf_TREHandler TEST_DESHandler;

NITFAPI(char**) TEST_DES_init(nitf_Error* error)
{
    if (!nitf_TREUtils_createBasicHandler(&descriptionSet,
                                          &TEST_DESHandler,error))
        return NULL; 
    return ident;
}
NITFAPI(void) TEST_DES_cleanup(void){}
NITFAPI(nitf_TREHandler*) TEST_DES_handler(nitf_Error* error) {
    return &TEST_DESHandler; 
}

NITF_CXX_ENDGUARD
