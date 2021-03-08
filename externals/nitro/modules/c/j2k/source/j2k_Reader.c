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

#include "j2k/Reader.h"

J2KAPI(NRT_BOOL) j2k_Reader_canReadTiles(j2k_Reader *reader, nrt_Error *error)
{
    if (reader->iface->canReadTiles)
        return reader->iface->canReadTiles(reader->data, error);
    /* otherwise, no */
    return NRT_FAILURE;
}

J2KAPI(uint64_t) j2k_Reader_readTile(j2k_Reader *reader,
        uint32_t tileX, uint32_t tileY,
        uint8_t **buf, nrt_Error *error)
{
    return reader->iface->readTile(reader->data, tileX, tileY, buf, error);
}

J2KAPI(uint64_t) j2k_Reader_readRegion(j2k_Reader *reader,
        uint32_t x0, uint32_t y0, uint32_t x1, uint32_t y1,
        uint8_t **buf, nrt_Error *error)
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

