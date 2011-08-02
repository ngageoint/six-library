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
 * License along with this program; 
 * If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "nitf/PluginRegistry.h"

NITFPRIV(nitf_PluginRegistry *) implicitConstruct(nitf_Error * error);
NITFPRIV(void) implicitDestruct(nitf_PluginRegistry ** reg);
NITFPRIV(void) exitListener(void);
NITFPRIV(NITF_BOOL) insertCreator(nitf_DLL* dso, 
                                  nitf_HashTable* handlers,
                                  const char* ident,
                                  const char* suffix,
                                  nitf_Error* error);

#ifndef WIN32
    static nitf_Mutex  __PluginRegistryLock = NITF_MUTEX_INIT;
    static const char DIR_DELIMITER = '/';
#else
    static nitf_Mutex __PluginRegistryLock = NULL;
    static long __PluginRegistryInitLock = 0;
    static const char DIR_DELIMITER = '\\';
#endif
/*
 *  This function retrieves the mutex that is necessary
 *  to establish the singleton in a portable way.
 *
 */

#ifdef WIN32
NITFPRIV(nitf_Mutex*) GET_MUTEX()
{
    if (__PluginRegistryLock == NULL)
    {
        while (InterlockedExchange(&__PluginRegistryInitLock, 1) == 1)
            /* loop, another thread own the lock */ ;
        if (__PluginRegistryLock == NULL)
            nitf_Mutex_init(&__PluginRegistryLock);
        InterlockedExchange(&__PluginRegistryInitLock, 0);
    }
    return &__PluginRegistryLock;
}

static
NITF_BOOL isDelimiter(char ch)
{
    /* On Windows, allow either delimiter */
    return (ch == '/' || ch == '\\');
}
#else
#define GET_MUTEX() &__PluginRegistryLock

static
NITF_BOOL isDelimiter(char ch)
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
NITFPROT(nitf_PluginRegistry *)
    nitf_PluginRegistry_getInstance(nitf_Error * error)
{
    static nitf_PluginRegistry *theInstance = NULL;

    /*nitf_Mutex mutex = GET_MUTEX();*/
    /*nitf_Mutex_lock(&mutex);*/
    
    if (theInstance == NULL)
    {
        nitf_Mutex_lock( GET_MUTEX());
        
        /*  If this call below fails, the error will have been  */
        /*  constructed                                         */
        if (theInstance == NULL)
        {
            theInstance = implicitConstruct(error);
            /*  If this succeeded...  */
            if (theInstance)
            {
                
                int loadRet = nitf_PluginRegistry_load(theInstance, error);
                /*  If the load failed  */
                if (!loadRet)
                {
                    /*  Sorry, no go...  */
                    implicitDestruct(&theInstance);
                }
                /*  Otherwise register destruct  */
                else
                    atexit(exitListener);
            }
            else
            {
            }
            
        }

        nitf_Mutex_unlock( GET_MUTEX());
    }

    
    return theInstance;
}

NITFPRIV(NITF_BOOL) insertPlugin(nitf_PluginRegistry * reg,
                                 char **ident,
                                 nitf_DLL * dll, 
                                 nitf_Error * error)
{
    nitf_HashTable *hash = NULL;
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
                         "The identity [%s] is not supported", ident[0]);
        return NITF_FAILURE;
    }
    /* Go through each identity and add it as a creator */
    for (i = 1;; i++)
    {
        const char *key = ident[i];

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


NITFPRIV(nitf_PluginRegistry *) implicitConstruct(nitf_Error * error)
{
    size_t pathLen;
    const char *pluginEnvVar;
    
    /*  Create the registry object  */
    nitf_PluginRegistry *reg =
        (nitf_PluginRegistry *) NITF_MALLOC(sizeof(nitf_PluginRegistry));

    /*  If we have a memory problem, init our error struct and return  */
    if (!reg)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
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
        /*strcpy(reg->path, NITF_DEFAULT_PLUGIN_PATH);*/
        return reg;
    }
    else
    {
        strcpy(reg->path, pluginEnvVar);
    }
    /*  
     * If the we have a user-defined path, they might not
     * have terminated their environment variable with a
     * trailing delimiter.  No problem, we can do it for them.
     */
    pathLen = strlen(reg->path);
    if (!isDelimiter(reg->path[pathLen - 1]))
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
    nitf_PluginRegistry *single = nitf_PluginRegistry_getInstance(&error);
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

NITFPRIV(void) implicitDestruct(nitf_PluginRegistry ** reg)
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

NITFPRIV(char **) doInit(nitf_DLL * dll,
                         const char *prefix, nitf_Error * error)
{
    NITF_PLUGIN_INIT_FUNCTION init;
    char **ident;

    char name[NITF_MAX_PATH];
    memset(name, 0, NITF_MAX_PATH);
    NITF_SNPRINTF(name, NITF_MAX_PATH, "%s%s", prefix, NITF_PLUGIN_INIT_SUFFIX);
    init = (NITF_PLUGIN_INIT_FUNCTION) nitf_DLL_retrieve(dll, name, error);
    if (!init)
    {
        nitf_Error_print(error, stdout, "Invalid init hook in DSO");
        return NULL;
    }

    /*  Else, call it  */

    ident = (*init)(error);
    if (!ident)
    {
        nitf_Error_initf(error,
                         NITF_CTXT,
                         NITF_ERR_INVALID_OBJECT,
                         "The plugin [%s] is not retrievable", prefix);
        return NULL;
    }
    return ident;
}


/*
 *  Cleanup the DLL.  This is called once when the registry is
 *  shut down.
 */
NITFPRIV(int) doCleanup(nitf_DLL * dll, nitf_Error* error)
{
    NITF_PLUGIN_CLEANUP_FUNCTION cleanup;
    const char* cleanupName = NITF_PLUGIN_CLEANUP;

    cleanup = (NITF_PLUGIN_CLEANUP_FUNCTION)nitf_DLL_retrieve(dll,
                                                              cleanupName,
                                                              error);
    if (!cleanup)
    {
        return 0;
    }
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
NITFPRIV(NITF_BOOL) unloadDSO(nitf_DLL* dll, nitf_Error * error)
{

    NITF_BOOL ok = NITF_SUCCESS;


    if (nitf_DLL_isValid(dll))
    {
	doCleanup(dll, error);

	/* destroy the lib */
        ok &= nitf_DLL_unload(dll, error);
	if ( dll->libname )
	{

#if NITF_DEBUG_PLUGIN_REG
            printf("Unloaded dll with name [%s]\n", dll->libname);
#endif

	    NITF_FREE( dll->libname );
	    dll->libname = NULL;
	}
	
        nitf_DLL_destruct(&dll);


    }
    return ok;
}




NITFPROT(NITF_BOOL) nitf_PluginRegistry_unload(nitf_PluginRegistry * reg,
        nitf_Error * error)
{

    /*  Pop the front off, until the list is empty  */
    nitf_List* l = reg->dsos;
    NITF_BOOL success = NITF_SUCCESS;
    while ( ! nitf_List_isEmpty(l) )
    {
        nitf_DLL* dso = (nitf_DLL*)nitf_List_popFront(l);
        success &= unloadDSO(dso, error);
    }
    return success;
    
}


NITFAPI(NITF_BOOL)
    nitf_PluginRegistry_loadPlugin(const char* fullName, nitf_Error * error)
{
    
    /*  For now, the key is the dll name minus the extension  */
    char keyName[NITF_MAX_PATH] = "";
    char* p;
    int ok;
    int i, begin, end;
    nitf_DLL *dll;
    char **ident;
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
#if NITF_DEBUG_PLUGIN_REG
        printf("Successfully loaded plugin: [%s] at [%p]\n",
               keyName, dll);
#endif
        return NITF_SUCCESS;
    }
    return NITF_FAILURE;
    
}



NITFAPI(NITF_BOOL)
nitf_PluginRegistry_registerTREHandler(NITF_PLUGIN_INIT_FUNCTION init,
                                       NITF_PLUGIN_TRE_HANDLER_FUNCTION handle,
                                       nitf_Error * error)
{
    
    nitf_PluginRegistry* reg = nitf_PluginRegistry_getInstance(error);

    char** ident;
    int i = 1;
    int ok = 1;
    if (!reg)
    {
        return NITF_FAILURE;
    }
    if ( (ident = (*init)(error)) == NULL)
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
#if NITF_DEBUG_PLUGIN_REG
        if (nitf_HashTable_exists(reg->treHandlers, ident[i]))
        {
            printf("Warning, static handler overriding [%s] hook", ident);
        }
#endif
        ok &= nitf_HashTable_insert(reg->treHandlers, ident[i], (NITF_DATA*)handle, error);
    }

    return ok;

}


NITFPROT(NITF_BOOL) 
    nitf_PluginRegistry_internalLoadDir(nitf_PluginRegistry * reg,
                                        const char *dirName,
                                        nitf_Error * error)
{
    const char *name;
    size_t sizePath;
    nitf_Directory *dir = NULL;
    
    if (!dirName)
    {
        nitf_Error_initf(error, NITF_CTXT, NITF_ERR_OPENING_FILE,
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

                char *end;
                char fullName[NITF_MAX_PATH];
                int pathSize = sizePath;
                memset(fullName, 0, NITF_MAX_PATH);
                memcpy(fullName, dirName, sizePath);
                if (!isDelimiter(dirName[pathSize - 1]))
                    fullName[pathSize++] = DIR_DELIMITER;
                memcpy(fullName + pathSize, name, strlen(name));

                /*  See if we have .so or .dll extensions  */
                if ((end =
                     (char *) strstr(name, NITF_DLL_EXTENSION)) != NULL)
                {
                    if (!nitf_PluginRegistry_loadPlugin(fullName, error))
                    {
#if NITF_DEBUG_PLUGIN_REG
                        printf("Warning: plugin [%s] failed to load!\n", name);
#endif                        
                    }
                }
                
                else
                {
#if NITF_DEBUG_PLUGIN_REG
                    printf("Skipping directory [%s]\n", name);
#endif
                }
                
                name = nitf_Directory_findNextFile(dir);
            }
            while (name);
        }
        else
        {
            printf("Error: %s\n", NITF_STRERROR(NITF_ERRNO));
        }
    }
    else
    {
#if NITF_DEBUG_PLUGIN_REG
        fprintf(stdout,
                "Could not open plug-in directory '%s'. "
                "You may have forgotten to set your NITF_PLUGIN_PATH environment "
                "variable : continuing without plugins...\n", dirName);
#endif
    }
    nitf_Directory_destruct(&dir);
    return NITF_SUCCESS;
}


NITFPROT(NITF_BOOL) nitf_PluginRegistry_load(nitf_PluginRegistry * reg,
                                             nitf_Error * error)
{
    return nitf_PluginRegistry_internalLoadDir(reg, reg->path, error);
}


NITFAPI(NITF_BOOL) nitf_PluginRegistry_loadDir(const char *dirName,
                                               nitf_Error * error)
{
    NITF_BOOL status;
    nitf_Mutex mutex;

    /* first, get the registry */
    nitf_PluginRegistry *reg = nitf_PluginRegistry_getInstance(error);
    
    /* must be thread safe */
    nitf_Mutex_lock( GET_MUTEX() );

    status = nitf_PluginRegistry_internalLoadDir(reg, dirName, error);

    /* unlock */
    nitf_Mutex_unlock( GET_MUTEX() );
    return status;
}



NITFPROT(NITF_PLUGIN_DECOMPRESSION_CONSTRUCT_FUNCTION)
nitf_PluginRegistry_retrieveDecompConstructor(nitf_PluginRegistry * reg,
                                              const char *ident,
                                              int *hadError,
                                              nitf_Error * error)
{
    
    /*  We get back a pair from the hash table  */
    nitf_Pair *pair;
    
    /*  No error has occurred (yet)  */
    *hadError = 0;
    
    if (!nitf_HashTable_exists(reg->decompressionHandlers, ident))
    {
        *hadError = 1;
        return NULL;
    }
    pair = nitf_HashTable_find(reg->decompressionHandlers, ident);
    
    /*  If nothing is there, we dont have a handler, plain and simple  */
    if (!pair)
        return NULL;
    
    return (NITF_PLUGIN_DECOMPRESSION_CONSTRUCT_FUNCTION) pair->data;
}

NITFPROT(NITF_PLUGIN_COMPRESSION_CONSTRUCT_FUNCTION)
nitf_PluginRegistry_retrieveCompConstructor(nitf_PluginRegistry * reg,
                                            const char *ident,
                                            int *hadError,
                                            nitf_Error * error)
{

    /*  We get back a pair from the hash table  */
    nitf_Pair *pair;
    
    /*  No error has occurred (yet)  */
    *hadError = 0;
    
    if (!nitf_HashTable_exists(reg->compressionHandlers, ident))
    {
        *hadError = 1;
        return NULL;
    }
    pair = nitf_HashTable_find(reg->compressionHandlers, ident);
    
    /*  If nothing is there, we dont have a handler, plain and simple  */
    if (!pair)
        return NULL;
    
    return (NITF_PLUGIN_COMPRESSION_CONSTRUCT_FUNCTION) pair->data;
}


NITFPRIV(NITF_BOOL) insertCreator(nitf_DLL* dso, 
                                  nitf_HashTable* hash,
                                  const char* ident,
                                  const char* suffix,
                                  nitf_Error* error)
{
    /*  We are trying to find tre_main  */
    NITF_DLL_FUNCTION_PTR dsoMain = NULL;

    /*  Get the name of the handler  */
    char name[NITF_MAX_PATH];
    char* p = NULL;

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
#if NITF_DEBUG_PLUGIN_REG
    printf("Loading function [%s] in dso at [%p]\n", name, dso);
#endif

    /*  Retrieve the main  */
    dsoMain = nitf_DLL_retrieve(dso, name, error);

    if (!dsoMain)
    {
        /*  If it didnt work, we are done  */
        return NITF_FAILURE;
    }

#if NITF_DEBUG_PLUGIN_REG
    if (nitf_HashTable_exists(hash, ident))
    {
        printf("Warning, overriding [%s] hook", ident);
        
    }
#endif
    
    return nitf_HashTable_insert(hash, ident, dsoMain, error);

}

/*
 *  Function is now greatly simplified.  We only retrieve TREs from
 *  the hash table.  If they are there, we are good, if not fail
 *
 *  No more talking to the DSOs directly
 */
NITFPROT(nitf_TREHandler*)
nitf_PluginRegistry_retrieveTREHandler(nitf_PluginRegistry * reg,
                                       const char *treIdent,
                                       int *hadError, 
                                       nitf_Error * error)
{
    nitf_TREHandler* theHandler;
    /*  We get back a pair from the hash table  */
    nitf_Pair *pair;
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
    treMain = (NITF_PLUGIN_TRE_HANDLER_FUNCTION) pair->data;

    theHandler = (*treMain)(error);
    if (!theHandler)
    {
        *hadError = 1;
    }
    return theHandler;
}


