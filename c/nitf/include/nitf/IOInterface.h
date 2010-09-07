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

#include "nrt/IOInterface.h"

typedef NRT_IO_INTERFACE_READ           NITF_IO_INTERFACE_READ;
typedef NRT_IO_INTERFACE_WRITE          NITF_IO_INTERFACE_WRITE;
typedef NRT_IO_INTERFACE_SEEK           NITF_IO_INTERFACE_SEEK;
typedef NRT_IO_INTERFACE_TELL           NITF_IO_INTERFACE_TELL;
typedef NRT_IO_INTERFACE_GET_SIZE       NITF_IO_INTERFACE_GET_SIZE;
typedef NRT_IO_INTERFACE_CLOSE          NITF_IO_INTERFACE_CLOSE;
typedef NRT_IO_INTERFACE_DESTRUCT       NITF_IO_INTERFACE_DESTRUCT;

typedef nrt_IIOInterface                nitf_IIOInterface;
typedef nrt_IOInterface                 nitf_IOInterface;

#define nitf_IOInterface_read           nrt_IOInterface_read
#define nitf_IOInterface_write          nrt_IOInterface_write
#define nitf_IOInterface_seek           nrt_IOInterface_seek
#define nitf_IOInterface_tell           nrt_IOInterface_tell
#define nitf_IOInterface_getSize        nrt_IOInterface_getSize
#define nitf_IOInterface_close          nrt_IOInterface_close
#define nitf_IOInterface_destruct       nrt_IOInterface_destruct
#define nitf_IOHandleAdaptor_construct  nrt_IOHandleAdaptor_construct

#endif
