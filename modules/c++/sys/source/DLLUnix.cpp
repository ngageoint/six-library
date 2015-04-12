/* =========================================================================
 * This file is part of sys-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * sys-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not, 
 * see <http://www.gnu.org/licenses/>.
 *
 */


#if !defined(WIN32)

#include "sys/DLL.h"

void sys::DLLException::adjustMessage()
{
    if (!mMessage.empty() )
    {
        mMessage += ": ";
    }
    mMessage += dlerror();
}

void sys::DLL::load(const std::string& libName)
{
    // First we get the library name in case you ask
    mLibName = libName;

    // Next we load the library
    mLib = dlopen( mLibName.c_str() , DLL_FLAGSET);

    // Now we check the return value
    if (!mLib)
        throw(sys::DLLException("Failed to load() DLL") );

}

void sys::DLL::unload()
{
    // First we check to see if it's unloaded
    if (mLib)
    {
        // Next we unload it or raise an exception
        if (dlclose( mLib ) != 0)
            throw(sys::DLLException("Failed to close library") );

        // Now we reset member data
        mLib = NULL;
        mLibName.clear();
    }
}
DLL_FUNCTION_PTR
sys::DLL::retrieve(const std::string& functionName)
{
    // Check to make sure we have a library
    if ( mLib != NULL )
    {
        // Now we get a ptr
        DLL_FUNCTION_PTR ptr = dlsym( mLib , functionName.c_str());

        // Now we check the ptr value
        if (ptr == NULL)
            throw(sys::DLLException("Failed to load function"));

        return ptr;
    }
    else
    {
        throw(sys::DLLException("No library loaded") );
    };
}

#endif
