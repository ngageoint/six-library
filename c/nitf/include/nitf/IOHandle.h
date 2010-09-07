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

#ifndef __NITF_IO_HANDLE_H__
#define __NITF_IO_HANDLE_H__

#include "nitf/System.h"
#include "nrt/IOHandle.h"

#define NITF_IO_SUCCESS         NRT_IO_SUCCESS
#define NITF_MAX_READ_ATTEMPTS  NRT_MAX_READ_ATTEMPTS

#define nitf_IOHandle_create    nrt_IOHandle_create
#define nitf_IOHandle_read      nrt_IOHandle_read
#define nitf_IOHandle_write     nrt_IOHandle_write
#define nitf_IOHandle_seek      nrt_IOHandle_seek
#define nitf_IOHandle_tell      nrt_IOHandle_tell
#define nitf_IOHandle_getSize   nrt_IOHandle_getSize
#define nitf_IOHandle_close     nrt_IOHandle_close

#endif
