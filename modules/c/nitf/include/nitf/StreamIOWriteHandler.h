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

#ifndef __NITF_STREAM_IO_WRITE_HANDLER_H__
#define __NITF_STREAM_IO_WRITE_HANDLER_H__

#include "nitf/System.h"
#include "nitf/WriteHandler.h"


NITF_CXX_GUARD

/**
 * Create a WriteHandler that streams from an input IO source. This is useful
 * if you want to bypass any specialized WriteHandlers and/or your data is
 * already in the desired format.
 *
 * \param inputHandle   The input IOHandle
 * \param offset        The offset of the IOHandle to start from
 * \param bytes         The # of bytes to write
 * \param error         The error object, which will get populated on error
 * \return              a nitf_WriteHandler*, or NULL on error
 */
NITFAPI(nitf_WriteHandler*) nitf_StreamIOWriteHandler_construct(
    nitf_IOInterface *io,
    nitf_Uint64 offset,
    nitf_Uint64 bytes,
    nitf_Error *error);


NITF_CXX_ENDGUARD

#endif
