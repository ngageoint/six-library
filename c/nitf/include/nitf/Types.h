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

#   include <assert.h>
#   include <stdlib.h>
#   include <string.h>
#   include <stdio.h>
#   include <math.h>
#   include <stdarg.h>
#   include <sys/types.h>
#   include <ctype.h>
#   include <time.h>

#ifdef WIN32
#      define WIN32_LEAN_AND_MEAN
#      include <windows.h>
#      undef WIN32_LEAN_AND_MEAN

/*  Types are defined for windows here  */
typedef unsigned char     nitf_Uint8;
typedef unsigned __int16  nitf_Uint16;
typedef unsigned __int32  nitf_Uint32;
typedef unsigned __int64  nitf_Uint64;

typedef signed char       nitf_Int8;
typedef __int16           nitf_Int16;
typedef __int32           nitf_Int32;
typedef __int64           nitf_Int64;
typedef HANDLE            nitf_IOHandle;
typedef HINSTANCE         NITF_NATIVE_DLL;
typedef FARPROC           NITF_DLL_FUNCTION_PTR;
typedef DWORD             nitf_AccessFlags;
typedef DWORD             nitf_CreationFlags;
/*  Determine the maximum file path length  */
#      define  NITF_MAX_PATH     MAX_PATH

/* use nitf_Off instead of off_t, since on Windows we want nitf_Int64 used */
typedef nitf_Int64        nitf_Off;

/*  IO macros  */
#      define  NITF_INVALID_HANDLE_VALUE INVALID_HANDLE_VALUE
#      define  NITF_INVALID_HANDLE(I) ((I) == NITF_INVALID_HANDLE_VALUE)
#      define  NITF_SEEK_CUR         FILE_CURRENT
#      define  NITF_SEEK_SET         FILE_BEGIN
#      define  NITF_SEEK_END         FILE_END
#      define  NITF_CREATE           OPEN_ALWAYS
#      define  NITF_OPEN_EXISTING    OPEN_EXISTING
#      define  NITF_TRUNCATE         OPEN_EXISTING
#      define  NITF_ACCESS_READONLY  GENERIC_READ
#      define  NITF_ACCESS_WRITEONLY GENERIC_WRITE
#      define  NITF_ACCESS_READWRITE GENERIC_READ|GENERIC_WRITE

/* type suffixes */
#      define  NITF_INT64(x) x##i64


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


/*  Typedefs on Unix are a different ball game */
typedef uint8_t            nitf_Uint8;
typedef uint16_t           nitf_Uint16;
typedef uint32_t           nitf_Uint32;
typedef uint64_t           nitf_Uint64;

typedef int8_t             nitf_Int8;
typedef int16_t            nitf_Int16;
typedef int32_t            nitf_Int32;
typedef int64_t            nitf_Int64;
typedef int                nitf_IOHandle;
typedef off_t              nitf_Off;

typedef void*             NITF_DLL_FUNCTION_PTR;
typedef void*             NITF_NATIVE_DLL;
typedef int               nitf_AccessFlags;
typedef int               nitf_CreationFlags;
/*
*  Under Unix, we want to provide default permissions.
*  Some are nice enough to default to 0666, others just
*  burn you, so we make it explicit
*/
#      define  NITF_DEFAULT_PERM  0644
#      define  NITF_INVALID_HANDLE_VALUE -1
#      define  NITF_INVALID_HANDLE(I) ((I) == NITF_INVALID_HANDLE_VALUE)
#      define  NITF_SEEK_CUR             SEEK_CUR
#      define  NITF_SEEK_SET             SEEK_SET
#      define  NITF_SEEK_END             SEEK_END
#      define  NITF_CREATE               O_CREAT
#      define  NITF_TRUNCATE             O_TRUNC
#      define  NITF_OPEN_EXISTING        0
#      define  NITF_ACCESS_READONLY      O_RDONLY
#      define  NITF_ACCESS_WRITEONLY     O_WRONLY
#      define  NITF_ACCESS_READWRITE     O_RDWR


/* type suffixes */
#      define  NITF_INT64(x) x##LL

#      if defined(HAVE_SYS_TIME_H)
#           include <sys/time.h>
#      endif

/*
*  On most systems, this gets included in limits.h, but sometimes its not
*  there, so we make one up.  I figure we can easily get 1k on the stack,
*  so if its not there, it gets set to that.
*/
#      if defined(MAXPATHLEN)
#          define NITF_MAX_PATH      MAXPATHLEN
#      else
#          define NITF_MAX_PATH      1024
#      endif
#endif



#   define NITF_SUCCESS  (1)
#   define NITF_FAILURE  (0)


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


/*
 *  Finally, we determine what kind of system you
 *  are using, and come up with our own swap routines.
 *  This is to avoid the nasty mess that comes with
 *  (Unix actually) the non-standard socket include locations,
 *  and to avoid including sockets just to swap bytes (Ugh).
 */

/* these functions are accessed if needed via the nitf_ntohs
   and nitf_ntohl macros defined in System.h                 */

NITFPROT(nitf_Uint16) nitf_System_swap16(nitf_Uint16 ins);
NITFPROT(nitf_Uint32) nitf_System_swap32(nitf_Uint32 inl);
NITFPROT(nitf_Uint64) nitf_System_swap64(nitf_Uint64 inl);
NITFPROT(nitf_Uint32) nitf_System_swap64c(nitf_Uint64 inl);

/* Configure says we are big-endian */
#if defined(__LITTLE_ENDIAN__) || !defined(WORDS_BIGENDIAN)

#       define NITF_NTOHS(x)  nitf_System_swap16 (x)
#       define NITF_HTONS(x)  nitf_System_swap16 (x)

#       define NITF_NTOHL(x)  nitf_System_swap32 (x)
#       define NITF_HTONL(x)  nitf_System_swap32 (x)

#       define NITF_HTONLL(x) nitf_System_swap64(x)
#       define NITF_NTOHLL(x) nitf_System_swap64(x)

#       define NITF_HTONLC(x) nitf_System_swap64c(x)
#       define NITF_NTOHLC(x) nitf_System_swap64c(x)

#   else

#       define NITF_NTOHS(x)  (x)
#       define NITF_HTONS(x)  (x)

#       define NITF_NTOHL(x)  (x)
#       define NITF_HTONL(x)  (x)

#       define NITF_HTONLL(x) (x)
#       define NITF_NTOHLL(x) (x)

#       define NITF_HTONLC(x) (x)
#       define NITF_NTOHLC(x) (X)

#   endif
typedef void NITF_DATA;
#endif
