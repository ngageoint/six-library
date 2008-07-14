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

#include "nitf/PluginRegistry.h"

NITFPRIV(nitf_PluginRegistry *) implicitConstruct(nitf_Error * error);
NITFPRIV(void) implicitDestruct(nitf_PluginRegistry ** reg);
NITFPRIV(void) exitListener(void);

NITFPRIV(nitf_Mutex) getMutex()
{
    
#ifndef WIN32
    /*  This */
    static nitf_Mutex _lock = NITF_MUTEX_INIT;
    
#else
    static nitf_Mutex _lock = NULL;
    static long _initLock = 0;
    
    if (_lock == NULL)
    {
        while (InterlockedExchange(&_initLock, 1) == 1)
            /* loop, another thread own the lock */ ;
        if (_lock == NULL)
            nitf_Mutex_init(&_lock);
        InterlockedExchange(&_initLock, 0);
    }
#endif
    return _lock;
}


/*NITF_MUTEX_STATIC_INITIALIZER( singletonSync );*/

NITFPROT(nitf_PluginRegistry *)
    nitf_PluginRegistry_getInstance(nitf_Error * error)
{
    static nitf_PluginRegistry *theInstance = NULL;
    if (theInstance == NULL)
    {
        nitf_Mutex mutex = getMutex();
        nitf_Mutex_lock(&mutex);
        
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
            
        }
        nitf_Mutex_unlock(&mutex);
        
    }
    
    return theInstance;
}


NITFPRIV(int) insertToHash(nitf_HashTable * hash,
                           const char *key,
                           nitf_DLL * dll, nitf_Error * error)
{
    return nitf_HashTable_insert(hash, key, dll, error);
    
}


NITFPRIV(int) insertPlugin(nitf_PluginRegistry * reg,
                           const char **ident,
                           nitf_DLL * dll, nitf_Error * error)
{
    nitf_HashTable *hash = NULL;
    int i;
    int ok;
    
    if (strcmp(ident[0], NITF_PLUGIN_TRE_KEY) == 0)
    {
        hash = reg->treHandlers;
    }
    else if (strcmp(ident[0], NITF_PLUGIN_COMPRESSION_KEY) == 0)
    {
        hash = reg->compressionHandlers;
    }
    else if (strcmp(ident[0], NITF_PLUGIN_DECOMPRESSION_KEY) == 0)
    {
        hash = reg->decompressionHandlers;
    }
    else
    {
        nitf_Error_init(error,
                        "Other handlers not yet supported!",
                        NITF_CTXT, NITF_ERR_UNK);
        return 0;
    }
    for (i = 1;; i++)
    {
        const char *key = ident[i];

        if (key == NULL)
            break;

        /* no more */
        ok = insertToHash(hash, key, dll, error);
        if (!ok)
        {
            return 0;
        }

    }
    return 1;
}


NITFPRIV(nitf_PluginRegistry *) implicitConstruct(nitf_Error * error)
{

    /*  Create the registry object  */
    nitf_PluginRegistry *reg =
        (nitf_PluginRegistry *) NITF_MALLOC(sizeof(nitf_PluginRegistry));

    const char *pluginEnvVar;
    /*  If we have a memory problem, init our error struct and return  */
    if (!reg)
    {
        nitf_Error_init(error,
                        NITF_STRERROR(NITF_ERRNO),
                        NITF_CTXT, NITF_ERR_MEMORY);
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

    /*nitf_HashTable_setPolicy(reg->treHandlers, NITF_DATA_RETAIN_OWNER);*/

    reg->compressionHandlers =
        nitf_HashTable_construct(NITF_COMPRESSION_HASH_SIZE, error);

    /*  If we have a problem, get rid of this object and return  */
    if (!reg->compressionHandlers)
    {
        implicitDestruct(&reg);
        return NULL;
    }

    /*nitf_HashTable_setPolicy(reg->compressionHandlers, NITF_DATA_RETAIN_OWNER);*/

    reg->decompressionHandlers =
        nitf_HashTable_construct(NITF_DECOMPRESSION_HASH_SIZE, error);

    /*  If we have a problem, get rid of this object and return  */
    if (!reg->decompressionHandlers)
    {
        implicitDestruct(&reg);
        return NULL;
    }

    /*nitf_HashTable_setPolicy(reg->decompressionHandlers, NITF_DATA_RETAIN_OWNER);
*/

    /*nitf_HashTable_setPolicy(reg->desHandlers, NITF_DATA_RETAIN_OWNER);
*/

    /*  Start with a clean slate  */
    memset(reg->path, 0, NITF_MAX_PATH);

    /*  Take the environment variable, or...  */
    pluginEnvVar = getenv(NITF_PLUGIN_PATH);
    if (!pluginEnvVar)
    {
        /*  Take the default path  */
        strcpy(reg->path, NITF_DEFAULT_PLUGIN_PATH);
    }
    else
    {
        strcpy(reg->path, pluginEnvVar);
    }
    /*  If the we have a user-defined path, they might not  */
    /*  have terminated their environment variable with a   */
    /*  trailing slash.  No problem, we can do it for them  */
    if (reg->path[strlen(reg->path) - 1] != '/')
    {
        /*  Need to append / to end  */
        strcat(reg->path, "/");
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
    nitf_Mutex mutex = getMutex();
    nitf_PluginRegistry *single = nitf_PluginRegistry_getInstance(&error);
    if (single)
    {
        int unloadRet = nitf_PluginRegistry_unload(single, &error);
        if (unloadRet)
        {
            implicitDestruct(&single);
        }
    }
    nitf_Mutex_delete(&mutex);
}

NITFPRIV(void) implicitDestruct(nitf_PluginRegistry ** reg)
{
    
    /*  If it is not NULL set  */
    if (*reg)
    {
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


NITFPRIV(const char **) doInit(nitf_DLL * dll,
                               const char *prefix, nitf_Error * error)
{
    NITF_PLUGIN_INIT_FUNCTION init;
    const char **ident;

    char name[NITF_MAX_PATH];
    memset(name, 0, NITF_MAX_PATH);
    sprintf(name, "%s%s", prefix, NITF_PLUGIN_INIT_SUFFIX);
    init = (NITF_PLUGIN_INIT_FUNCTION) nitf_DLL_retrieve(dll, name, error);
    if (!init)
    {
        nitf_Error_print(error, stdout, "FIXME");
        return NULL;
    }

    /*  Else, call it  */

    ident = init(error);
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


NITFPRIV(int) doCleanup(nitf_DLL * dll,
                        const char *prefix, nitf_Error * error)
{
    NITF_PLUGIN_CLEANUP_FUNCTION cleanup;
    char name[NITF_MAX_PATH];
    memset(name, 0, NITF_MAX_PATH);
    sprintf(name, "%s%s", prefix, NITF_PLUGIN_CLEANUP_SUFFIX);

    if (nitf_DLL_isValid(dll))
    {
	cleanup = (NITF_PLUGIN_CLEANUP_FUNCTION) nitf_DLL_retrieve(dll,
								   name,
								   error);
	if (!cleanup)
	{
	    return 0;
	}
	/*  Else, call it  */
	cleanup();
    }
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
NITFPRIV(int) unloadIt(nitf_HashTable * ht, nitf_Pair * pair,
                       NITF_DATA* userData, nitf_Error * error)
{
    nitf_DLL *dll = (nitf_DLL *) pair->data;

    if (nitf_DLL_isValid(dll))
    {
	if (!doCleanup(dll, pair->key, error))
	    return 0;

	/* destroy the lib */
        nitf_DLL_unload(dll, error);
	if ( dll->libname )
	{
	    NITF_FREE( dll->libname );
	    dll->libname = NULL;
	}
	
	/* Don't ever do this!
	   nitf_DLL_destruct(&dll);
	   that way object is still valid
	*/
#if NITF_DEBUG_PLUGIN_REG
    printf("Successfully unloaded dll with key [%s]\n", pair->key);
#endif

    }
    else
    {

	assert(dll->libname == NULL);
	/* We *have* to get rid of this entry now! */
	pair->data = NULL;
#if NITF_DEBUG_PLUGIN_REG
	printf("No need to unload dll with key [%s]\n", pair->key);
#endif
	
    }
    return 1;
}


NITFPROT(NITF_BOOL) nitf_PluginRegistry_unload(nitf_PluginRegistry * reg,
        nitf_Error * error)
{
    NITF_HASH_FUNCTOR fn = unloadIt;
    int rv = 0;
    if (reg->treHandlers)
        rv = nitf_HashTable_foreach(reg->treHandlers, fn, NULL, error);
    if (reg->compressionHandlers)
        rv |= nitf_HashTable_foreach(reg->compressionHandlers, fn, NULL, error);
    if (reg->decompressionHandlers)
        rv |= nitf_HashTable_foreach(reg->decompressionHandlers, fn, NULL, error);
    return (NITF_BOOL) rv;
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
    const char **ident;
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
        /*  If the load failed, we have a set error      */
        /*  So all we have to do is close shop, go home  */
        return NITF_FAILURE;
    }

    begin = 0;
    end = strlen(fullName);
    p = strstr(fullName, NITF_DLL_EXTENSION);
    for (i = 0; i < strlen(fullName); i++)
    {
        if ( *(fullName + i) == '/' || *(fullName + i) == '\\')
            begin = i + 1;

        if ( fullName + i == p)
            end = i - 1;
    }
    memcpy(keyName, &fullName[begin], end - begin + 1);
    end = strstr(keyName, NITF_DLL_EXTENSION);
    keyName[ end - begin + 1] = NULL;
    
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
               name, dll);
#endif
        return NITF_SUCCESS;
    }
    return NITF_FAILURE;
    
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
    
    /* removed this so we don't fail if there aren't any plugins loaded */
    /*if (!nitf_Directory_exists(reg->path))
       {
       nitf_Error_initf(error,
       NITF_CTXT, NITF_ERR_OPENING_FILE,
       "Could not open plug-in directory '%s'.  You may have forgotten to set your NITF_PLUGIN_PATH environment variable",
       reg->path);
       nitf_Directory_destruct(&dir);
       return NITF_FAILURE;
       } */

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
                if (dirName[pathSize - 1] != '/')
                    fullName[pathSize++] = '/';
                memcpy(fullName + pathSize, name, strlen(name));

                /*  See if we have .so or .dll extensions  */
                if ((end =
                     (char *) strstr(name, NITF_DLL_EXTENSION)) != NULL)
                {
                    if (!nitf_PluginRegistry_loadPlugin(fullName, error))
                    {
#if NITF_DEBUG_PLUGIN_REG
                        /*  printf("Here, closing 1\n");  */
                        printf("Warning: plugin [%s] failed to load!\n", name);
#endif                        
                    }
                }
                
                else
                {
#if NITF_DEBUG_PLUGIN_REG
                    /*  printf("Here, closing 1\n");  */
                    printf("Skipping directory [%s]\n", name);
#endif
                    /*return (NITF_BOOL)1; */
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
        fprintf(stdout,
                "Could not open plug-in directory '%s'. "
                "You may have forgotten to set your NITF_PLUGIN_PATH environment "
                "variable : continuing without plugins...\n", dirName);
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
    mutex = getMutex();
    nitf_Mutex_lock(&mutex);

    status = nitf_PluginRegistry_internalLoadDir(reg, dirName, error);

    /* unlock */
    nitf_Mutex_unlock(&mutex);
    return status;
}


NITFPROT(nitf_TREHandler*)
nitf_PluginRegistry_retrieveTREHandler(nitf_PluginRegistry * reg,
                                       const char *tre_id,
                                       int *had_error, nitf_Error * error)
{
    char* p = NULL;

    nitf_TREHandler* theHandler;
	/*  We get back a pair from the hash table  */
    nitf_Pair *pair;
    /*  We get back a DLL from the pair  */
    nitf_DLL *dll;

    /*  We are trying to find tre_main  */
    NITF_PLUGIN_TRE_HANDLER_FUNCTION tre_main = NULL;

    /*  Get the name of the handler  */
    char tre_name[NITF_MAX_PATH];
    memset(tre_name, 0, NITF_MAX_PATH);
    sprintf(tre_name, "%s%s", tre_id, NITF_PLUGIN_HOOK_SUFFIX);

    while( (p = strchr(tre_name, ' ')) != NULL)
    {
        *p = '_';
    }

    /*  No error has occurred (yet)  */
    *had_error = 0;

    /*  Lookup the pair from the hash table, by the tre_id  */
    pair = nitf_HashTable_find(reg->treHandlers, tre_id);

    /*  If nothing is there, we dont have a handler, plain and simple  */
    if (!pair)
        return NULL;

    /*  If something is, get its DLL part  */
    dll = (nitf_DLL *) pair->data;
    if (!nitf_DLL_isValid(dll))
    {
        return NULL;
    }
#if NITF_DEBUG_PLUGIN_REG
    printf("Loading function [%s] in dll at [%p]\n", tre_name, dll);
#endif

    /*  Retrieve the main  */
    tre_main =
        (NITF_PLUGIN_TRE_HANDLER_FUNCTION) nitf_DLL_retrieve(dll, tre_name,
                error);
    if (!tre_main)
    {
        /*  If it didnt work, we are done  */
        *had_error = 1;
        return NULL;
	}
    /*  Yes!!! We did it!!  */
    theHandler = (*tre_main)(error);
	return theHandler;
}
#if 0

NITFPROT(NITF_PLUGIN_TRE_GET_DESCRIPTIONS_FUNCTION)
nitf_PluginRegistry_retrieveTREDescription(nitf_PluginRegistry * reg,
        const char *tre_id,
        int *had_error,
        nitf_Error * error)
{
    /*  We get back a pair from the hash table  */
    nitf_Pair *pair;
    /*  We get back a DLL from the pair  */
    nitf_DLL *dll;

    /*  We are trying to find tre_main  */
    NITF_PLUGIN_TRE_GET_DESCRIPTIONS_FUNCTION tre_main = NULL;

    /*  Get the name of the handler  */
    char tre_name[NITF_MAX_PATH];
    memset(tre_name, 0, NITF_MAX_PATH);
    sprintf(tre_name, "%s%s", tre_id, NITF_PLUGIN_TRE_GET_DESC_SUFFIX);

    /*  No error has occurred (yet)  */
    *had_error = 0;

    /*  Lookup the pair from the hash table, by the tre_id  */
    pair = nitf_HashTable_find(reg->treHandlers, tre_id);

    /*  If nothing is there, we dont have a handler, plain and simple  */
    if (!pair)
        return NULL;

    /*  If something is, get its DLL part  */
    dll = (nitf_DLL *) pair->data;
    if (!nitf_DLL_isValid(dll))
    {
        return NULL;
    }
#if NITF_DEBUG_PLUGIN_REG
    printf("Loading function [%s] in dll at [%p]\n", tre_name, dll);
#endif

    /*  Retrieve the main  */
    tre_main =
        (NITF_PLUGIN_TRE_GET_DESCRIPTIONS_FUNCTION) nitf_DLL_retrieve(dll,
                tre_name,
                error);
    if (!tre_main)
    {
        /*  If it didnt work, we are done  */
        *had_error = 1;
        return NULL;
    }
    /*  Yes!!! We did it!!  */
    return tre_main;
}



NITFPROT(NITF_PLUGIN_TRE_SET_DESCRIPTION_FUNCTION)
nitf_PluginRegistry_retrieveSetTREDescription(nitf_PluginRegistry * reg,
        const char *tre_id,
        int *had_error,
        nitf_Error * error)
{
    /*  We get back a pair from the hash table  */
    nitf_Pair *pair;
    /*  We get back a DLL from the pair  */
    nitf_DLL *dll;

    /*  We are trying to find tre_main  */
    NITF_PLUGIN_TRE_SET_DESCRIPTION_FUNCTION tre_main = NULL;

    /*  Get the name of the handler  */
    char tre_name[NITF_MAX_PATH];
    memset(tre_name, 0, NITF_MAX_PATH);
    sprintf(tre_name, "%s%s", tre_id, NITF_PLUGIN_TRE_SET_DESC_SUFFIX);

    /*  No error has occurred (yet)  */
    *had_error = 0;

    /*  Lookup the pair from the hash table, by the tre_id  */
    pair = nitf_HashTable_find(reg->treHandlers, tre_id);

    /*  If nothing is there, we dont have a handler, plain and simple  */
    if (!pair)
        return NULL;

    /*  If something is, get its DLL part  */
    dll = (nitf_DLL *) pair->data;
    if (!nitf_DLL_isValid(dll))
    {
        return NULL;
    }


    /*  Retrieve the main  */
    tre_main =
        (NITF_PLUGIN_TRE_SET_DESCRIPTION_FUNCTION) nitf_DLL_retrieve(dll,
                tre_name,
                error);
    if (!tre_main)
    {
        /*  If it didnt work, we are done  */
        *had_error = 1;
        return NULL;
    }
    /*  Yes!!! We did it!!  */
    return tre_main;
}

#endif
NITFPROT(NITF_PLUGIN_DECOMPRESSION_CONSTRUCT_FUNCTION)
nitf_PluginRegistry_retrieveDecompConstructor(nitf_PluginRegistry * reg,
        const char *comp_id,
        int *had_error,
        nitf_Error * error)
{
    /*  We get back a pair from the hash table  */
    nitf_Pair *pair;
    /*  We get back a DLL from the pair  */
    nitf_DLL *dll;

    /*  We are trying to find decomp_constructor  */
    NITF_PLUGIN_DECOMPRESSION_CONSTRUCT_FUNCTION decomp_constructor = NULL;

    /*  Get the name of the handler  */
    char decomp_name[NITF_MAX_PATH];
    memset(decomp_name, 0, NITF_MAX_PATH);
    sprintf(decomp_name, "%s%s", comp_id, NITF_PLUGIN_CONSTRUCT_SUFFIX);

    /*  No error has occurred (yet)  */
    *had_error = 0;

    /*  Lookup the pair from the hash table, by the decomp_id  */
    pair = nitf_HashTable_find(reg->decompressionHandlers, comp_id);

    /*  If nothing is there, we dont have a handler, plain and simple  */
    if (!pair)
        return NULL;

    /*  If something is, get its DLL part  */
    dll = (nitf_DLL *) pair->data;
    if (!nitf_DLL_isValid(dll))
    {
        return NULL;
    }
#if NITF_DEBUG_PLUGIN_REG
    printf("Loading function [%s] in dll at [%p]\n", decomp_name, dll);
#endif

    /*  Retrieve the main  */
    decomp_constructor =
        (NITF_PLUGIN_DECOMPRESSION_CONSTRUCT_FUNCTION)
        nitf_DLL_retrieve(dll, decomp_name, error);
    if (!decomp_constructor)
    {
        /*  If it didnt work, we are done  */
        *had_error = 1;
        return NULL;
    }
    /*  Yes!!! We did it!!  */
    return decomp_constructor;
}

