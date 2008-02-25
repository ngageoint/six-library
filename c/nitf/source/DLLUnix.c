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

#if !defined(WIN32)

#include "nitf/DLL.h"

NITFAPI(nitf_DLL *) nitf_DLL_construct(nitf_Error * error)
{
    nitf_DLL *dll = (nitf_DLL *) NITF_MALLOC(sizeof(nitf_DLL));
    if (!dll)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);

    }
    dll->libname = NULL;
    dll->lib = NULL;
    return dll;
}


NITFAPI(void) nitf_DLL_destruct(nitf_DLL ** dll)
{
    nitf_Error error;
    if (*dll)
    {
        /* destroy the lib */
        nitf_DLL_unload((*dll), &error);
        if ( (*dll)->libname )
        {
            NITF_FREE( (*dll)->libname );
            (*dll)->libname = NULL;
        }
        NITF_FREE(*dll);
        *dll = NULL;
    }
}


NITFAPI(NITF_BOOL) nitf_DLL_isValid(nitf_DLL * dll)
{
    return (dll->lib != (NITF_NATIVE_DLL) NULL);
}


NITFAPI(NITF_BOOL) nitf_DLL_load(nitf_DLL * dll,
                                 const char *libname, nitf_Error * error)
{
    dll->libname = (char*)NITF_MALLOC(strlen(libname) + 1);
    if (!dll->libname)
    {
        nitf_Error_init(error, NITF_STRERROR(NITF_ERRNO), NITF_CTXT, NITF_ERR_MEMORY);
        return NITF_FAILURE;
    }
    strcpy(dll->libname, libname);
    dll->lib = dlopen(libname, RTLD_LAZY);
    if (!dll->lib)
    {
        nitf_Error_init(error, dlerror(), NITF_CTXT, NITF_ERR_LOADING_DLL);
        NITF_FREE( dll->libname );
        dll->libname = NULL;
        return NITF_FAILURE;
    }

    return NITF_SUCCESS;
}


NITFAPI(NITF_BOOL) nitf_DLL_unload(nitf_DLL * dll, nitf_Error * error)
{
    if (dll->lib)
    {
        assert( dll->libname );
        NITF_FREE( dll->libname );
        dll->libname = NULL;
        
        if (dlclose(dll->lib) != 0)
        {
            nitf_Error_init(error, dlerror(),
                            NITF_CTXT, NITF_ERR_UNLOADING_DLL);
            return NITF_FAILURE;
        }
        dll->lib = NULL;
    }
    return NITF_SUCCESS;
}


NITFAPI(NITF_DLL_FUNCTION_PTR) nitf_DLL_retrieve(nitf_DLL * dll,
        const char *function,
        nitf_Error * error)
{
    if (dll->lib)
    {

        NITF_DLL_FUNCTION_PTR ptr = dlsym(dll->lib, function);
        if (ptr == (NITF_DLL_FUNCTION_PTR) NULL)
        {
            /* Problem if you couldnt produce the function */
            nitf_Error_init(error, dlerror(),
                            NITF_CTXT, NITF_ERR_RETRIEVING_DLL_HOOK);
        }
        return ptr;

    }

    /*  You shouldnt be calling it if it didnt load */
    nitf_Error_init(error, dlerror(),
                    NITF_CTXT, NITF_ERR_UNINITIALIZED_DLL_READ);
    return (NITF_DLL_FUNCTION_PTR) NULL;
}
#endif
