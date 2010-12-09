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
 * License along with this program; if not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NRT_IO_INTERFACE_H__
#define __NRT_IO_INTERFACE_H__

/* Default implementation */
#include "nrt/IOHandle.h"

NRT_CXX_GUARD

typedef NRT_BOOL (*NRT_IO_INTERFACE_READ)(NRT_DATA* data, char *buf,
        size_t size, nrt_Error *error);

typedef NRT_BOOL (*NRT_IO_INTERFACE_WRITE)(NRT_DATA* data, const char *buf,
        size_t size, nrt_Error * error);

typedef nrt_Off (*NRT_IO_INTERFACE_SEEK)(NRT_DATA* data, nrt_Off offset,
        int whence, nrt_Error* error);

typedef nrt_Off (*NRT_IO_INTERFACE_TELL)(NRT_DATA* data, nrt_Error * error);

typedef nrt_Off (*NRT_IO_INTERFACE_GET_SIZE)(NRT_DATA* data, nrt_Error * error);

typedef NRT_BOOL (*NRT_IO_INTERFACE_CLOSE)(NRT_DATA* data, nrt_Error* error);

typedef void (*NRT_IO_INTERFACE_DESTRUCT)(NRT_DATA *);

typedef struct _NRT_IIOInterface
{
    NRT_IO_INTERFACE_READ read;
    NRT_IO_INTERFACE_WRITE write;
    NRT_IO_INTERFACE_SEEK seek;
    NRT_IO_INTERFACE_TELL tell;
    NRT_IO_INTERFACE_GET_SIZE getSize;
    NRT_IO_INTERFACE_CLOSE close;
    NRT_IO_INTERFACE_DESTRUCT destruct;
} nrt_IIOInterface;

typedef struct _NRT_IOInterface
{
    NRT_DATA* data;
    nrt_IIOInterface* iface;
} nrt_IOInterface;

NRTAPI(NRT_BOOL) nrt_IOInterface_read(nrt_IOInterface*,
        char *buf, size_t size,
        nrt_Error *error);

NRTAPI(NRT_BOOL) nrt_IOInterface_write(nrt_IOInterface* io,
        const char *buf,
        size_t size,
        nrt_Error* error);

NRTAPI(nrt_Off) nrt_IOInterface_seek(nrt_IOInterface* io,
        nrt_Off offset,
        int whence,
        nrt_Error* error);

NRTAPI(nrt_Off) nrt_IOInterface_tell(nrt_IOInterface* io,
        nrt_Error* error);

NRTAPI(nrt_Off) nrt_IOInterface_getSize(nrt_IOInterface* io,
        nrt_Error* error);

NRTAPI(NRT_BOOL) nrt_IOInterface_close(nrt_IOInterface* io,
        nrt_Error* error);
/*!
 *  'Destructor' for IOInterface.  This relies on its internal iface,
 *  pointer table to determine how/if to destruct the NRT_DATA* data
 *  field.
 *
 */
NRTAPI(void) nrt_IOInterface_destruct(nrt_IOInterface** io);

/*!
 *  This special 'subclass' puts the IOHandle into the IOInterface
 *  that is needed to supply it for reads and writes within the library.
 *
 *  You can define your own
 *  interfaces, and set them using the nrt_Reader_setInputSource()
 *  and the nrt_Writer_setOutputDest()
 */
NRTAPI(nrt_IOInterface*) nrt_IOHandleAdaptor_construct(nrt_IOHandle handle,
        nrt_Error* error);

NRT_CXX_ENDGUARD

#endif
