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

#ifndef __NITF_TRE_PRIVATE_DATA_H__
#define __NITF_TRE_PRIVATE_DATA_H__

#include "nitf/TRE.h"
#include "nitf/TREDescription.h"

NITF_CXX_GUARD


/*!
 * A structure meant to be used for the private data of the TRE structure.
 * It keeps track of the length (if given) as well as the Description
 */
typedef struct _nitf_TREPrivateData
{
    nitf_Uint32 length;
    char* descriptionName;   /* the name/ID of the TREDescription */
    nitf_TREDescription* description;
    nitf_HashTable *hash;
    NITF_DATA *userData;    /*! user-defined - meant for extending this */
} nitf_TREPrivateData;


NITFAPI(nitf_TREPrivateData *) nitf_TREPrivateData_construct(
        nitf_Error * error);

NITFAPI(nitf_TREPrivateData *) nitf_TREPrivateData_clone(
        nitf_TREPrivateData *source, nitf_Error * error);

NITFAPI(void) nitf_TREPrivateData_destruct(nitf_TREPrivateData **priv);

NITFPROT(NITF_BOOL) nitf_TREPrivateData_flush(nitf_TREPrivateData *priv,
                                              nitf_Error * error);

NITFPROT(NITF_BOOL) nitf_TREPrivateData_setDescriptionName(
        nitf_TREPrivateData *priv, const char* name, nitf_Error * error);



NITF_CXX_ENDGUARD

#endif

