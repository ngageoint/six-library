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

#ifndef __NRT_DEFINES_H__
#define __NRT_DEFINES_H__
#pragma once

/* The version of the NRT library */

#ifdef __cplusplus
#   define NRT_C extern "C"
#   define NRT_GUARD {
#   define NRT_ENDGUARD }
#   define NRT_BOOL bool
#else
#   define NRT_C
#   define NRT_GUARD
#   define NRT_ENDGUARD
#   define NRT_BOOL int
#endif

#include "nrt/Exports.h"

#if defined(WIN32) || defined(_WIN32)
/*  Negotiate the meaning of NRTAPI, NRTPROT (for public and protected)  */
#      if defined(NRT_MODULE_EXPORTS)
#          define NRTAPI(RT)  NRT_C __declspec(dllexport) RT
#          define NRTPROT(RT) NRT_C __declspec(dllexport) RT
#      elif defined(NRT_MODULE_IMPORTS)
#          define NRTAPI(RT)  NRT_C __declspec(dllimport) RT
#          define NRTPROT(RT) NRT_C __declspec(dllexport) RT
#      else                     /* Static library */
#          define NRTAPI(RT) NRT_C RT
#          define NRTPROT(RT) NRT_C RT
#      endif
/*  String conversion, on windows  */
#      define  NRT_ATO32(A) strtol(A, (char **)NULL, 10)
#      define  NRT_ATOU32(A) strtoul(A, (char **)NULL, 10)
#      define  NRT_ATOU32_BASE(A,B) strtoul(A, (char **)NULL, B)
#      define  NRT_ATO64(A) _atoi64(A)
#else
/*
*  NRTAPI and NRTPROT don't mean as much on Unix since they
*  Don't try and mangle functions for you in C
*/
#      define  NRTAPI(RT)  NRT_C RT
#      define  NRTPROT(RT) NRT_C RT
#      define  NRT_ATO32(A) strtol(A, (char **)NULL, 10)
#      define  NRT_ATOU32(A) strtoul(A, (char **)NULL, 10)
#      define  NRT_ATOU32_BASE(A,B) strtoul(A, (char **)NULL, B)
#      if defined(__aix__)
#          define  NRT_ATO64(A) strtoll(A, 0, 0)
#      else
#          define  NRT_ATO64(A) atoll(A)
#      endif
#endif
#define NRT_SNPRINTF snprintf
#define NRT_VSNPRINTF vsnprintf

// Export no matter what ... when you KNOW you're building a DLL/SO, e.g. a plugin
#define NRTEXPORT(RT) NRT_C NITRO_NRT_library_export RT // extern "C" __declspec(dllexport) void* foo();

// Adjust the above ... but w/o chaning too much :-(
#if defined(NRTAPI) && defined(CODA_OSS_LIBRARY_SHARED)
	#undef NRTAPI
	#ifdef NITRO_NRT_EXPORTS  // See Exports.h
		#define NRTAPI(RT) NRTEXPORT(RT)
	#else
		#define NRTAPI(RT) NRT_C NITRO_NRT_library_import RT // extern "C" __declspec(dllimport) void* foo();		
	#endif
#endif

/*
 *  This section describes a set of macros to help with
 *  C++ compilation.  The 'extern C' set is required to
 *  prevent name-mangling.
 *
 *  We also define a boolean according to the language,
 *  since it is obnoxious to use an int when C++ has a
 *  perfectly good one already.
 */

#define NRT_CXX_GUARD     NRT_C NRT_GUARD
#define NRT_CXX_ENDGUARD  NRT_ENDGUARD

/*  Private declarations.  */
#define NRTPRIV(RT) static RT

/*  Negotiate the 'context'  */
#define NRT_FILE __FILE__
#define NRT_LINE __LINE__
#if defined(__GNUC__)
#    define NRT_FUNC __PRETTY_FUNCTION__
#elif defined(__STDC_VERSION__) && (__STDC_VERSION__ < 199901)
#    define NRT_FUNC "unknown function"
#else                           /* Should be c99 */
#    define NRT_FUNC __func__
#endif

#endif
