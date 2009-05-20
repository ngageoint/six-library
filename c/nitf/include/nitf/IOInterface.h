/* =========================================================================
 * This file is part of NITRO
 * =========================================================================
 *
 * (C) Copyright 2004 - 2008, General Dynamics - Advanced Information Systems
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

#ifndef __NITF_IO_INTERFACE_H__
#define __NITF_IO_INTERFACE_H__

/* Default implementation */
#include "nitf/IOHandle.h"

NITF_CXX_GUARD


typedef NITF_BOOL (*NITF_IO_INTERFACE_READ) (NITF_DATA* data,
                                             char *buf, size_t size,
                                             nitf_Error *error);

typedef NITF_BOOL (*NITF_IO_INTERFACE_WRITE)(NITF_DATA* data,
                                             const char *buf, size_t size,
                                             nitf_Error * error);

typedef nitf_Off    (*NITF_IO_INTERFACE_SEEK)(NITF_DATA* data,
                                              nitf_Off offset, int whence,
                                              nitf_Error* error);

typedef nitf_Off     (*NITF_IO_INTERFACE_TELL)(NITF_DATA* data,
                                               nitf_Error * error);

typedef nitf_Off     (*NITF_IO_INTERFACE_GET_SIZE)(NITF_DATA* data,
                                                   nitf_Error * error);

typedef NITF_BOOL (*NITF_IO_INTERFACE_CLOSE)(NITF_DATA* data,
                                             nitf_Error* error);

typedef void      (*NITF_IO_INTERFACE_DESTRUCT) (NITF_DATA *);

typedef struct _nitf_IIOInterface
{
    NITF_IO_INTERFACE_READ     read;
    NITF_IO_INTERFACE_WRITE    write;
    NITF_IO_INTERFACE_SEEK     seek;
    NITF_IO_INTERFACE_TELL     tell;
    NITF_IO_INTERFACE_GET_SIZE getSize;
    NITF_IO_INTERFACE_CLOSE    close;
    NITF_IO_INTERFACE_DESTRUCT destruct;
}
nitf_IIOInterface;

typedef struct _nitf_IOInterface
{
    NITF_DATA* data;
    nitf_IIOInterface* iface;
}
nitf_IOInterface;

NITFAPI(NITF_BOOL) nitf_IOInterface_read(nitf_IOInterface*,
                                         char *buf, size_t size,
                                         nitf_Error *error);

NITFAPI(NITF_BOOL) nitf_IOInterface_write(nitf_IOInterface* io,
                                          const char *buf,
                                          size_t size,
                                          nitf_Error* error);

NITFAPI(nitf_Off) nitf_IOInterface_seek(nitf_IOInterface* io,
                                        nitf_Off offset,
                                        int whence,
                                        nitf_Error* error);

NITFAPI(nitf_Off) nitf_IOInterface_tell(nitf_IOInterface* io,
                                     nitf_Error* error);



NITFAPI(nitf_Off) nitf_IOInterface_getSize(nitf_IOInterface* io,
                                        nitf_Error* error);


NITFAPI(NITF_BOOL) nitf_IOInterface_close(nitf_IOInterface* io,
                                          nitf_Error* error);
/*!
 *  'Destructor' for IOInterface.  This relies on its internal iface,
 *  pointer table to determine how/if to destruct the NITF_DATA* data
 *  field.
 *
 */
NITFAPI(void) nitf_IOInterface_destruct(nitf_IOInterface** io);

/*!
 *  This special 'subclass' puts the IOHandle into the IOInterface
 *  that is needed to supply it for reads and writes within the library.
 *
 *  You can define your own
 *  interfaces, and set them using the nitf_Reader_setInputSource()
 *  and the nitf_Writer_setOutputDest()
 */
NITFAPI(nitf_IOInterface*) nitf_IOHandleAdaptor_construct(nitf_IOHandle handle,
                                                          nitf_Error* error);




NITF_CXX_ENDGUARD

#endif
