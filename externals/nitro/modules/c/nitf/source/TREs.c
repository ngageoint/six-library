/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * © Copyright 2023, Maxar Technologies, Inc.
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

#undef NITF_PLUGIN_FUNCTION_EXPORT
#define NITF_PLUGIN_FUNCTION_EXPORT(retval_) static retval_

#include "nitf/TRE.h"
#include "nitf/PluginIdentifier.h"

#if _MSC_VER
#pragma warning(disable: 4464) // relative include path contains '..'
#endif

#include "../shared/ACCHZB.c"
#include "../shared/ACCPOB.c"
#include "../shared/ACFTA.c"
#include "../shared/AIMIDB.c"
#include "../shared/CSCRNA.c"
#include "../shared/CSEXRB.c"
#include "../shared/ENGRDA.c"
#include "../shared/HISTOA.c"
#include "../shared/JITCID.c"
#include "../shared/PTPRAA.c"
#include "../shared/RPFHDR.c"

#include "../shared/TEST_DES.c"
#include "../shared/XML_DATA_CONTENT.c"

 /******************************************************************************/
 /*
 *   Simple example DES for testing.

   This file defines a very simple Data Extension Segment. This example is
   used for test of the UserSegment object and can be used aa an example
   and starting point for the development of DES.

   This example defines a "standard" DE segment which means the user
   header can be implemented via a TRE object and there are no out of
   segment dependencies

   The DES ID will be TEST_PRELOADED_DES

   The user header will have three fields

     COUNT     - Number of data values
     START     - Start value in ramp
     INCREMENT - Increment between values in ramp

   The data is an 8-bit ramp defined by the three values. in testing, the
   ramp will be setup to contain printable values.
 */

 /*    TRE description for user header */
static nitf_TREDescription TEST_PRELOADED_DES_description[] = {
    {NITF_BCS_N, 2, "Number of data values", "COUNT" },
    {NITF_BCS_N, 3, "Start value in ramp", "START" },
    {NITF_BCS_N, 2, "Increment between values in ramp", "INCREMENT" },
    {NITF_END, 0, NULL, NULL}
};
static nitf_TREDescriptionInfo TEST_PRELOADED_DES_descriptions[] = {
    { "Preloaded DES (for testing)", TEST_PRELOADED_DES_description, NITF_TRE_DESC_NO_LENGTH },
    { "TEST_PRELOADED_DES", TEST_PRELOADED_DES_description, NITF_TRE_DESC_NO_LENGTH },
    { NULL, NULL, NITF_TRE_DESC_NO_LENGTH }
};
static nitf_TREDescriptionSet TEST_PRELOADED_DES_descriptionSet = { 0, TEST_PRELOADED_DES_descriptions };

static const char* TEST_PRELOADED_DES_ident[] =
{
    NITF_PLUGIN_TRE_KEY, "TEST_PRELOADED_DES", "Preloaded DES (for testing)", NULL
};

static nitf_TREHandler TEST_PRELOADED_DESHandler;
static const char** TEST_PRELOADED_DES_init(nitf_Error* error)
{
    if (!nitf_TREUtils_createBasicHandler(&TEST_PRELOADED_DES_descriptionSet,
        &TEST_PRELOADED_DESHandler, error))
        return NULL;
    return TEST_PRELOADED_DES_ident;
}
static nitf_TREHandler* TEST_PRELOADED_DES_handler(nitf_Error* error) {
    (void)error;
    return &TEST_PRELOADED_DESHandler;
}

/******************************************************************************/

#define NITF_preload_TRE_(Tre_, enabled_) { #Tre_, Tre_##_init, Tre_##_handler, enabled_ }
#define NITF_preload_TRE(Tre_) NITF_preload_TRE_(Tre_, NRT_FALSE /*enabled*/)

nitf_TREPreloaded defaultPreloadedTREs[] = {
    // Not preloading any TREs right now: with the existing system,
    // a TRE can be removed by deleting the DLL/SO.  If that same TRE
    // were preloaded, there would be no way to get rid of it.
    NITF_preload_TRE(ACCHZB),
    NITF_preload_TRE(ACCPOB),
    NITF_preload_TRE(ACFTA),
    NITF_preload_TRE(AIMIDB),
    NITF_preload_TRE(CSCRNA),
    NITF_preload_TRE(CSEXRB),
	NITF_preload_TRE(ENGRDA),
    NITF_preload_TRE(HISTOA),
    NITF_preload_TRE(JITCID),
    NITF_preload_TRE(PTPRAA),
    NITF_preload_TRE(RPFHDR),

    NITF_preload_TRE_(TEST_PRELOADED_DES,  NRT_TRUE /*enabled*/),

	{ NULL, NULL, NULL, NRT_FALSE } // end of list
};
