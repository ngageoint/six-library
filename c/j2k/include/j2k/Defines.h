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

#ifndef __J2K_DEFINES_H__
#define __J2K_DEFINES_H__


#ifdef WIN32
#      if defined(J2K_MODULE_EXPORTS)
#          define NRT_MODULE_EXPORTS
#      elif defined(J2K_MODULE_IMPORTS)
#          define NRT_MODULE_IMPORTS
#      endif
#endif

#include <import/nrt.h>

#define J2K_C               NRT_C
#define J2K_GUARD           NRT_GUARD
#define J2K_ENDGUARD        NRT_ENDGUARD
#define J2K_BOOL            NRT_BOOL
#define J2KAPI(RT)          NRTAPI(RT)
#define J2KPROT(RT)         NRTPROT(RT)
#define J2K_CXX_GUARD       NRT_CXX_GUARD
#define J2K_CXX_ENDGUARD    NRT_CXX_ENDGUARD
#define J2KPRIV             NRTPRIV
#define J2K_FILE            NRT_FILE
#define J2K_LINE            NRT_LINE
#define J2K_FUNC            NRT_FUNC

#define J2K_MALLOC          NRT_MALLOC
#define J2K_REALLOC         NRT_REALLOC
#define J2K_FREE            NRT_FREE

#define J2K_SUCCESS         NRT_SUCCESS
#define J2K_FAILURE         NRT_FAILURE
#define J2K_TRUE            NRT_TRUE
#define J2K_FALSE           NRT_FALSE

typedef NRT_DATA J2K_USER_DATA;

typedef enum _j2k_ImageType
{
    J2K_TYPE_UNKNOWN = 0,
    J2K_TYPE_MONO,
    J2K_TYPE_RGB
} j2k_ImageType;

#endif
