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

#ifndef __NRT_DLL_H__
#define __NRT_DLL_H__

#include "nrt/Defines.h"
#include "nrt/Types.h"
#include "nrt/Memory.h"
#include "nrt/Error.h"

#ifdef WIN32
/*  Under windows, a dynamic shared object is a DLL  */
#    define  NRT_DLL_EXTENSION ".dll"
#else
/*
 *  BE WARY: Under Unix, we expect a DSO to have a .so extension,
 *  in addition to the NRT_PLUGIN_PATH.  That means that,
 *  despite a weak type extension system, your plugin won't
 *  get loaded without an .so extension.
 */
#    define  NRT_DLL_EXTENSION ".so"
#endif

/*!
 *  \struct nrt_DLL
 *  \brief The C structure for loading a DLL
 */
typedef struct _NRT_DLL
{
    char *libname;              /* The name of the library */
    NRT_NATIVE_DLL lib;         /* A handle to the library */
} nrt_DLL;

NRT_CXX_GUARD
/*!
 *  Construct a DLL object
 *  This does nothing more than a memset, and a ZERO,
 *  and then it returns the object
 *  \param error An error object to fill on problem
 *  \return A DLL object, or NULL upon failure
 */
NRTAPI(nrt_DLL *) nrt_DLL_construct(nrt_Error * error);

/*!
 *  Destroy the dll object.  If the lib value is not null,
 *  calls nrt_DLL_unload().  NULLs the pointer
 *  \param dll A double pointer to the object
 */
NRTAPI(void) nrt_DLL_destruct(nrt_DLL ** dll);

/*!
 *  Load the DLL object with an actual library.  This is
 *  the equivalent of LoadLibrary() in windows.
 *  \param dll The DLL to load to
 *  \param libname  The libname to use (this sets the value in the DLL object)
 *  \param error An error object to populate if there is a problem
 *  \return A status to be tested using NRT_FAILURE()
 */
NRTAPI(NRT_BOOL) nrt_DLL_load(nrt_DLL * dll, const char *libname,
                              nrt_Error * error);

/*!
 *  Unload the structure.  This may shrink the executable size.
 *  \param dll The object to unload
 *  \param error An error to be populated on failure
 *  \return A status to be tested using NRT_FAILURE()
 */
NRTAPI(NRT_BOOL) nrt_DLL_unload(nrt_DLL * dll, nrt_Error * error);

/*!
 *  Returns a pointer to a DLL function by the name
 *  \param dll The DLL to retrieve from
 *  \param function The FARPROC function to call
 *  \param error Object to populate on error
 *  \return A pointer to the FARPROC function
 */
NRTAPI(NRT_DLL_FUNCTION_PTR) nrt_DLL_retrieve(nrt_DLL * dll,
                                              const char *function,
                                              nrt_Error * error);

NRTAPI(NRT_BOOL) nrt_DLL_isValid(nrt_DLL * dll);

NRT_CXX_ENDGUARD
#endif
