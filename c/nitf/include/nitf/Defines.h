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

#ifndef __NITF_DEFINES_H__
#define __NITF_DEFINES_H__

/* The version of the NITF library */
#define NITF_LIB_VERSION    "2.5"


#ifdef __cplusplus
#   define NITF_C extern "C"
#   define NITF_GUARD {
#   define NITF_ENDGUARD }
#   define NITF_BOOL bool
#else
#   define NITF_C
#   define NITF_GUARD
#   define NITF_ENDGUARD
#   define NITF_BOOL int
#endif

#ifdef WIN32
/*  Negotiate the meaning of NITFAPI, NITFPROT (for public and protected)  */
#      if defined(NITF_MODULE_EXPORTS)
#          define NITFAPI(RT)  NITF_C __declspec(dllexport) RT
#          define NITFPROT(RT) NITF_C __declspec(dllexport) RT
#      elif defined(NITF_MODULE_IMPORTS)
#          define NITFAPI(RT)  NITF_C __declspec(dllimport) RT
#          define NITFPROT(RT) NITF_C __declspec(dllexport) RT
#      else /* Static library */
#          define NITFAPI(RT) NITF_C RT
#          define NITFPROT(RT) NITF_C RT
#      endif
/*  String conversion, on windows  */
#      define  NITF_ATO32(A) strtol(A, (char **)NULL, 10)
#      define  NITF_ATOU32(A) strtoul(A, (char **)NULL, 10)
#      define  NITF_ATOU32_BASE(A,B) strtoul(A, (char **)NULL, B)
#      define  NITF_ATO64(A) _atoi64(A)
#      define  NITF_SNPRINTF _snprintf
#      define  NITF_VSNPRINTF _vsnprintf
#else
/*
*  NITFAPI and NITFPROT don't mean as much on Unix since they
*  Don't try and mangle functions for you in C
*/
#      define  NITFAPI(RT)  NITF_C RT
#      define  NITFPROT(RT) NITF_C RT
#      define  NITF_ATO32(A) strtol(A, (char **)NULL, 10)
#      define  NITF_ATOU32(A) strtoul(A, (char **)NULL, 10)
#      define  NITF_ATOU32_BASE(A,B) strtoul(A, (char **)NULL, B)
#      if defined(__aix__)
#          define  NITF_ATO64(A) strtoll(A, 0, 0)
#      else
#          define  NITF_ATO64(A) atoll(A)
#      endif
#      define NITF_SNPRINTF snprintf
#      define NITF_VSNPRINTF vsnprintf
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

#define NITF_CXX_GUARD     NITF_C NITF_GUARD
#define NITF_CXX_ENDGUARD  NITF_ENDGUARD

/*  Private declarations.  */
#define NITFPRIV(RT) static RT

/*  Negotiate the 'context'  */
#define NITF_FILE __FILE__
#define NITF_LINE __LINE__
#if defined(__GNUC__)
#    define NITF_FUNC __PRETTY_FUNCTION__
#elif __STDC_VERSION__ < 199901
#    define NITF_FUNC "unknown function"
#else /* Should be c99 */
#    define NITF_FUNC __func__
#endif


/**
 * Macro which declares a TRE Plugin
 *
 * This assumes that the variable 'descriptions' is an accessible
 * nitf_TRE_DescriptionSet
 *
 * _Tre the name of the input TRE
 */
#define NITF_DECLARE_PLUGIN(_Tre) \
    static char *ident[] = { \
        NITF_PLUGIN_TRE_KEY, \
        #_Tre, \
        NULL \
    }; \
    static nitf_TREHandler _Tre##Handler; \
    NITFAPI(char**) _Tre##_init(nitf_Error* error){ \
       if (!nitf_TREUtils_createBasicHandler(&descriptionSet,\
                                        &_Tre##Handler,error)) \
       return NULL; return ident; \
    } \
    NITFAPI(nitf_TREHandler*) _Tre##_handler(nitf_Error* error) { \
        return &_Tre##Handler; \
    }

/**
 * Declare a TRE Plugin with a single TREDescription
 *
 * _Tre the name of the input TRE
 * _Description the description to use
 */
#define NITF_DECLARE_SINGLE_PLUGIN(_Tre, _Description) \
    static nitf_TREDescriptionInfo descriptions[] = { \
        { #_Tre, _Description, NITF_TRE_DESC_NO_LENGTH }, \
        { NULL, NULL, NITF_TRE_DESC_NO_LENGTH } \
    }; \
    static nitf_TREDescriptionSet descriptionSet = { 0, descriptions }; \
    NITF_DECLARE_PLUGIN(_Tre)

/**
 * Reference a TRE that has been statically compiled inside of a library
 *
 */

#ifdef __cplusplus
#   define NITF_TRE_STATIC_HANDLER_REF(_Tre) \
        extern "C" char** _Tre##_init(nitf_Error*); \
        extern "C" nitf_TREHandler* _Tre##_handler(nitf_Error*);
#else
#   define NITF_TRE_STATIC_HANDLER_REF(_Tre) \
        extern char** _Tre##_init(nitf_Error*); \
        extern nitf_TREHandler* _Tre##_handler(nitf_Error*);
#endif


#endif
