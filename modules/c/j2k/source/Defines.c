/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 * (C) Copyright 2022, Maxar Technologies, Inc.
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

#include "j2k/Config.h"

#include <assert.h>

#include "j2k/Defines.h"
#include "j2k/Reader.h"
#include "j2k/Writer.h"

// provide a default j2k_getImplementation(), and test our macros
#if NITRO_J2K_IMPLEMENTATION == NITRO_J2K_IMPLEMENTATION_OPENJPEG
    // done in OpenJPEGImpl.c
#elif NITRO_J2K_IMPLEMENTATION == NITRO_J2K_IMPLEMENTATION_JASPER
    // done in JasPerImpl.c
#elif NITRO_J2K_IMPLEMENTATION == NITRO_J2K_IMPLEMENTATION_NONE
/******************************************************************************/
/******************************************************************************/
/* PUBLIC FUNCTIONS                                                           */
/******************************************************************************/
/******************************************************************************/

J2KAPI(j2k_Reader*) j2k_Reader_open(const char* fname, nrt_Error* error)
{
    if (error == NULL)
    {
        return NULL;
    }

    if (!fname)
    {
        nrt_Error_init(error, "NULL filename", NRT_CTXT, NRT_ERR_INVALID_OBJECT);
        return NULL;
    }

    nrt_Error_init(error, "Not implemented.", NRT_CTXT, NRT_ERR_INVALID_OBJECT);
    return NULL;
}

J2KAPI(j2k_Reader*) j2k_Reader_openIO(nrt_IOInterface* io, nrt_Error* error)
{
    if (error == NULL)
    {
        return NULL;
    }

    nrt_Error_init(error, "Not implemented.", NRT_CTXT, NRT_ERR_INVALID_OBJECT);
    return NULL;
}

J2KAPI(j2k_Writer*) j2k_Writer_construct(j2k_Container* container,
    j2k_WriterOptions* writerOps,
    nrt_Error* error)
{
    if (error == NULL)
    {
        return NULL;
    }

    nrt_Error_init(error, "Not implemented.", NRT_CTXT, NRT_ERR_INVALID_OBJECT);
    return NULL;
}

J2KAPI(j2k_Implementation) j2k_getImplementation(nrt_Error* error)
{
    assert(NITRO_J2K_IMPLEMENTATION == j2k_Implementation_None);
    if (error == NULL)
    {
        return j2k_Implementation_Error;
    }
    return j2k_Implementation_None;
}
#else
#error "Can't determine J2K implementation."
#endif