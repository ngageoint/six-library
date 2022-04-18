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

#ifndef __J2K_WRITER_H__
#define __J2K_WRITER_H__

#include "j2k/Container.h"

J2K_CXX_GUARD

typedef J2K_BOOL        (*J2K_IWRITER_SET_TILE)(J2K_USER_DATA*, uint32_t,
                                                uint32_t, const uint8_t *,
                                                uint32_t, nrt_Error*);
typedef J2K_BOOL        (*J2K_IWRITER_WRITE)(J2K_USER_DATA*, nrt_IOInterface*,
                                             nrt_Error*);
typedef j2k_Container*  (*J2K_IWRITER_GET_CONTAINER)(J2K_USER_DATA*, nrt_Error*);
typedef void            (*J2K_IWRITER_DESTRUCT)(J2K_USER_DATA *);

typedef struct _j2k_IWriter
{
    J2K_IWRITER_SET_TILE        setTile;
    J2K_IWRITER_WRITE           write;
    J2K_IWRITER_GET_CONTAINER   getContainer;
    J2K_IWRITER_DESTRUCT        destruct;
} j2k_IWriter;

#if _MSC_VER
#pragma warning(push)
#pragma warning(disable: 4820) // '...': '...' bytes padding added after data member '...'
#endif
typedef struct _j2k_WriterOptions
{
    /* TODO add more options as we see fit */
    float compressionRatio;
    uint32_t numResolutions;
} j2k_WriterOptions;
#if _MSC_VER
#pragma warning(pop)
#endif

typedef struct _j2k_Writer
{
    j2k_IWriter *iface;
    J2K_USER_DATA *data;
} j2k_Writer;

J2KAPI(NRT_BOOL) j2k_Writer_setOptions(j2k_WriterOptions* options, 
                                       nrt_HashTable* userOptions, 
                                       nrt_Error* error);
/**
 * Opens a J2K Container from an IOInterface
 */
J2KAPI(j2k_Writer*) j2k_Writer_construct(j2k_Container*, j2k_WriterOptions*,
                                         nrt_Error*);

/**
 * Sets the uncompressed data for the tile at the given indices.
 * It is assumed that the passed-in buffer will be available for deletion
 * immediately after the call returns.
 */
J2KAPI(J2K_BOOL) j2k_Writer_setTile(j2k_Writer*, uint32_t tileX,
                                    uint32_t tileY, const uint8_t *buf,
                                    uint32_t bufSize, nrt_Error*);

/**
 * Writes to the IOInterface
 */
J2KAPI(J2K_BOOL) j2k_Writer_write(j2k_Writer*, nrt_IOInterface*, nrt_Error*);

/**
 * Returns the associated container
 */
J2KAPI(j2k_Container*) j2k_Writer_getContainer(j2k_Writer*, nrt_Error*);

/**
 * Destroys the Writer object
 */
J2KAPI(void) j2k_Writer_destruct(j2k_Writer**);

J2K_CXX_ENDGUARD

#endif
