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

typedef NRT_BOOL(*NRT_IO_INTERFACE_READ) (NRT_DATA *, char *, size_t,
                                          nrt_Error *);
typedef NRT_BOOL(*NRT_IO_INTERFACE_WRITE) (NRT_DATA *, const char *, size_t,
                                           nrt_Error *);
typedef NRT_BOOL(*NRT_IO_INTERFACE_CAN_SEEK) (NRT_DATA *, nrt_Error *);
typedef nrt_Off(*NRT_IO_INTERFACE_SEEK) (NRT_DATA *, nrt_Off, int, nrt_Error *);
typedef nrt_Off(*NRT_IO_INTERFACE_TELL) (NRT_DATA *, nrt_Error *);
typedef nrt_Off(*NRT_IO_INTERFACE_GET_SIZE) (NRT_DATA *, nrt_Error *);
typedef int (*NRT_IO_INTERFACE_GET_MODE) (NRT_DATA *, nrt_Error *);
typedef NRT_BOOL(*NRT_IO_INTERFACE_CLOSE) (NRT_DATA *, nrt_Error *);
typedef void (*NRT_IO_INTERFACE_DESTRUCT) (NRT_DATA *);

typedef struct _NRT_IIOInterface
{
    NRT_IO_INTERFACE_READ read;
    NRT_IO_INTERFACE_WRITE write;
    NRT_IO_INTERFACE_CAN_SEEK canSeek;
    NRT_IO_INTERFACE_SEEK seek;
    NRT_IO_INTERFACE_TELL tell;
    NRT_IO_INTERFACE_GET_SIZE getSize;
    NRT_IO_INTERFACE_GET_MODE getMode;
    NRT_IO_INTERFACE_CLOSE close;
    NRT_IO_INTERFACE_DESTRUCT destruct;
} nrt_IIOInterface;

typedef struct _NRT_IOInterface
{
    NRT_DATA *data;
    nrt_IIOInterface *iface;
} nrt_IOInterface;

/**
 * Reads data from the interface
 */
NRTAPI(NRT_BOOL) nrt_IOInterface_read(nrt_IOInterface *, char *buf, size_t size,
                                      nrt_Error * error);

/**
 * Writes data to the interface
 */
NRTAPI(NRT_BOOL) nrt_IOInterface_write(nrt_IOInterface * io, const char *buf,
                                       size_t size, nrt_Error * error);

/**
 * Returns whether the interface is seekable
 */
NRTAPI(NRT_BOOL) nrt_IOInterface_canSeek(nrt_IOInterface * io, nrt_Error *);

/**
 * Seeks to the offset specified, given the provided seek scenario
 */
NRTAPI(nrt_Off) nrt_IOInterface_seek(nrt_IOInterface * io, nrt_Off offset,
                                     int whence, nrt_Error * error);

/**
 * Returns the current offset
 */
NRTAPI(nrt_Off) nrt_IOInterface_tell(nrt_IOInterface * io, nrt_Error * error);

/**
 * Returns the current size
 */
NRTAPI(nrt_Off) nrt_IOInterface_getSize(nrt_IOInterface * io,
                                        nrt_Error * error);

/**
 * Returns the access mode (NRT_ACCESS_READONLY, NRT_ACCESS_WRITEONLY,
 * NRT_ACCESS_READWRITE)
 */
NRTAPI(int) nrt_IOInterface_getMode(nrt_IOInterface * io, nrt_Error * error);

/**
 * Closes the interface
 */
NRTAPI(NRT_BOOL) nrt_IOInterface_close(nrt_IOInterface * io, nrt_Error * error);

/**
 * Destroys the interface and cleans up any owned resources
 */
NRTAPI(void) nrt_IOInterface_destruct(nrt_IOInterface ** io);

/**
 * Creates an IOInterface that wraps an IOHandle.
 */
NRTAPI(nrt_IOInterface *) nrt_IOHandleAdapter_construct(nrt_IOHandle handle,
                                                        int accessMode,
                                                        nrt_Error * error);

/**
 * Creates an IOInterface that wraps an IOHandle.
 */
NRTAPI(nrt_IOInterface *) nrt_IOHandleAdapter_open(const char *fname,
                                                   int accessFlags,
                                                   int creationFlags,
                                                   nrt_Error * error);

/**
 * Creats an IOInterface that wraps a buffer
 */
NRTAPI(nrt_IOInterface *) nrt_BufferAdapter_construct(char *buf, size_t size,
                                                      NRT_BOOL ownBuf,
                                                      nrt_Error * error);

NRT_CXX_ENDGUARD
#endif
