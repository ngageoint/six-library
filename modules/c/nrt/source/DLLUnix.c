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

#if !defined(WIN32)

#include "nrt/DLL.h"

NRTAPI(nrt_DLL *) nrt_DLL_construct(nrt_Error * error)
{
    nrt_DLL *dll = (nrt_DLL *) NRT_MALLOC(sizeof(nrt_DLL));
    if (!dll)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);

    }
    dll->libname = NULL;
    dll->lib = NULL;
    return dll;
}

NRTAPI(void) nrt_DLL_destruct(nrt_DLL ** dll)
{
    nrt_Error error;
    if (*dll)
    {
        /* destroy the lib */
        nrt_DLL_unload((*dll), &error);
        if ((*dll)->libname)
        {
            NRT_FREE((*dll)->libname);
            (*dll)->libname = NULL;
        }
        NRT_FREE(*dll);
        *dll = NULL;
    }
}

NRTAPI(NRT_BOOL) nrt_DLL_isValid(nrt_DLL * dll)
{
    return (dll->lib != (NRT_NATIVE_DLL) NULL);
}

NRTAPI(NRT_BOOL) nrt_DLL_load(nrt_DLL * dll, const char *libname,
                              nrt_Error * error)
{
    dll->libname = (char *) NRT_MALLOC(strlen(libname) + 1);
    if (!dll->libname)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        return NRT_FAILURE;
    }
    strcpy(dll->libname, libname);
    dll->lib = dlopen(libname, RTLD_LAZY);
    if (!dll->lib)
    {
        nrt_Error_init(error, dlerror(), NRT_CTXT, NRT_ERR_LOADING_DLL);
        NRT_FREE(dll->libname);
        dll->libname = NULL;
        return NRT_FAILURE;
    }

    return NRT_SUCCESS;
}

NRTAPI(NRT_BOOL) nrt_DLL_unload(nrt_DLL * dll, nrt_Error * error)
{
    if (dll->lib)
    {
        assert(dll->libname);
        NRT_FREE(dll->libname);
        dll->libname = NULL;

        if (dlclose(dll->lib) != 0)
        {
            nrt_Error_init(error, dlerror(), NRT_CTXT, NRT_ERR_UNLOADING_DLL);
            return NRT_FAILURE;
        }
        dll->lib = NULL;
    }
    return NRT_SUCCESS;
}

NRTAPI(NRT_DLL_FUNCTION_PTR) nrt_DLL_retrieve(nrt_DLL * dll,
                                              const char *function,
                                              nrt_Error * error)
{
    if (dll->lib)
    {

        NRT_DLL_FUNCTION_PTR ptr = dlsym(dll->lib, function);
        if (ptr == (NRT_DLL_FUNCTION_PTR) NULL)
        {
            /* Problem if you couldnt produce the function */
            nrt_Error_init(error, dlerror(), NRT_CTXT,
                           NRT_ERR_RETRIEVING_DLL_HOOK);
        }
        return ptr;

    }

    /* You shouldnt be calling it if it didnt load */
    nrt_Error_init(error, dlerror(), NRT_CTXT, NRT_ERR_UNINITIALIZED_DLL_READ);
    return (NRT_DLL_FUNCTION_PTR) NULL;
}
#endif
