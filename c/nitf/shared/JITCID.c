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

#include <nitf/IOHandle.h>
#include <nitf/TREUtils.h>
#include <nitf/Record.h>

NITF_CXX_GUARD

static nitf_TREDescription description[] = {
    {NITF_BCS_A, 100, "File Comment", "FILCMT1" },
	{NITF_BCS_A, 100, "File Comment (part II)", "FILCMT2" },
    {NITF_END, 0, NULL, NULL}
};

#if 1

/* Define the available descriptions and the default one */
static nitf_TREDescriptionInfo descriptions[] = {
    { "JITCID", description, NITF_TRE_DESC_NO_LENGTH },
    { NULL, NULL, NITF_TRE_DESC_NO_LENGTH }
};

static nitf_TREDescriptionSet descriptionSet = { 0, descriptions };

static char *ident[] = { NITF_PLUGIN_TRE_KEY, "JITCID", NULL }; 
    

static nitf_TREHandler* gHandler = NULL;
NITFAPI(char**) JITCID_init(nitf_Error* error)
{
    gHandler = nitf_TREUtils_createBasicHandler(&descriptionSet, error);
    if (!gHandler)
	return NULL;
    
    return ident;
} 
    
NITFAPI(nitf_TREHandler*) JITCID_handler(nitf_Error* error)
{
    return gHandler;
}

NITFAPI(void) JITCID_cleanup(void)
{

}
#else
NITF_DECLARE_SINGLE_PLUGIN(JITCID, description)

#endif



NITF_CXX_ENDGUARD
