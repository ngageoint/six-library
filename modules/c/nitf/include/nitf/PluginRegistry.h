#ifndef __NITF_PLUGIN_REGISTRY_H__
#define __NITF_PLUGIN_REGISTRY_H__

#include "nitf/System.h"
#include "nitf/TRE.h"
#include "nitf/PluginIdentifier.h"


#   define NITF_COMPRESSION_HASH_SIZE 2
#   define NITF_DECOMPRESSION_HASH_SIZE 2

/*  The environment variable for the plugin path  */
#   define NITF_PLUGIN_PATH "NITF_PLUGIN_PATH"

NITF_CXX_GUARD

/*!
 *  \file
 *  \brief Sort out TRE handling plugins
 *
 *  This file is concerned with the plugin API and ensuring
 *  that it works.  For now, the plugins come from the environment
 *  variable, ${NITF_PLUGIN_PATH}
 *
 */
typedef struct _protected_nitf_PluginRegistry
{

    /*  For right now, for simplicity's sake, you   */
    /*  may only designate ONE location for plugins */
    char path[NITF_MAX_PATH];

    /*  Stick the combo of key, DLL in the hash table  */
    nitf_HashTable *treHandlers;
    nitf_HashTable *compressionHandlers;
    nitf_HashTable *decompressionHandlers;

    nitf_List* dsos;

}
nitf_PluginRegistry;


/*!
 *  Since 3/14/2004, this object is a singleton.  If you wish to
 *  create it, you must call this function.  This method checks
 *  its local static pointer to the object (using the double-check
 *  method) and creates it only if no such object exists.  It
 *  loads the plugin registry and returns the object the first time.
 *  Subsequent calls simply return the static object pointer.
 *
 *  By using the double-check method, we avoid costly mutex locking,
 *  in the event that the object has, observably, already been created.
 *  In the unlikely event that we have a race condition, the mutex will
 *  ensure that it is not in an undefined state.
 *
 *  This singleton object is garbage-collected.  Do not attempt to
 *  destroy it, it will auto-destruct.  During construction time,
 *  if the object is created, but the plugin registry load fails, the object
 *  will destroy itself and return NULL.  This is considered a MAJOR
 *  error, which will be probably be unrecoverable.
 *
 *  \return In the event that this function fails, it will return NULL,
 *  along with an error
 *
 */
NITFPROT(nitf_PluginRegistry *)
    nitf_PluginRegistry_getInstance(nitf_Error * error);


/*!
 *  Load the plugin registry.  This will walk the DLL path and search
 *  for plugins.  All DSOs are loaded, and queried for their purpose.
 *  Once this has occurred the object will be in memory, and will be
 *  deletable at nitf_PluginRegistry_unload() time.  Since this is normally
 *  called implicitly, if you use this method, you will need to synchronize
 *  any code that is threaded if you plan on calling these between threads.
 *
 *  \param reg   The registry to load
 *  \param error The error structure to populate on failure
 *  \return 1 on success, 0 on failure
 *
 */
NITFPROT(NITF_BOOL)
    nitf_PluginRegistry_load(nitf_PluginRegistry * reg, nitf_Error * error);


/*!
 *  This function allows you to register your own TRE handlers.  This function
 *  will override any handlers that are currently handling the identifier.
 */
NITFAPI(NITF_BOOL)
nitf_PluginRegistry_registerTREHandler(NITF_PLUGIN_INIT_FUNCTION init,
                                       NITF_PLUGIN_TRE_HANDLER_FUNCTION handler,
                                       nitf_Error* error);
   
/*!
 *  Public function to load the registry with plugins in the given directory.
 *  This will walk the DLL path and search
 *  for plugins.  All DSOs are loaded, and queried for their purpose.
 *  Once this has occurred the object will be in memory, and will be
 *  deletable at nitf_PluginRegistry_unload() time.
 *  This call is thread safe.
 *
 *  \param dirName   The directory to read from and load
 *  \param error The error structure to populate on failure
 *  \return 1 on success, 0 on failure
 *
 */
NITFAPI(NITF_BOOL)
    nitf_PluginRegistry_loadDir(const char* dirName, nitf_Error * error);


NITFAPI(NITF_BOOL)
    nitf_PluginRegistry_loadPlugin(const char* fullPathName, nitf_Error* error);

/*!
 *  Unload the plugin registry.  This will unload the DLLs and free
 *  the nodes in the data structure containing them.  Since this is
 *  normally called implicitly (by the destructor at exit), if you
 *  choose to make this call explicitly, you will need to call it with
 *  locks, if you plan on sharing calls between threads.
 *
 *  \param reg  The registry
 *  \param error An error to populate on failure
 *  \return 1 on success, 0 on failure
 *
 */
NITFPROT(NITF_BOOL)
nitf_PluginRegistry_unload(nitf_PluginRegistry * reg, nitf_Error * error);

/*!
 *  Retrieve the plugin point of entry (NITF_PLUGIN_HANDLER_FUNCTION), so that
 *  the parser or writer can call it.  If no such plugin exists, the
 *  handler will return NULL.  If such a plugin DOES exist, the handler
 *  will return it, unless an error occurred, in which case, it sets
 *  had_error to 1.
 *
 *  \param reg This is the registry
 *  \param ident  This is the ID of the tre (the plugin will have same name)
 *  \param had_error If an error occured, this will be 1, otherwise it is 0
 *  \param error An error to be populated if tre_main is NULL and return val
 *    is -1
 *  \return The plugin main, or NULL
 */
NITFPROT(nitf_TREHandler*)
nitf_PluginRegistry_retrieveTREHandler(nitf_PluginRegistry * reg,
                                       const char *ident,
                                       int *hadError, 
                                       nitf_Error * error);


/*!
 *  Retrieve the plugin point of entry, so that
 *  the image io object can recieve it.  If no such compression plugin,
 *  handler will return NULL.  If such a plugin DOES exist, the handler
 *  will return it, unless an error occurred, in which case, it sets
 *  had_error to 1.
 *
 *  \param reg This is the registry
 *  \param ident  This is the ID (e.g., C8)
 *  \param had_error If an error occured, this will be 1, otherwise it is 0
 *  \param error An error to be populated if tre_main is NULL and return val
 *    is -1
 *  \return The plugin main, or NULL
 */
NITFPROT(NITF_PLUGIN_DECOMPRESSION_CONSTRUCT_FUNCTION)
nitf_PluginRegistry_retrieveDecompConstructor(nitf_PluginRegistry * reg,
                                              const char *ident,
                                              int *hadError,
                                              nitf_Error * error);




NITFPROT(NITF_PLUGIN_COMPRESSION_CONSTRUCT_FUNCTION)
nitf_PluginRegistry_retrieveCompConstructor(nitf_PluginRegistry * reg,
                                            const char *ident,
                                            int *hadError,
                                            nitf_Error * error);




NITF_CXX_ENDGUARD

#endif
