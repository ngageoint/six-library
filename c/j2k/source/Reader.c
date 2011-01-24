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

#include "j2k/Reader.h"

J2KAPI(NRT_BOOL) j2k_Reader_canReadTiles(j2k_Reader *reader, nrt_Error *error)
{
    if (reader->iface->canReadTiles)
        return reader->iface->canReadTiles(reader->data, error);
    /* otherwise, no */
    return NRT_FAILURE;
}

J2KAPI(nrt_Uint64) j2k_Reader_readTile(j2k_Reader *reader,
        nrt_Uint32 tileX, nrt_Uint32 tileY,
        nrt_Uint8 **buf, nrt_Error *error)
{
    return reader->iface->readTile(reader->data, tileX, tileY, buf, error);
}

J2KAPI(nrt_Uint64) j2k_Reader_readRegion(j2k_Reader *reader,
        nrt_Uint32 x0, nrt_Uint32 y0, nrt_Uint32 x1, nrt_Uint32 y1,
        nrt_Uint8 **buf, nrt_Error *error)
{
    return reader->iface->readRegion(reader->data, x0, y0, x1, y1, buf, error);
}

J2KAPI(j2k_Container*) j2k_Reader_getContainer(j2k_Reader *reader,
                                               nrt_Error *error)
{
    return reader->iface->getContainer(reader->data, error);
}

J2KAPI(void) j2k_Reader_destruct(j2k_Reader **reader)
{
    if (*reader)
    {
        if ((*reader)->iface && (*reader)->data)
            (*reader)->iface->destruct((*reader)->data);
        J2K_FREE(*reader);
        *reader = NULL;
    }
}

