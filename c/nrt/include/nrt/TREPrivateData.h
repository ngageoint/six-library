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

#ifndef __NRT_TRE_PRIVATE_DATA_H__
#define __NRT_TRE_PRIVATE_DATA_H__

#include "nrt/TRE.h"
#include "nrt/TREDescription.h"
#include "nrt/HashTable.h"

NRT_CXX_GUARD


/*!
 * A structure meant to be used for the private data of the TRE structure.
 * It keeps track of the length (if given) as well as the Description
 */
typedef struct _NRT_TREPrivateData
{
    nrt_Uint32 length;
    char* descriptionName;   /* the name/ID of the TREDescription */
    nrt_TREDescription* description;
    nrt_HashTable *hash;
    NRT_DATA *userData;    /*! user-defined - meant for extending this */
} nrt_TREPrivateData;


NRTAPI(nrt_TREPrivateData *) nrt_TREPrivateData_construct(
        nrt_Error * error);

NRTAPI(nrt_TREPrivateData *) nrt_TREPrivateData_clone(
        nrt_TREPrivateData *source, nrt_Error * error);

NRTAPI(void) nrt_TREPrivateData_destruct(nrt_TREPrivateData **priv);

NRTPROT(NRT_BOOL) nrt_TREPrivateData_flush(nrt_TREPrivateData *priv,
                                              nrt_Error * error);

NRTPROT(NRT_BOOL) nrt_TREPrivateData_setDescriptionName(
        nrt_TREPrivateData *priv, const char* name, nrt_Error * error);



NRT_CXX_ENDGUARD

#endif

