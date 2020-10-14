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

#ifndef __NRT_TYPES_H__
#define __NRT_TYPES_H__
#pragma once

#include "nrt/Config.h"

#   include <assert.h>
#   include <stdlib.h>
#   include <stdint.h>
#   include <string.h>
#   include <stdio.h>
#   include <math.h>
#   include <stdarg.h>
#   include <sys/types.h>
#   include <ctype.h>
#   include <time.h>

 // Keeping these here so that code using "nrt_Uint8" still compiles;
 // we can't make others change to "uint8_t".
typedef uint8_t nrt_Uint8;
typedef uint16_t nrt_Uint16;
typedef uint32_t nrt_Uint32;
typedef uint64_t nrt_Uint64;

typedef int8_t nrt_Int8;
typedef int16_t nrt_Int16;
typedef int32_t nrt_Int32;
typedef int64_t nrt_Int64;

#if defined(WIN32) || defined(_WIN32)
#      include <windows.h>

/*  Types are defined for windows here  */
typedef HANDLE nrt_IOHandle;
typedef HINSTANCE NRT_NATIVE_DLL;
typedef FARPROC NRT_DLL_FUNCTION_PTR;
typedef DWORD nrt_AccessFlags;
typedef DWORD nrt_CreationFlags;
/*  Determine the maximum file path length  */
#      define  NRT_MAX_PATH     MAX_PATH

/* use nrt_Off instead of off_t, since on Windows we want int64_t used */
typedef int64_t nrt_Off;

/*  IO macros  */
#      define  NRT_INVALID_HANDLE_VALUE INVALID_HANDLE_VALUE
#      define  NRT_INVALID_HANDLE(I) ((I) == NRT_INVALID_HANDLE_VALUE)
#      define  NRT_SEEK_CUR         FILE_CURRENT
#      define  NRT_SEEK_SET         FILE_BEGIN
#      define  NRT_SEEK_END         FILE_END
#      define  NRT_CREATE           OPEN_ALWAYS
#      define  NRT_OPEN_EXISTING    OPEN_EXISTING
#      define  NRT_TRUNCATE         OPEN_EXISTING
#      define  NRT_ACCESS_READONLY  GENERIC_READ
#      define  NRT_ACCESS_WRITEONLY GENERIC_WRITE
#      define  NRT_ACCESS_READWRITE GENERIC_READ|GENERIC_WRITE

/* type suffixes */
#if defined(__GNUC__)
#      define  NRT_INT64(x) x##LL
#else
#      define  NRT_INT64(x) x##i64
#endif

#else
/*
*  Here, we define the basic libraries needed by
*  all compilers (the non-platform specific code)
*
*/

#      include <unistd.h>
#      include <errno.h>
#      include <dirent.h>
#      include <fcntl.h>
#      include <sys/stat.h>
#      include <dlfcn.h>
#      include <inttypes.h>

/* Defines MAXPATHLEN
 * Absolutely essential to include this here as otherwise we could see below
 * that it's not defined and set NRT_MAX_PATH to 1024 and then later someone
 * else could include sys/param.h themselves, then include nrt/Types.h, and
 * then NRT_MAX_PATH would be set to a different value and cause a mismatch
 * leading to crashes and a lot of confusion.
 */
#      include <sys/param.h>

/*  Typedefs on Unix are a different ball game */
typedef int nrt_IOHandle;
typedef off_t nrt_Off;

typedef void *NRT_DLL_FUNCTION_PTR;
typedef void *NRT_NATIVE_DLL;
typedef int nrt_AccessFlags;
typedef int nrt_CreationFlags;
/*
*  Under Unix, we want to provide default permissions.
*  Some are nice enough to default to 0666, others just
*  burn you, so we make it explicit
*/
#      define  NRT_DEFAULT_PERM  0644
#      define  NRT_INVALID_HANDLE_VALUE -1
#      define  NRT_INVALID_HANDLE(I) ((I) == NRT_INVALID_HANDLE_VALUE)
#      define  NRT_SEEK_CUR             SEEK_CUR
#      define  NRT_SEEK_SET             SEEK_SET
#      define  NRT_SEEK_END             SEEK_END
#      define  NRT_CREATE               O_CREAT
#      define  NRT_TRUNCATE             O_TRUNC
#      define  NRT_OPEN_EXISTING        0
#      define  NRT_ACCESS_READONLY      O_RDONLY
#      define  NRT_ACCESS_WRITEONLY     O_WRONLY
#      define  NRT_ACCESS_READWRITE     O_RDWR

/* type suffixes */
#      define  NRT_INT64(x) x##LL

#      if defined(HAVE_SYS_TIME_H)
#           include <sys/time.h>
#      endif

/*
*  On most systems, this gets included in sys/param.h, but sometimes it's not
*  there, so we make one up.  I figure we can easily get 1K on the stack,
*  so if its not there, it gets set to that.
*  TODO: Is using PATH_MAX out of limits.h more portable?
*        Or to be safer, maybe we should do this check at configure time so
*        there's no way this can change based on what headers someone
*        includes.
*/
#      if defined(MAXPATHLEN)
#          define NRT_MAX_PATH      MAXPATHLEN
#      else
#          define NRT_MAX_PATH      1024
#      endif
#endif

#define NRT_SUCCESS  (1)
#define NRT_FAILURE  (0)
#define NRT_TRUE     (1)
#define NRT_FALSE    (0)


#if defined(__cplusplus) && !defined(SWIGPYTHON)
// "enum class" for C++ w/o SWIG
#define NRT_DECLARE_ENUM(name, ...) enum class name { __VA_ARGS__ }
#else
#define NRT_DECLARE_ENUM(name, ...) typedef enum _ ## name { __VA_ARGS__ } name
#endif

NRT_DECLARE_ENUM(nrt_CornersType,
    NRT_CORNERS_UNKNOWN = -1,
    NRT_CORNERS_UTM,
    NRT_CORNERS_UTM_UPS_S,
    NRT_CORNERS_UTM_UPS_N,
    NRT_CORNERS_GEO,
    NRT_CORNERS_DECIMAL
);

/*
 *  Finally, we determine what kind of system you
 *  are using, and come up with our own swap routines.
 *  This is to avoid the nasty mess that comes with
 *  (Unix actually) the non-standard socket include locations,
 *  and to avoid including sockets just to swap bytes (Ugh).
 */

/* these functions are accessed if needed via the nrt_ntohs
   and nrt_ntohl macros defined in System.h                 */

NRTPROT(uint16_t) nrt_System_swap16(uint16_t ins);
NRTPROT(uint32_t) nrt_System_swap32(uint32_t inl);
NRTPROT(uint64_t) nrt_System_swap64(uint64_t inl);
NRTPROT(uint64_t) nrt_System_swap64c(uint64_t inl);

/* Configure says we are big-endian */
#if defined(__LITTLE_ENDIAN__) || !defined(WORDS_BIGENDIAN)

#       define NRT_NTOHS(x)  nrt_System_swap16 (x)
#       define NRT_HTONS(x)  nrt_System_swap16 (x)

#       define NRT_NTOHL(x)  nrt_System_swap32 (x)
#       define NRT_HTONL(x)  nrt_System_swap32 (x)

#       define NRT_HTONLL(x) nrt_System_swap64(x)
#       define NRT_NTOHLL(x) nrt_System_swap64(x)

#       define NRT_HTONLC(x) nrt_System_swap64c(x)
#       define NRT_NTOHLC(x) nrt_System_swap64c(x)

#   else

#       define NRT_NTOHS(x)  (x)
#       define NRT_HTONS(x)  (x)

#       define NRT_NTOHL(x)  (x)
#       define NRT_HTONL(x)  (x)

#       define NRT_HTONLL(x) (x)
#       define NRT_NTOHLL(x) (x)

#       define NRT_HTONLC(x) (x)
#       define NRT_NTOHLC(x) (X)

#   endif
typedef void NRT_DATA;
#endif
