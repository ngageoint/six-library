/* =========================================================================
 * This file is part of plugin-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * plugin-c++ is free software; you can redistribute it and/or modify
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


#ifndef __PLUGIN_DEFINES_H__
#define __PLUGIN_DEFINES_H__

#include <mem/SharedPtr.h>

/* #define PLUGIN_CONSTRUCTOR_NAME "SpawnPlugin" */
/* #define PLUGIN_DESTRUCTOR_NAME "DestroyPlugin" */
#define GET_PLUGIN_IDENT "GetPluginIdentity"

#ifdef WIN32
#    define PLUGIN_DSO_EXTENSION ".dll"
#else
#    define PLUGIN_DSO_EXTENSION ".so"
#endif

/*
 *  These numbers should be incremented if
 *  the constructor and destructor name ever change
 *  They may be used for the Plugin managers to define
 *  their own versions, but they may also be ignored in favor
 *  of an actual plugin manager version
 */
#define PLUGIN_API_MAJOR_VERSION 1
#define PLUGIN_API_MINOR_VERSION 0


#ifdef WIN32
#      if defined(PLUGIN_MODULE_EXPORTS)
#          define PLUGIN_HOOK extern "C" __declspec(dllexport)
#      else
#          define PLUGIN_HOOK extern "C" __declspec(dllimport)
#      endif
#else
#      define PLUGIN_HOOK extern "C"
#endif

/*
 *  This is absolutely optional, but it does also serve as a good
 *  example for how to expose a plugin to the application
 *
 *  If you do choose to use this magic, you don't want a semi-colon at
 *  the end of the file (where you put this)
 *
 *  If you don't choose to use this magic, you need to define functions
 *  with PLUGIN_CONSTRUCTOR_NAME and PLUGIN_DESTRUCTOR_NAME that are
 *  extern "C" and return pointers to shared pointers to your base classes.
 *  The return type of the functions must be a void* since the functions are
 *  extern C'd - the caller must cast to a pointer to a shared pointer.
 */
#define PLUGIN_EXPOSE_IDENT(IDENT, BASE) \
    PLUGIN_HOOK const void* GetPluginIdentity() { \
        static const mem::SharedPtr<BASE > ident(new IDENT()); \
        return &ident;  \
    }


#define PLUGIN_EXPOSE_IDENT_PRE(IDENT, PRE, BASE) \
    PLUGIN_HOOK const void* PRE##GetPluginIdentity() { \
        static const mem::SharedPtr<BASE > ident(new IDENT()); \
        return &ident;  \
    }

namespace plugin
{
template<typename T> class PluginIdentity
{
public:
    PluginIdentity() {}
    virtual ~PluginIdentity() {}
    //virtual void getOperations(std::vector<std::string>& ops) = 0;
    virtual const char** getOperations() = 0;
    virtual int getMajorVersion() = 0;
    virtual int getMinorVersion() = 0;
    virtual T* spawnHandler() = 0;
    virtual void destroyHandler(T*& handler) = 0;
};
}

#endif
