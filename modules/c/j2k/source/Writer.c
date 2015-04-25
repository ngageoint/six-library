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

#include "j2k/Writer.h"
#include "nitf/WriterOptions.h"

J2KAPI(NRT_BOOL) j2k_Writer_setOptions(j2k_WriterOptions* options, 
                                       nrt_HashTable* userOptions, 
                                       nrt_Error* error)
{
    nrt_Pair* compressionRatio;
    nrt_Pair* numResolutions;
    if(options && userOptions)
    {
        compressionRatio = nrt_HashTable_find(userOptions, C8_COMPRESSION_RATIO_KEY);
        numResolutions = nrt_HashTable_find(userOptions, C8_NUM_RESOLUTIONS_KEY);

        if(compressionRatio)
        {
            options->compressionRatio = *((double*)compressionRatio->data);
        }
        if(numResolutions)
        {
            options->numResolutions = *((nrt_Uint32*)numResolutions->data);
        }
    }

    return NRT_SUCCESS;
}

J2KAPI(NRT_BOOL) j2k_Writer_setTile(j2k_Writer *writer,
                                    nrt_Uint32 tileX,
                                    nrt_Uint32 tileY,
                                    const nrt_Uint8 *buf,
                                    nrt_Uint32 bufSize,
                                    nrt_Error *error)
{
    return writer->iface->setTile(writer->data, tileX, tileY, buf, bufSize, error);
}

J2KAPI(NRT_BOOL) j2k_Writer_write(j2k_Writer *writer,
                                  nrt_IOInterface *io,
                                  nrt_Error *error)
{
    return writer->iface->write(writer->data, io, error);
}

J2KAPI(j2k_Container*) j2k_Writer_getContainer(j2k_Writer *writer,
                                               nrt_Error *error)
{
    return writer->iface->getContainer(writer->data, error);
}

J2KAPI(void) j2k_Writer_destruct(j2k_Writer **writer)
{
    if (*writer)
    {
        if ((*writer)->iface && (*writer)->data)
            (*writer)->iface->destruct((*writer)->data);
        J2K_FREE(*writer);
        *writer = NULL;
    }
}

