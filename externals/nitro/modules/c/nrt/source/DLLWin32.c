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

#include "nrt/DLL.h"
#include "nrt/Utils.h"

#if defined(WIN32) || defined(_WIN32)

NRTAPI(nrt_DLL *) nrt_DLL_construct(nrt_Error * error)
{
    nrt_DLL *dll = (nrt_DLL *) NRT_MALLOC(sizeof(nrt_DLL));
    if (!dll)
    {
        nrt_Error_init(error, "Failed to alloc DLL", NRT_CTXT, NRT_ERR_MEMORY);

    }
    else
    {
        dll->libname = NULL;
        dll->lib = NULL;
        dll->dsoMain = NULL;
    }
    return dll;
}

NRTAPI(void) nrt_DLL_destruct(nrt_DLL ** dll)
{
    nrt_Error error;
    if (*dll)
    {
        nrt_DLL_unload((*dll), &error);
        if (*dll)
        {
            if ((*dll)->libname)
            {
                NRT_FREE((*dll)->libname);
                (*dll)->libname = NULL;
            }
            NRT_FREE(*dll);
            *dll = NULL;
        }
    }
}

NRTAPI(NRT_BOOL) nrt_DLL_isValid(nrt_DLL * dll)
{
    return (dll->lib != NULL) && (dll->dsoMain == NULL);
}

NRTAPI(NRT_BOOL) nrt_DLL_load(nrt_DLL * dll, const char *libname,
                              nrt_Error * error)
{
    const size_t libname_sz = strlen(libname) + 1;
    dll->libname = (char *) NRT_MALLOC(libname_sz);
    if (!dll->libname)
    {
        nrt_Error_init(error, NRT_STRERROR(NRT_ERRNO), NRT_CTXT,
                       NRT_ERR_MEMORY);
        return NRT_FAILURE;
    }

    strcpy_s(dll->libname, libname_sz, libname);
    dll->lib = LoadLibrary(dll->libname);
    if (!dll->lib)
    {
        nrt_Error_initf(error, NRT_CTXT, NRT_ERR_LOADING_DLL,
                        "Failed to load library [%s]", dll->libname);
        NRT_FREE(dll->libname);
        dll->libname = NULL;
        return NRT_FAILURE;
    }
    dll->dsoMain = NULL;
    return NRT_SUCCESS;
}

NRTAPI(NRT_BOOL) nrt_DLL_unload(nrt_DLL * dll, nrt_Error * error)
{
    if (dll->lib)
    {
        assert(dll->libname);
        NRT_FREE(dll->libname);
        dll->libname = NULL;

        if (!FreeLibrary(dll->lib))
        {
            nrt_Error_initf(error, NRT_CTXT, NRT_ERR_UNLOADING_DLL,
                            "Failed to unload library [%s]", dll->libname);

            return NRT_FAILURE;
        }
        dll->lib = NULL;
    }
    return 1;
}

NRTAPI(NRT_DLL_FUNCTION_PTR) nrt_DLL_retrieve(nrt_DLL * dll,
                                              const char *function,
                                              nrt_Error * error)
{
    /* Make sure we actually have a dll */
    if (dll->lib)
    {
        NRT_DLL_FUNCTION_PTR ptr =
            (NRT_DLL_FUNCTION_PTR) GetProcAddress(dll->lib,
                                                  function);
        /* Now check the resultant value */
        if (ptr == NULL)
        {
            /* Problem if you couldnt produce the function */
            nrt_Error_initf(error, NRT_CTXT, NRT_ERR_RETRIEVING_DLL_HOOK,
                            "Failed to get function [%s] from dll [%s]",
                            function, dll->libname);

        }
        return ptr;
    }

    // This might be a "preloaded" TRE
    if (dll->dsoMain)
    {
        const char* underscore = strchr(function, '_');
        if ((underscore != NULL) && strcmp(underscore, "_handler") == 0)
        {
            return dll->dsoMain;
        }
    }

    nrt_Error_initf(error, NRT_CTXT, NRT_ERR_UNINITIALIZED_DLL_READ,
                    "Failed to retrieve function [%s] -- DLL appears to be uninitialized",
                    function);
    return (NRT_DLL_FUNCTION_PTR) NULL;
}
#endif
