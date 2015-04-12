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


#ifndef __SYS_DLL_H__
#define __SYS_DLL_H__


/*!
 *  \file DLL.h
 *  \brief Load DLL's using an object, in a system-independent fashion
 *
 *  DLL's are often painful to use, especially in a cross-platform manner.
 *  This code supports the dl API under UNIX and the windows DLL API.
 *
 *  It allows users to conjure up DLL code and retrieve functions from
 *  that library.  For more information, see the CodingWithDLLsGuide
 *
 */

# if defined(WIN32)
#   include "sys/Conf.h"
#   define DLL_PUBLIC_FUNCTION extern "C" __declspec (dllexport)
typedef HINSTANCE DYNAMIC_LIBRARY;
typedef FARPROC DLL_FUNCTION_PTR;
# else
#   include <dlfcn.h>
#   define DLL_FLAGSET RTLD_LAZY
#   define DLL_PUBLIC_FUNCTION extern "C"
typedef void* DYNAMIC_LIBRARY;
typedef void* DLL_FUNCTION_PTR;
# endif

#include "except/Exception.h"
#include "sys/Err.h"


namespace sys
{

/*!
 *  \class DLLException
 *  \brief Exception class for DLLs
 *
 *  This class behaves like its parent, except that it
 *  appends the system or DLL API error that caused the exception
 */
class DLLException : public except::Exception
{
public:
    /*!
     *  Constructor.  
     */
    DLLException()
    {
        adjustMessage();
    }

    /*!
     *  Construct from a base message.  Appends the system-specific
     *  dll error
     *  \param message The base message
     */
    DLLException(const char* message) :
            except::Exception(message)
    {
        adjustMessage();
    }

    /*!
     *  Construct from a base message.  Appends the system-specific
     *  dll error
     *  \param message The base message
     */
    DLLException(const std::string& message) :
            except::Exception(message)
    {
        adjustMessage();
    }

    //!  Destructor
    virtual ~DLLException()
    {}

    /*!
     *  This tacks on the system or API error message
     *  to the original base message.
     */
    virtual void adjustMessage();
};

/*!
 *  \class DLL
 *  \brief Dynamically link a library in a system-independent fashion
 *
 *  This is a class to load a DLL (.dll or .so) at runtime.  This
 *  uses the dl API for unix and the windows DLL stuff.  Here is an
 *  example of DLL usage:
 *  
 *   DLL dll( "my_dll" );
 *   dll.retrieve( "my_function" )();
 *   DLL_FUNCTION_PTR ptr = dll.retrieve( "my_function_2" );
 *   ptr();
 *
 *  For more information of coding with dll's see the 
 *  CodingWithDLLsGuide
 */


class DLL
{
public:

    /*!
     *  Construct a library object, but dont populate or load it
     *  If you use this method, you must call load() separately
     */
    DLL();
    /*!
     *  Construct a load a libray object.  Dont call load() if you
     *  have called this constructor, unless you unload() first
     *  \param libName The name of the library
     */
    DLL( const std::string& libName )
    {
        load( libName );
    }
    /*!
     *  Destructor.  This auto-closes the DLL if you didn't call
     *  unload() explicitly.
     *
     */
    virtual ~DLL()
    {
        if ( mLib )
        {
            try
            {
                unload();
            }
            catch (...)
            {
            }
        }
    }

    /*!
     *  Load the library by the name given.
     *  This also sets the internal member mLibName.
     *  \param libName  The library name to load
     */
    void load( const std::string& libName );

    /*!
     *  Unload the library explicitly.  This resets
     *  the member fields.
     */
    void unload();

    /*!
     *  Retrieve a function pointer by function name from
     *  a previously loaded library.  The libray must have
     *  been previously loaded using the load() function, either
     *  directly or indirectly.
     *  \param functionName The name of the function to retrieve
     *
     */
    DLL_FUNCTION_PTR
    retrieve( const std::string& functionName );

    /*!
     *  Get the name of the library that is currently loaded
     *  \return The library name
     */
    std::string getLibName() const
    {
        return mLibName;
    }

protected:
    //!  The library by name
    std::string mLibName;

    //!  The library by handle
    DYNAMIC_LIBRARY mLib;

};


}

#endif
