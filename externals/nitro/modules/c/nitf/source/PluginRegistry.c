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
 * License along with this program;
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nitf/PluginRegistry.h"

NITFPRIV(nitf_PluginRegistry*) implicitConstruct(nitf_Error* error, FILE* log);
NITFPRIV(void) implicitDestruct(nitf_PluginRegistry** reg);
NITFPRIV(void) exitListener(void);
NITFPRIV(NITF_BOOL)
insertCreator(nitf_DLL* dso,
              nitf_HashTable* handlers,
              const char* ident,
              const char* suffix,
              nitf_Error* error);

static nitf_Mutex __PluginRegistryLock = NITF_MUTEX_INIT;
#if !(defined(WIN32) || defined(_WIN32))
static const char DIR_DELIMITER = '/';
#else
static long __PluginRegistryInitLock = 0;
static const char DIR_DELIMITER = '\\';
#endif
/*
 *  This function retrieves the mutex that is necessary
 *  to establish the singleton in a portable way.
 *
 */

#if defined(WIN32) || defined(_WIN32)
NITFPRIV(nitf_Mutex*) GET_MUTEX(void)
{
    if (__PluginRegistryLock == NULL)
    {
        while (InterlockedExchange(&__PluginRegistryInitLock, 1) == 1)
            /* loop, another thread own the lock */;
        if (__PluginRegistryLock == NULL)
            nitf_Mutex_init(&__PluginRegistryLock);
        InterlockedExchange(&__PluginRegistryInitLock, 0);
    }
    return &__PluginRegistryLock;
}

static NITF_BOOL isDelimiter(char ch)
{
    /* On Windows, allow either delimiter */
    return (ch == '/' || ch == '\\');
}
#else
#define GET_MUTEX() &__PluginRegistryLock

static NITF_BOOL isDelimiter(char ch)
{
    /* On Unix, allow only forward slash */
    return (ch == '/');
}
#endif

/*
 *  Retrieves the singleton instance.  If there is none
 *  already, one is created.
 *
 *  When we create a new registry, we call the load function
 *  in an attempt to load up all DSO references
 *
 */
NITFPROT(nitf_PluginRegistry*)
nitf_PluginRegistry_getInstanceLog(nitf_Error* error, FILE* log)
{
    static nitf_PluginRegistry* theInstance = NULL;

    /*nitf_Mutex mutex = GET_MUTEX();*/
    /*nitf_Mutex_lock(&mutex);*/

    if (theInstance == NULL)
    {
        nitf_Mutex_lock(GET_MUTEX());

        /*  If this call below fails, the error will have been  */
        /*  constructed                                         */
        if (theInstance == NULL)
        {
            theInstance = implicitConstruct(error, log);
            /*  If this succeeded...  */
            if (theInstance)
            {
                const NITF_BOOL loadedOK =
                        nitf_PluginRegistry_load(theInstance, error);
                if (loadedOK)
                {
                    atexit(exitListener);
                }
                else
                {
                    /*  Sorry, no go...  */
                    implicitDestruct(&theInstance);
                }
            }
        }

        nitf_Mutex_unlock(GET_MUTEX());
    }

    return theInstance;
}
NITFPROT(nitf_PluginRegistry*)
nitf_PluginRegistry_getInstance(nitf_Error* error)
{
    return nitf_PluginRegistry_getInstanceLog(error, stderr);
}

NITFPRIV(NITF_BOOL)
insertPlugin(nitf_PluginRegistry* reg,
             const char** ident,
             nitf_DLL* dll,
             nitf_Error* error)
{
    nitf_HashTable* hash = NULL;
    int i;
    int ok;
    const char* suffix = NULL;

    /* Load the DLL */
    if (!nitf_List_pushBack(reg->dsos, dll, error))
    {
        return NITF_FAILURE;
    }

    if (strcmp(ident[0], NITF_PLUGIN_TRE_KEY) == 0)
    {
        hash = reg->treHandlers;
        suffix = NITF_PLUGIN_HOOK_SUFFIX;
    }
    else if (strcmp(ident[0], NITF_PLUGIN_COMPRESSION_KEY) == 0)
    {
        hash = reg->compressionHandlers;
        suffix = NITF_PLUGIN_CONSTRUCT_SUFFIX;
    }
    else if (strcmp(ident[0], NITF_PLUGIN_DECOMPRESSION_KEY) == 0)
    {
        hash = reg->decompressionHandlers;
        suffix = NITF_PLUGIN_CONSTRUCT_SUFFIX;
    }
    else
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "The identity [%s] is not supported",
                         ident[0]);
        return NITF_FAILURE;
    }
    /* Go through each identity and add it as a creator */
    for (i = 1;; i++)
    {
        const char* key = ident[i];

        if (key == NULL)
            break;

        /* no more */
        ok = insertCreator(dll, hash, key, suffix, error);
        if (!ok)
        {
            return NITF_FAILURE;
        }
    }
    return NITF_SUCCESS;
}

static char* nitf_PluginRegistry_getenv(char const* varName)
{
#ifdef _MSC_VER // Visual Studio
#pragma warning(push)
#pragma warning(disable: 4996) // '...' : This function or variable may be unsafe. Consider using ... instead. To disable deprecation, use _CRT_SECURE_NO_WARNINGS. See online help for details.
#endif
    return getenv(varName);
#ifdef _MSC_VER // Visual Studio
#pragma warning(pop)
#endif
}
#define getenv(varName) nitf_PluginRegistry_getenv(varName)

NITFPRIV(nitf_PluginRegistry*) implicitConstruct(nitf_Error* error, FILE* log)
{
    size_t pathLen;
    const char* pluginEnvVar;

    /*  Create the registry object  */
    nitf_PluginRegistry* reg =
            (nitf_PluginRegistry*)NITF_MALLOC(sizeof(nitf_PluginRegistry));

    /*  If we have a memory problem, init our error struct and return  */
    if (!reg)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT,
                        NITF_ERR_MEMORY);
        return NULL;
    }

    /* set these to NULL to possibly protect us later */
    reg->compressionHandlers = NULL;
    reg->treHandlers = NULL;
    reg->decompressionHandlers = NULL;
    reg->dsos = NULL;

    reg->dsos = nitf_List_construct(error);
    if (!reg->dsos)
    {
        implicitDestruct(&reg);
        return NULL;
    }

    /*  Construct our hash object  */
    reg->treHandlers = nitf_HashTable_construct(NITF_TRE_HASH_SIZE, error);

    /*  If we have a problem, get rid of this object and return  */
    if (!reg->treHandlers)
    {
        implicitDestruct(&reg);
        return NULL;
    }

    /* do not adopt the data - we will clean it up ourselves */
    nitf_HashTable_setPolicy(reg->treHandlers, NITF_DATA_RETAIN_OWNER);

    reg->compressionHandlers =
            nitf_HashTable_construct(NITF_COMPRESSION_HASH_SIZE, error);

    /*  If we have a problem, get rid of this object and return  */
    if (!reg->compressionHandlers)
    {
        implicitDestruct(&reg);
        return NULL;
    }

    /* do not adopt the data - we will clean it up ourselves */
    nitf_HashTable_setPolicy(reg->compressionHandlers, NITF_DATA_RETAIN_OWNER);

    reg->decompressionHandlers =
            nitf_HashTable_construct(NITF_DECOMPRESSION_HASH_SIZE, error);

    /*  If we have a problem, get rid of this object and return  */
    if (!reg->decompressionHandlers)
    {
        implicitDestruct(&reg);
        return NULL;
    }

    /* do not adopt the data - we will clean it up ourselves */
    nitf_HashTable_setPolicy(reg->decompressionHandlers,
                             NITF_DATA_RETAIN_OWNER);

    /*  Start with a clean slate  */
    memset(reg->path, 0, NITF_MAX_PATH);

    /*  Take the environment variable, or...  */
    pluginEnvVar = getenv(NITF_PLUGIN_PATH);
    if (!pluginEnvVar)
    {
        /*  Take the default path  */
#ifndef NITF_DEFAULT_PLUGIN_PATH
#define NITF_DEFAULT_PLUGIN_PATH "/putenv/" NITF_PLUGIN_PATH "/" // just to compile ...
#endif
        if (nrt_Directory_exists(NITF_DEFAULT_PLUGIN_PATH))
        {
            nrt_strncpy_s(reg->path, NITF_MAX_PATH, NITF_DEFAULT_PLUGIN_PATH, NITF_MAX_PATH);
            return reg;
        }
        else
        {
            if (log != NULL)
            {
                fprintf(log,
                    "Warning: Unable to find plugin path.\n"
                    "Specify plugin location by setting environment variable "
                    "%s, or by building the library from source\n",
                    NITF_PLUGIN_PATH);
            }
            return reg;
        }
    }
    else
    {
        nrt_strncpy_s(reg->path, NITF_MAX_PATH, pluginEnvVar, NITF_MAX_PATH);
    }
    /*
     * If the we have a user-defined path, they might not
     * have terminated their environment variable with a
     * trailing delimiter.  No problem, we can do it for them.
     */
    pathLen = strlen(reg->path);
    if (pathLen > 0 && !isDelimiter(reg->path[pathLen - 1]))
    {
        /*  Need to append delimiter to end  */
        reg->path[pathLen++] = DIR_DELIMITER;
        reg->path[pathLen++] = '\0';
    }

    /*  Return the object, its okay!  */
    return reg;
}

/*
 *  This function is a garbage-collector.
 *  It is called at exit time.  In the event that the singleton
 *  is properly returned, it destructs it.
 *
 *  Since this function is only registered in the getInstance()
 *  call in the first place, it is presumed to exist already, and
 *  thereby need destruction.
 *
 */
NITFPRIV(void) exitListener(void)
{
    nitf_Error error;
    nitf_Mutex* mutex = GET_MUTEX();
    nitf_PluginRegistry* single = nitf_PluginRegistry_getInstance(&error);
    if (single)
    {
        int unloadRet = nitf_PluginRegistry_unload(single, &error);
        if (unloadRet)
        {
            implicitDestruct(&single);
        }
    }
    nitf_Mutex_delete(mutex);
}

NITFPRIV(void) implicitDestruct(nitf_PluginRegistry** reg)
{
    /*  If it is not NULL set  */
    if (*reg)
    {
        if ((*reg)->dsos)
            nitf_List_destruct(&(*reg)->dsos);

        if ((*reg)->treHandlers)
            nitf_HashTable_destruct(&(*reg)->treHandlers);
        if ((*reg)->compressionHandlers)
            nitf_HashTable_destruct(&(*reg)->compressionHandlers);
        if ((*reg)->decompressionHandlers)
            nitf_HashTable_destruct(&(*reg)->decompressionHandlers);
        NITF_FREE(*reg);
        *reg = NULL;
    }
}

/*
 *  Initialize a DSO.  The init hook is retrieved and called once
 *  when the DSO is loaded
 */

NITFPRIV(const char**)
doInit(nitf_DLL* dll, const char* prefix, nitf_Error* error)
{
    const char** ident;

#define NITF_MAX_PATH_NAME_SIZE_ NITF_MAX_PATH+4096
    char name[NITF_MAX_PATH_NAME_SIZE_];
    memset(name, 0, NITF_MAX_PATH_NAME_SIZE_);
    NITF_SNPRINTF(name, NITF_MAX_PATH_NAME_SIZE_, "%s" NITF_PLUGIN_INIT_SUFFIX, prefix);
    NRT_DLL_FUNCTION_PTR init_ =  nitf_DLL_retrieve(dll, name, error);
    if (!init_)
    {
        nitf_Error_print(error, stdout, "Invalid init hook in DSO");
        return NULL;
    }

    /*  Else, call it  */
    NITF_PLUGIN_INIT_FUNCTION init = (NITF_PLUGIN_INIT_FUNCTION)init_;
    ident = (*init)(error);
    if (!ident)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "The plugin [%s] is not retrievable",
                         prefix);
        return NULL;
    }
    return ident;
}

/*
 *  Cleanup the DLL.  This is called once when the registry is
 *  shut down.
 */
NITFPRIV(int) doCleanup(nitf_DLL* dll, nitf_Error* error)
{
    const char* cleanupName = NITF_PLUGIN_CLEANUP;
    NRT_DLL_FUNCTION_PTR cleanup_ = nitf_DLL_retrieve(dll, cleanupName, error);
    if (!cleanup_)
    {
        return 0;
    }

    NITF_PLUGIN_CLEANUP_FUNCTION cleanup = (NITF_PLUGIN_CLEANUP_FUNCTION)cleanup_;
    /*  Else, call it  */
    cleanup();

    return 1;
}

/*!
 *  Unload the plugin registry.  This will unload the DLLs and free
 *  the nodes in the data structure containing them.
 *
 *  \param reg  The registry
 *  \param error An error to populate on failure
 *  \return 1 on success, 0 on failure
 *
 */
NITFPRIV(NITF_BOOL) unloadDSO(nitf_DLL* dll, nitf_Error* error)
{
    NITF_BOOL ok = NITF_SUCCESS;

    if (nitf_DLL_isValid(dll))
    {
        doCleanup(dll, error);

        /* destroy the lib */
        ok &= nitf_DLL_unload(dll, error);
        if (dll->libname)
        {
#ifdef NITF_DEBUG_PLUGIN_REG
            printf("Unloaded dll with name [%s]\n", dll->libname);
#endif

            NITF_FREE(dll->libname);
            dll->libname = NULL;
        }

        nitf_DLL_destruct(&dll);
    }
    return ok;
}

NITFPROT(NITF_BOOL)
nitf_PluginRegistry_unload(nitf_PluginRegistry* reg, nitf_Error* error)
{
    /*  Pop the front off, until the list is empty  */
    nitf_List* l = reg->dsos;
    NITF_BOOL success = NITF_SUCCESS;
    while (!nitf_List_isEmpty(l))
    {
        nitf_DLL* dso = (nitf_DLL*)nitf_List_popFront(l);
        success &= unloadDSO(dso, error);
    }
    return success;
}

NITFAPI(NITF_BOOL)
nitf_PluginRegistry_loadPlugin(const char* fullName, nitf_Error* error)
{
    /*  For now, the key is the dll name minus the extension  */
    char keyName[NITF_MAX_PATH] = "";
    int ok;
    nitf_DLL* dll;
    const char** ident;
    nitf_PluginRegistry* reg = nitf_PluginRegistry_getInstance(error);

    /*  Construct the DLL object  */
    dll = nitf_DLL_construct(error);
    if (!dll)
    {
        return NITF_FAILURE;
    }
    /*  Otherwise we can load the DLL  */
    if (!nitf_DLL_load(dll, fullName, error))
    {
        /*
         * If the load failed, we have a set error
         *  So all we have to do is close shop, go home
         */
        return NITF_FAILURE;
    }
    nitf_Utils_baseName(keyName, fullName, NITF_DLL_EXTENSION);

    /* Now init the plugin!!!  */
    ident = doInit(dll, keyName, error);

    /*  If no ident, we have a set error and an invalid plugin  */
    if (ident)
    {
        /*  I expect to have problems with this now and then  */
        ok = insertPlugin(reg, ident, dll, error);

        /*  If insertion failed, take our toys and leave  */
        if (!ok)
        {
            return NITF_FAILURE;
        }
#ifdef NITF_DEBUG_PLUGIN_REG
        printf("Successfully loaded plugin: [%s] at [%p]\n", keyName, dll);
#endif
        return NITF_SUCCESS;
    }
    return NITF_FAILURE;
}

NITFAPI(NITF_BOOL)
nitf_PluginRegistry_registerCompressionHandler(
        NITF_PLUGIN_INIT_FUNCTION init,
        NITF_PLUGIN_COMPRESSION_CONSTRUCT_FUNCTION handle,
        nitf_Error* error)
{
    nitf_PluginRegistry* reg = nitf_PluginRegistry_getInstance(error);

    const char** ident;
    int i = 1;
    NITF_BOOL ok = NRT_TRUE;
    if (!reg)
    {
        return NITF_FAILURE;
    }
    if ((ident = (*init)(error)) == NULL)
    {
        return NITF_FAILURE;
    }

    if (!ident[0] || (strcmp(ident[0], NITF_PLUGIN_COMPRESSION_KEY) != 0))
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Expected a Compression identity");
        return NITF_FAILURE;
    }

    for (; ident[i] != NULL; ++i)
    {
#ifdef NITF_DEBUG_PLUGIN_REG
        if (nitf_HashTable_exists(reg->compressionHandlers, ident[i]))
        {
            printf("Warning, static handler overriding [%s] hook\n", ident[i]);
        }
#endif
        ok &= nitf_HashTable_insert(reg->compressionHandlers,
                                    ident[i],
                                    (NITF_DATA*)handle,
                                    error);
    }

    return ok;
}

NITFAPI(NITF_BOOL)
nitf_PluginRegistry_registerDecompressionHandler(
        NITF_PLUGIN_INIT_FUNCTION init,
        NITF_PLUGIN_DECOMPRESSION_CONSTRUCT_FUNCTION handle,
        nitf_Error* error)
{
    nitf_PluginRegistry* reg = nitf_PluginRegistry_getInstance(error);

    const char** ident;
    int i = 1;
    NITF_BOOL ok = NRT_TRUE;
    if (!reg)
    {
        return NITF_FAILURE;
    }
    if ((ident = (*init)(error)) == NULL)
    {
        return NITF_FAILURE;
    }

    if (!ident[0] || (strcmp(ident[0], NITF_PLUGIN_DECOMPRESSION_KEY) != 0))
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Expected a Decompression identity");
        return NITF_FAILURE;
    }

    for (; ident[i] != NULL; ++i)
    {
#ifdef NITF_DEBUG_PLUGIN_REG
        if (nitf_HashTable_exists(reg->decompressionHandlers, ident[i]))
        {
            printf("Warning, static handler overriding [%s] hook\n", ident[i]);
        }
#endif
        ok &= nitf_HashTable_insert(reg->decompressionHandlers,
                                    ident[i],
                                    (NITF_DATA*)handle,
                                    error);
    }

    return ok;
}

NITFAPI(NITF_BOOL)
nitf_PluginRegistry_registerTREHandler(NITF_PLUGIN_INIT_FUNCTION init,
                                       NITF_PLUGIN_TRE_HANDLER_FUNCTION handle,
                                       nitf_Error* error)
{
    nitf_PluginRegistry* reg = nitf_PluginRegistry_getInstance(error);

    const char** ident;
    int i = 1;
    NITF_BOOL ok = NRT_TRUE;
    if (!reg)
    {
        return NITF_FAILURE;
    }
    if ((ident = (*init)(error)) == NULL)
    {
        return NITF_FAILURE;
    }

    if (!ident[0] || (strcmp(ident[0], NITF_PLUGIN_TRE_KEY) != 0))
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "Expected a TRE identity");
        return NITF_FAILURE;
    }

    for (; ident[i] != NULL; ++i)
    {
#ifdef NITF_DEBUG_PLUGIN_REG
        if (nitf_HashTable_exists(reg->treHandlers, ident[i]))
        {
            printf("Warning, static handler overriding [%s] hook\n", ident[i]);
        }
#endif
        ok &= nitf_HashTable_insert(reg->treHandlers,
                                    ident[i],
                                    (NITF_DATA*)handle,
                                    error);
    }

    return ok;
}

NITFPROT(NITF_BOOL)
nitf_PluginRegistry_internalLoadDir(nitf_PluginRegistry* reg,
                                    const char* dirName,
                                    nitf_Error* error)
{
    (void)reg;

    const char* name;
    size_t sizePath;
    nitf_Directory* dir = NULL;

    if (!dirName)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_OPENING_FILE,
                         "Null directory name");
        return NITF_FAILURE;
    }

    dir = nitf_Directory_construct(error);
    if (!dir)
    {
        return NITF_FAILURE;
    }

    sizePath = strlen(dirName);

    if (nitf_Directory_exists(dirName))
    {
        name = nitf_Directory_findFirstFile(dir, dirName);
        if (name)
        {
            do
            {
                char* end;
                char fullName[NITF_MAX_PATH];
                size_t pathSize = sizePath;
                memset(fullName, 0, NITF_MAX_PATH);
                memcpy(fullName, dirName, sizePath);
                if (!isDelimiter(dirName[pathSize - 1]))
                    fullName[pathSize++] = DIR_DELIMITER;
                memcpy(fullName + pathSize, name, strlen(name));

                /*  See if we have .so or .dll extensions  */
                if ((end = (char*)strstr(name, NITF_DLL_EXTENSION)) != NULL)
                {
                    if (!nitf_PluginRegistry_loadPlugin(fullName, error))
                    {
#ifdef NITF_DEBUG_PLUGIN_REG
                        printf("Warning: plugin [%s] failed to load!\n", name);
#endif
                    }
                }

                else
                {
#ifdef NITF_DEBUG_PLUGIN_REG
                    printf("Skipping directory [%s]\n", name);
#endif
                }

                name = nitf_Directory_findNextFile(dir);
            } while (name);
        }
        else
        {
            printf("Error: %s\n", NITF_STRERROR(NITF_ERRNO));
        }
    }
    else
    {
#ifdef NITF_DEBUG_PLUGIN_REG
        fprintf(stdout,
                "Could not open plug-in directory '%s'. "
                "You may have forgotten to set your NITF_PLUGIN_PATH "
                "environment "
                "variable : continuing without plugins...\n",
                dirName);
#endif
    }
    nitf_Directory_destruct(&dir);
    return NITF_SUCCESS;
}

NITFPROT(NITF_BOOL)
nitf_PluginRegistry_internalTREHandlerExists(nitf_PluginRegistry* reg,
                                             const char* ident)
{
    return nitf_HashTable_exists(reg->treHandlers, ident);
}

NITFPROT(NITF_BOOL)
nitf_PluginRegistry_internalCompressionHandlerExists(nitf_PluginRegistry* reg,
                                                     const char* ident)
{
    return nitf_HashTable_exists(reg->compressionHandlers, ident);
}

NITFPROT(NITF_BOOL)
nitf_PluginRegistry_internalDecompressionHandlerExists(nitf_PluginRegistry* reg,
                                                       const char* ident)
{
    return nitf_HashTable_exists(reg->decompressionHandlers, ident);
}

NITFPROT(NITF_BOOL)
nitf_PluginRegistry_internalHandlerExists(
        const char* ident,
        NITF_BOOL handlerCheck(nitf_PluginRegistry*, const char*),
        FILE* log)
{
    NITF_BOOL exists;
    nitf_Error error;

    /* first, get the registry */
    nitf_PluginRegistry* const reg = nitf_PluginRegistry_getInstanceLog(&error, log);
    if (reg == NULL)
    {
        return NITF_FAILURE;
    }

    /* must be thread safe */
    nitf_Mutex_lock(GET_MUTEX());

    exists = handlerCheck(reg, ident);

    /* unlock */
    nitf_Mutex_unlock(GET_MUTEX());

    return exists;
}

NITFPROT(NITF_BOOL)
nitf_PluginRegistry_load(nitf_PluginRegistry* reg, nitf_Error* error)
{
    return nitf_PluginRegistry_internalLoadDir(reg, reg->path, error);
}

NITFAPI(NITF_BOOL)
nitf_PluginRegistry_loadDir(const char* dirName, nitf_Error* error)
{
    NITF_BOOL status;

    /* first, get the registry */
    nitf_PluginRegistry* reg = nitf_PluginRegistry_getInstance(error);

    /* must be thread safe */
    nitf_Mutex_lock(GET_MUTEX());

    status = nitf_PluginRegistry_internalLoadDir(reg, dirName, error);

    /* unlock */
    nitf_Mutex_unlock(GET_MUTEX());
    return status;
}

NITFAPI(NITF_BOOL)
nitf_PluginRegistry_TREHandlerExistsLog(const char* ident, FILE* log)
{
    return nitf_PluginRegistry_internalHandlerExists(
        ident, nitf_PluginRegistry_internalTREHandlerExists, log);
}
NITFAPI(NITF_BOOL)
nitf_PluginRegistry_TREHandlerExists(const char* ident)
{
    return nitf_PluginRegistry_TREHandlerExistsLog(ident, stderr);
}

NITFAPI(NITF_BOOL)
nitf_PluginRegistry_compressionHandlerExists(const char* ident)
{
    return nitf_PluginRegistry_internalHandlerExists(
            ident, nitf_PluginRegistry_internalCompressionHandlerExists, stderr);
}

NITFAPI(NITF_BOOL)
nitf_PluginRegistry_decompressionHandlerExists(const char* ident)
{
    return nitf_PluginRegistry_internalHandlerExists(
            ident, nitf_PluginRegistry_internalDecompressionHandlerExists, stderr);
}

NITFPROT(NITF_PLUGIN_DECOMPRESSION_CONSTRUCT_FUNCTION)
nitf_PluginRegistry_retrieveDecompConstructor(nitf_PluginRegistry* reg,
                                              const char* ident,
                                              int* hadError,
                                              nitf_Error* error)
{
    /*  We get back a pair from the hash table  */
    nitf_Pair* pair;

    /*  No error has occurred (yet)  */
    *hadError = 0;

    if (!nitf_HashTable_exists(reg->decompressionHandlers, ident))
    {
        *hadError = 1;
        nitf_Error_init(error,
                        "Decompression handlers not set",
                        NRT_CTXT,
                        NRT_ERR_DECOMPRESSION);
        return NULL;
    }
    pair = nitf_HashTable_find(reg->decompressionHandlers, ident);

    /*  If nothing is there, we don't have a handler, plain and simple  */
    if (!pair)
    {
        nitf_Error_initf(error,
                         NRT_CTXT,
                         NRT_ERR_DECOMPRESSION,
                         "Don't have a handler for '%s'",
                         ident);
        return NULL;
    }

    return (NITF_PLUGIN_DECOMPRESSION_CONSTRUCT_FUNCTION)pair->data;
}

NITFPROT(NITF_PLUGIN_COMPRESSION_CONSTRUCT_FUNCTION)
nitf_PluginRegistry_retrieveCompConstructor(nitf_PluginRegistry* reg,
                                            const char* ident,
                                            int* hadError,
                                            nitf_Error* error)
{
    /*  We get back a pair from the hash table  */
    nitf_Pair* pair;

    /*  No error has occurred (yet)  */
    *hadError = 0;

    if (!nitf_HashTable_exists(reg->compressionHandlers, ident))
    {
        *hadError = 1;
        nitf_Error_init(error,
                        "Compression handlers not set",
                        NRT_CTXT,
                        NRT_ERR_COMPRESSION);
        return NULL;
    }
    pair = nitf_HashTable_find(reg->compressionHandlers, ident);

    /*  If nothing is there, we don't have a handler, plain and simple  */
    if (!pair)
    {
        nitf_Error_initf(error,
                         NRT_CTXT,
                         NRT_ERR_COMPRESSION,
                         "Don't have a handler for '%s'",
                         ident);
        return NULL;
    }

    return (NITF_PLUGIN_COMPRESSION_CONSTRUCT_FUNCTION)pair->data;
}

NITFPRIV(NITF_BOOL)
insertCreator(nitf_DLL* dso,
              nitf_HashTable* hash,
              const char* ident,
              const char* suffix,
              nitf_Error* error)
{
    /*  Get the name of the handler  */
    char name[NITF_MAX_PATH];

    if (!nitf_DLL_isValid(dso))
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_PARAMETER,
                         "DSO is not valid for [%s]",
                         ident);
    }

    memset(name, 0, NITF_MAX_PATH);
    NITF_SNPRINTF(name, NITF_MAX_PATH, "%s%s", ident, suffix);

    nitf_Utils_replace(name, ' ', '_');

    /*  No error has occurred (yet)  */
#ifdef NITF_DEBUG_PLUGIN_REG
    printf("Loading function [%s] in dso at [%p]\n", name, dso);
#endif

    /*  We are trying to find tre_main  */
    /*  Retrieve the main  */
    NITF_DLL_FUNCTION_PTR dsoMain  = nitf_DLL_retrieve(dso, name, error);

    if (!dsoMain)
    {
        /*  If it didnt work, we are done  */
        return NITF_FAILURE;
    }

#ifdef NITF_DEBUG_PLUGIN_REG
    if (nitf_HashTable_exists(hash, ident))
    {
        printf("Warning, overriding [%s] hook", ident);
    }
#endif

    return nitf_HashTable_insert(hash, ident, (NITF_DATA*)dsoMain, error);
}

/*
 *  Function is now greatly simplified.  We only retrieve TREs from
 *  the hash table.  If they are there, we are good, if not fail
 *
 *  No more talking to the DSOs directly
 */
NITFPROT(nitf_TREHandler*)
nitf_PluginRegistry_retrieveTREHandler(nitf_PluginRegistry* reg,
                                       const char* treIdent,
                                       int* hadError,
                                       nitf_Error* error)
{
    nitf_TREHandler* theHandler;
    /*  We get back a pair from the hash table  */
    nitf_Pair* pair;
    /*  We are trying to find tre_main  */
    NITF_PLUGIN_TRE_HANDLER_FUNCTION treMain = NULL;

    /*  No error has occurred (yet)  */
    *hadError = 0;

    if (!nitf_HashTable_exists(reg->treHandlers, treIdent))
    {
        return NULL;
    }
    /*  Lookup the pair from the hash table, by the tre_id  */
    pair = nitf_HashTable_find(reg->treHandlers, treIdent);

    /*  If nothing is there, we dont have a handler, plain and simple  */
    if (!pair)
        return NULL;

    /*  If something is, get its DLL part  */
    treMain = (NITF_PLUGIN_TRE_HANDLER_FUNCTION)pair->data;

    theHandler = (*treMain)(error);
    if (!theHandler)
    {
        *hadError = 1;
    }
    return theHandler;
}

NITFPROT(nitf_CompressionInterface*)
nitf_PluginRegistry_retrieveCompInterface(const char* comp, nitf_Error* error)
{
    /* In all cases below, the function we're calling should populate the
     * error if one occurs. */
    nitf_PluginRegistry* reg;
    nitf_CompressionInterface* compIface;
    NITF_PLUGIN_COMPRESSION_CONSTRUCT_FUNCTION constructCompIface;
    int hadError = 0;

    /*  Find the compression interface here  */
    reg = nitf_PluginRegistry_getInstance(error);
    if (!reg)
    {
        return NULL;
    }

    /*  Now retrieve the comp iface creator  */
    constructCompIface = nitf_PluginRegistry_retrieveCompConstructor(reg,
                                                                     comp,
                                                                     &hadError,
                                                                     error);
    if (hadError || constructCompIface == NULL)
    {
        return NULL;
    }

    /* Now actually construct it */
    compIface = (nitf_CompressionInterface*)(*constructCompIface)(comp, error);
    if (compIface == NULL)
    {
        return NULL;
    }

    return compIface;
}
