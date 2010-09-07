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
 * License along with this program; if not, If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#ifndef __NITF_TYPES_H__
#define __NITF_TYPES_H__

#include "nrt/Types.h"

typedef nrt_Uint8                   nitf_Uint8;
typedef nrt_Uint16                  nitf_Uint16;
typedef nrt_Uint32                  nitf_Uint32;
typedef nrt_Uint64                  nitf_Uint64;
typedef nrt_Int8                    nitf_Int8;
typedef nrt_Int16                   nitf_Int16;
typedef nrt_Int32                   nitf_Int32;
typedef nrt_Int64                   nitf_Int64;
typedef nrt_IOHandle                nitf_IOHandle;
typedef NRT_NATIVE_DLL              NITF_NATIVE_DLL;
typedef NRT_DLL_FUNCTION_PTR        NITF_DLL_FUNCTION_PTR;
typedef nrt_AccessFlags             nitf_AccessFlags;
typedef nrt_CreationFlags           nitf_CreationFlags;
typedef nrt_Off                     nitf_Off;

#define NITF_MAX_PATH               NRT_MAX_PATH
#define NITF_DEFAULT_PERM           NRT_DEFAULT_PERM
#define NITF_INVALID_HANDLE_VALUE   NRT_INVALID_HANDLE_VALUE
#define NITF_INVALID_HANDLE         NRT_INVALID_HANDLE
#define NITF_SEEK_CUR               NRT_SEEK_CUR
#define NITF_SEEK_SET               NRT_SEEK_SET
#define NITF_SEEK_END               NRT_SEEK_END
#define NITF_CREATE                 NRT_CREATE
#define NITF_OPEN_EXISTING          NRT_OPEN_EXISTING
#define NITF_TRUNCATE               NRT_TRUNCATE
#define NITF_ACCESS_READONLY        NRT_ACCESS_READONLY
#define NITF_ACCESS_WRITEONLY       NRT_ACCESS_WRITEONLY
#define NITF_ACCESS_READWRITE       NRT_ACCESS_READWRITE
#define NITF_INT64                  NRT_INT64
#define NITF_SUCCESS                NRT_SUCCESS
#define NITF_FAILURE                NRT_FAILURE

#define nitf_System_swap16          nrt_System_swap16
#define nitf_System_swap32          nrt_System_swap32
#define nitf_System_swap64          nrt_System_swap64
#define nitf_System_swap64c         nrt_System_swap64c

#define NITF_NTOHS                  NRT_NTOHS
#define NITF_HTONS                  NRT_HTONS
#define NITF_NTOHL                  NRT_NTOHL
#define NITF_HTONL                  NRT_HTONL
#define NITF_HTONLL                 NRT_HTONLL
#define NITF_NTOHLL                 NRT_NTOHLL
#define NITF_HTONLC                 NRT_HTONLC
#define NITF_NTOHLC                 NRT_NTOHLC


/* Enum for the supported version types */
typedef enum _nitf_Version
{
    NITF_VER_20 = 100,
    NITF_VER_21,
    NITF_VER_UNKNOWN
} nitf_Version;

typedef enum _nitf_CornersType
{
    NITF_CORNERS_UNKNOWN = -1,
    NITF_CORNERS_UTM,
    NITF_CORNERS_UTM_UPS_S,
    NITF_CORNERS_UTM_UPS_N,
    NITF_CORNERS_GEO,
    NITF_CORNERS_DECIMAL
} nitf_CornersType;

/* These macros check the NITF Version */
#define IS_NITF20(V) ((V) == NITF_VER_20)
#define IS_NITF21(V) ((V) == NITF_VER_21)


typedef void NITF_DATA;
#endif
