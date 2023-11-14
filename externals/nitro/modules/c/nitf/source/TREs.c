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

#define NITF_preload_TRE(Tre_) { #Tre_, Tre_##_init, Tre_##_handler }

extern const nitf_TREPreloaded preloadedTREs[];
const nitf_TREPreloaded preloadedTREs[] = {
/*
	NITF_preload_TRE(ACCHZB),
	NITF_preload_TRE(ACCPOB),
	NITF_preload_TRE(ACFTA),
	NITF_preload_TRE(AIMIDB),
	NITF_preload_TRE(CSCRNA),
	NITF_preload_TRE(CSEXRB),
	//NITF_preload_TRE(ENGRDA),
	NITF_preload_TRE(HISTOA),
	NITF_preload_TRE(JITCID),
	NITF_preload_TRE(PTPRAA),
	NITF_preload_TRE(RPFHDR),

	NITF_preload_TRE(TEST_DES),
*/
	{ NULL, NULL, NULL }
};
