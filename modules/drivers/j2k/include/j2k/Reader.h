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

#ifndef __J2K_READER_H__
#define __J2K_READER_H__

#include "j2k/Container.h"

J2K_CXX_GUARD

typedef J2K_BOOL        (*J2K_IREADER_CAN_READ_TILES)(J2K_USER_DATA*, nrt_Error*);
typedef nrt_Uint64      (*J2K_IREADER_READ_TILE)(J2K_USER_DATA*, nrt_Uint32 tileX,
                                                nrt_Uint32 tileY, nrt_Uint8 **buf,
                                                nrt_Error*);
typedef nrt_Uint64      (*J2K_IREADER_READ_REGION)(J2K_USER_DATA*, nrt_Uint32 x0,
                                                  nrt_Uint32 y0, nrt_Uint32 x1,
                                                  nrt_Uint32 y1, nrt_Uint8 **buf,
                                                  nrt_Error*);
typedef j2k_Container*  (*J2K_IREADER_GET_CONTAINER)(J2K_USER_DATA*, nrt_Error*);
typedef void            (*J2K_IREADER_DESTRUCT)(J2K_USER_DATA *);

typedef struct _j2k_IReader
{
    J2K_IREADER_CAN_READ_TILES  canReadTiles;
    J2K_IREADER_READ_TILE       readTile;
    J2K_IREADER_READ_REGION     readRegion;
    J2K_IREADER_GET_CONTAINER   getContainer;
    J2K_IREADER_DESTRUCT        destruct;
} j2k_IReader;

typedef struct _j2k_Reader
{
    j2k_IReader *iface;
    J2K_USER_DATA *data;
} j2k_Reader;

/**
 * Opens a J2K Container from an IOInterface
 */
J2KAPI(j2k_Reader*) j2k_Reader_openIO(nrt_IOInterface*, nrt_Error*);

/**
 * Opens a J2K Container from a file on disk.
 */
J2KAPI(j2k_Reader*) j2k_Reader_open(const char*, nrt_Error*);

/**
 * Declares whether or not the reader can read individual tiles
 */
J2KAPI(J2K_BOOL) j2k_Reader_canReadTiles(j2k_Reader*, nrt_Error*);

/**
 * Reads an individual tile at the given indices
 */
J2KAPI(nrt_Uint64) j2k_Reader_readTile(j2k_Reader*, nrt_Uint32 tileX,
                                          nrt_Uint32 tileY, nrt_Uint8 **buf,
                                          nrt_Error*);

/**
 * Reads image data from the desired region
 */
J2KAPI(nrt_Uint64) j2k_Reader_readRegion(j2k_Reader*, nrt_Uint32 x0,
                                          nrt_Uint32 y0, nrt_Uint32 x1,
                                          nrt_Uint32 y1, nrt_Uint8 **buf,
                                          nrt_Error*);

/**
 * Returns the associated container (the Reader will still own it)
 */
J2KAPI(j2k_Container*) j2k_Reader_getContainer(j2k_Reader*, nrt_Error*);

/**
 * Destroys the Reader object
 */
J2KAPI(void) j2k_Reader_destruct(j2k_Reader**);

J2K_CXX_ENDGUARD

#endif
