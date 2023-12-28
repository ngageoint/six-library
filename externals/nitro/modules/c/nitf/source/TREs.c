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
#include "../shared/ACCVTB.c"
#include "../shared/ACFTA.c"
#include "../shared/ACFTB.c"
#include "../shared/AIMIDA.c"
#include "../shared/AIMIDB.c"
#include "../shared/AIPBCA.c"
#include "../shared/ASTORA.c"
#include "../shared/BANDSA.c"
#include "../shared/BANDSB.c"
#include "../shared/BCKGDA.c"
#include "../shared/BLOCKA.c"
#include "../shared/BNDPLB.c"
#include "../shared/CCINFA.c"
#include "../shared/CLCTNA.c"
#include "../shared/CLCTNB.c"
#include "../shared/CMETAA.c"
#include "../shared/CSCCGA.c"
#include "../shared/CSCRNA.c"
#include "../shared/CSDIDA.c"
#include "../shared/CSEPHA.c"
#include "../shared/CSEXRA.c"
#include "../shared/CSEXRB.c"
#include "../shared/CSPROA.c"
#include "../shared/CSSFAA.c"
#include "../shared/CSSHPA.c"
#include "../shared/ENGRDA.c"
#include "../shared/EXOPTA.c"
#include "../shared/EXPLTA.c"
#include "../shared/EXPLTB.c"
#include "../shared/GEOLOB.c"
#include "../shared/GEOPSB.c"
#include "../shared/GRDPSB.c"
#include "../shared/HISTOA.c"
#include "../shared/ICHIPB.c"
#include "../shared/IMASDA.c"
#include "../shared/IMGDTA.c"
#include "../shared/IMRFCA.c"
#include "../shared/IOMAPA.c"
#include "../shared/J2KLRA.c"
#include "../shared/JITCID.c"
#include "../shared/MAPLOB.c"
#include "../shared/MATESA.c"
#include "../shared/MENSRA.c"
#include "../shared/MENSRB.c"
#include "../shared/MPDSRA.c"
#include "../shared/MSDIRA.c"
#include "../shared/MSTGTA.c"
#include "../shared/MTIRPA.c"
#include "../shared/MTIRPB.c"
#include "../shared/NBLOCA.c"
#include "../shared/OBJCTA.c"
#include "../shared/OFFSET.c"
#include "../shared/PATCHA.c"
#include "../shared/PATCHB.c"
#include "../shared/PIAEQA.c"
#include "../shared/PIAEVA.c"
#include "../shared/PIAIMB.c"
#include "../shared/PIAIMC.c"
#include "../shared/PIAPEA.c"
#include "../shared/PIAPEB.c"
#include "../shared/PIAPRC.c"
#include "../shared/PIAPRD.c"
#include "../shared/PIATGA.c"
#include "../shared/PIATGB.c"
#include "../shared/PIXQLA.c"
#include "../shared/PLTFMA.c"
#include "../shared/PRADAA.c"
#include "../shared/PRJPSB.c"
#include "../shared/PTPRAA.c"
#include "../shared/REGPTB.c"
#include "../shared/RPC00B.c"
#include "../shared/RPFDES.c"
#include "../shared/RPFHDR.c"
#include "../shared/RPFIMG.c"
#include "../shared/RSMAPA.c"
#include "../shared/RSMDCA.c"
#include "../shared/RSMECA.c"
#include "../shared/RSMGGA.c"
#include "../shared/RSMGIA.c"
#include "../shared/RSMIDA.c"
#include "../shared/RSMPCA.c"
#include "../shared/RSMPIA.c"
#include "../shared/SECTGA.c"
#include "../shared/SENSRA.c"
#include "../shared/SENSRB.c"
#include "../shared/SNSPSB.c"
#include "../shared/SNSRA.c"
#include "../shared/SOURCB.c"
#include "../shared/STDIDC.c"
#include "../shared/STEROB.c"
#include "../shared/STREOB.c"
#include "../shared/TRGTA.c"
#include "../shared/USE00A.c"

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
    {NITF_BCS_N, 2, "Number of data values",  "COUNT" },
    {NITF_BCS_N, 3, "Start value in ramp",  "START" },
    {NITF_BCS_N, 2, "Increment between values in ramp",  "INCREMENT" },
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
#define NITF_preload_TRE(Tre_) NITF_preload_TRE_(Tre_, -1 /*not set*/)

nitf_TREPreloaded defaultPreloadedTREs[] = {
    // Not preloading any TREs right now: with the existing system,
    // a TRE can be removed by deleting the DLL/SO.  If that same TRE
    // were preloaded, there would be no way to get rid of it.
    NITF_preload_TRE(ACCHZB), 
    NITF_preload_TRE(ACCPOB), 
    NITF_preload_TRE(ACCVTB), 
    NITF_preload_TRE(ACFTA), 
    NITF_preload_TRE(ACFTB), 
    NITF_preload_TRE(AIMIDA), 
    NITF_preload_TRE(AIMIDB), 
    NITF_preload_TRE(AIPBCA), 
    NITF_preload_TRE(ASTORA), 
    NITF_preload_TRE(BANDSA), 
    NITF_preload_TRE(BANDSB), 
    NITF_preload_TRE(BCKGDA), 
    NITF_preload_TRE(BLOCKA), 
    NITF_preload_TRE(BNDPLB), 
    NITF_preload_TRE(CCINFA), 
    NITF_preload_TRE(CLCTNA),
    NITF_preload_TRE(CLCTNB), 
    NITF_preload_TRE(CMETAA), 
    NITF_preload_TRE(CSCCGA), 
    NITF_preload_TRE(CSCRNA), 
    NITF_preload_TRE(CSDIDA), 
    NITF_preload_TRE(CSEPHA), 
    NITF_preload_TRE(CSEXRA), 
    NITF_preload_TRE(CSEXRB), 
    NITF_preload_TRE(CSPROA), 
    NITF_preload_TRE(CSSFAA), 
    NITF_preload_TRE(CSSHPA), 
    NITF_preload_TRE(ENGRDA), 
    NITF_preload_TRE(EXOPTA), 
    NITF_preload_TRE(EXPLTA), 
    NITF_preload_TRE(EXPLTB), 
    NITF_preload_TRE(GEOLOB), 
    NITF_preload_TRE(GEOPSB), 
    NITF_preload_TRE(GRDPSB), 
    NITF_preload_TRE(HISTOA), 
    NITF_preload_TRE(ICHIPB), 
    NITF_preload_TRE(IMASDA), 
    NITF_preload_TRE(IMGDTA), 
    NITF_preload_TRE(IMRFCA), 
    NITF_preload_TRE(IOMAPA), 
    NITF_preload_TRE(J2KLRA), 
    NITF_preload_TRE(JITCID), 
    NITF_preload_TRE(MAPLOB), 
    NITF_preload_TRE(MATESA), 
    NITF_preload_TRE(MENSRA), 
    NITF_preload_TRE(MENSRB), 
    NITF_preload_TRE(MPDSRA), 
    NITF_preload_TRE(MSDIRA), 
    NITF_preload_TRE(MSTGTA), 
    NITF_preload_TRE(MTIRPA), 
    NITF_preload_TRE(MTIRPB), 
    NITF_preload_TRE(NBLOCA), 
    NITF_preload_TRE(OBJCTA), 
    NITF_preload_TRE(OFFSET), 
    NITF_preload_TRE(PATCHA), 
    NITF_preload_TRE(PATCHB), 
    NITF_preload_TRE(PIAEQA), 
    NITF_preload_TRE(PIAEVA), 
    NITF_preload_TRE(PIAIMB), 
    NITF_preload_TRE(PIAIMC), 
    NITF_preload_TRE(PIAPEA), 
    NITF_preload_TRE(PIAPEB), 
    NITF_preload_TRE(PIAPRC), 
    NITF_preload_TRE(PIAPRD), 
    NITF_preload_TRE(PIATGA), 
    NITF_preload_TRE(PIATGB), 
    NITF_preload_TRE(PIXQLA), 
    NITF_preload_TRE(PLTFMA), 
    NITF_preload_TRE(PRADAA), 
    NITF_preload_TRE(PRJPSB), 
    NITF_preload_TRE(PTPRAA), 
    NITF_preload_TRE(REGPTB), 
    NITF_preload_TRE(RPC00B), 
    NITF_preload_TRE(RPFDES), 
    NITF_preload_TRE(RPFHDR), 
    NITF_preload_TRE(RPFIMG), 
    NITF_preload_TRE(RSMAPA), 
    NITF_preload_TRE(RSMDCA), 
    NITF_preload_TRE(RSMECA), 
    NITF_preload_TRE(RSMGGA), 
    NITF_preload_TRE(RSMGIA), 
    NITF_preload_TRE(RSMIDA), 
    NITF_preload_TRE(RSMPCA), 
    NITF_preload_TRE(RSMPIA), 
    NITF_preload_TRE(SECTGA), 
    NITF_preload_TRE(SENSRA), 
    NITF_preload_TRE(SENSRB), 
    NITF_preload_TRE(SNSPSB), 
    NITF_preload_TRE(SNSRA), 
    NITF_preload_TRE(SOURCB), 
    NITF_preload_TRE(STDIDC), 
    NITF_preload_TRE(STEROB), 
    NITF_preload_TRE(STREOB), 
    NITF_preload_TRE(TRGTA), 
    NITF_preload_TRE(USE00A), 
    NITF_preload_TRE(XML_DATA_CONTENT),

    //NITF_preload_TRE(TEST_DES), // This should ALWAYS come from the DLL/SO
    NITF_preload_TRE_(TEST_PRELOADED_DES,  NRT_TRUE /*enabled*/),

	{ NULL, NULL, NULL, NRT_FALSE } // end of list
};
