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

#ifndef __NITF_DEFINES_H__
#define __NITF_DEFINES_H__

/**
 * Macro which declares a TRE Plugin
 *
 * This assumes that the variable 'descriptions' is an accessible
 * nitf_TRE_DescriptionSet
 *
 * _Tre the name of the input TRE
 */
#ifndef NITF_PLUGIN_FUNCTION_EXPORT
#define NITF_PLUGIN_FUNCTION_EXPORT(retval_) NRTEXPORT(retval_)
#endif
#define NITF_DECLARE_PLUGIN(_Tre) \
    static const char* _Tre##Ident[] = { \
        NITF_PLUGIN_TRE_KEY, \
        #_Tre, \
        NULL \
    }; \
    static nitf_TREHandler _Tre##Handler; \
    NITF_PLUGIN_FUNCTION_EXPORT(const char**) _Tre##_init(nitf_Error* error){ \
       if (!nitf_TREUtils_createBasicHandler(&_Tre##DescriptionSet, &_Tre##Handler,error)) return NULL; \
       return  _Tre##Ident; }					  \
    NITF_PLUGIN_FUNCTION_EXPORT(nitf_TREHandler*) _Tre##_handler(nitf_Error* error) { \
        (void)error; \
        return &_Tre##Handler; \
    }

/**
 * Declare a TRE Plugin with a single TREDescription
 *
 * _Tre the name of the input TRE
 * _Description the description to use
 */
#define NITF_DECLARE_SINGLE_PLUGIN(_Tre, _Description) \
    static nitf_TREDescriptionInfo _Tre##Descriptions[] = { \
        { #_Tre, _Description, NITF_TRE_DESC_NO_LENGTH }, \
        { NULL, NULL, NITF_TRE_DESC_NO_LENGTH } \
    }; \
    static nitf_TREDescriptionSet _Tre##DescriptionSet = { 0, _Tre##Descriptions }; \
    NITF_DECLARE_PLUGIN(_Tre)

#define NITF_DECLARE_SINGLE_PLUGIN_SIMPLE(_Tre) \
    NITF_DECLARE_SINGLE_PLUGIN(_Tre, _Tre##_description)
/**
 * Reference a TRE that has been statically compiled inside of a library
 *
 */

#ifdef __cplusplus
#define NITF_TRE_STATIC_HANDLER_REF(_Tre) \
        extern "C" const char** _Tre##_init(nitf_Error*); \
        extern "C" nitf_TREHandler* _Tre##_handler(nitf_Error*);
#else
#define NITF_TRE_STATIC_HANDLER_REF(_Tre) \
        extern const char** _Tre##_init(nitf_Error*); \
        extern nitf_TREHandler* _Tre##_handler(nitf_Error*);
#endif

#ifdef __cplusplus
#define NITF_COMPRESSION_STATIC_HANDLE_REF(_Compressor) \
        extern "C" const char** _Compressor##_init(nitf_Error*); \
        extern "C" void* C7_construct(const char*, nitf_Error*);
#else
#define NITF_COMPRESSION_STATIC_HANDLE_REF(_Compressor) \
        extern const char** _Compressor##_init(nitf_Error*); \
        extern void* C7_construct(const char*, nitf_Error*);
#endif

#ifdef __cplusplus
#define NITF_J2K_DECOMPRESSION_STATIC_HANDLE_REF(_Decompressor) \
        extern "C" const char** _Decompressor##_init(nitf_Error*); \
        extern "C" void* C8_construct(const char*, nitf_Error*);
#else
#define NITF_J2K_DECOMPRESSION_STATIC_HANDLE_REF(_Decompressor) \
        extern const char** _Decompressor##_init(nitf_Error*); \
        extern void* C8_construct(const char*, nitf_Error*);
#endif

#endif
