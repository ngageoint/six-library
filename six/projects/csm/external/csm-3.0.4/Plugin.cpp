//#############################################################################
//
//    FILENAME:          Plugin.cpp
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    This file provides implementation for methods declared in the
//    CSMPlugin class.
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     17-Dec-2012   BAH      Documentation updates.
//
//    NOTES:
// 
//    Refer to Plugin.h for more information.
//
//#############################################################################

#define CSM_LIBRARY

#include "Plugin.h"

#include <algorithm>
#include <iostream>

#ifdef _WIN32          //exports the symbols to be used (KJR)
# include <windows.h>
# include "RasterGM.h"
# include "BytestreamIsd.h"
# include "NitfIsd.h"
#else
# include <pthread.h>
#endif

namespace csm
{

PluginList*   Plugin::theList = NULL;
Plugin::Impl* Plugin::theImpl = NULL;
std::string   Plugin::theDataDir("");

//*****************************************************************************
// Plugin::Impl
//*****************************************************************************
class Plugin::Impl
{
public:
   Impl() : mutex() { initializeMutex(); }

   void lockList(void);
      // pre: The list is unlocked.
      // post: The list has been locked.
   void unlockList(void);
      // pre: The list is locked.
      // post: The list has been unlocked.

   class Locker
   {
   public:
      Locker(Impl* impl) : theImpl(impl) { if (theImpl) theImpl->lockList(); }
      ~Locker() { if (theImpl) theImpl->unlockList(); }

   private:
      Impl* theImpl;
   };

private:
   void initializeMutex();
      // pre: None.
      // post: The mutex has been initialized.

#ifdef _WIN32
   typedef HANDLE          Mutex;
#else
   typedef pthread_mutex_t Mutex;
#endif

   Mutex mutex;
};

//*****************************************************************************
// Plugin::Impl::initializeMutex
//*****************************************************************************
void Plugin::Impl::initializeMutex()
{
#ifdef _WIN32
   mutex = CreateMutex(NULL, FALSE, NULL); // TBD: handle errors
#else
   pthread_mutex_init(&mutex, NULL); // TBD: handle errors
#endif
}

//*****************************************************************************
// Plugin::Impl::lockList
//*****************************************************************************
void Plugin::Impl::lockList(void)
{
#ifdef _WIN32
   WaitForSingleObject(mutex, INFINITE);
#else
   pthread_mutex_lock(&mutex); // TBD: handle error returns
#endif
}

//*****************************************************************************
// Plugin::Impl::unlockList
//*****************************************************************************
void Plugin::Impl::unlockList(void)
{
#ifdef _WIN32
   ReleaseMutex(mutex); // TBD: handle errors
#else
   pthread_mutex_unlock(&mutex); // TBD: handle error returns
#endif
}

//*****************************************************************************
// Plugin::getList
//*****************************************************************************
const PluginList& Plugin::getList()
{
   // create the list if it hasn't already been made
   if (!theList) theList = new PluginList();

   return *theList;
}

//*****************************************************************************
// Plugin::findPlugin
//*****************************************************************************
const Plugin* Plugin::findPlugin(const std:: string& pluginName,
                                 WarningList* warnings)
{
   Impl::Locker locker(impl());

   const PluginList& plugins = getList();

   for (PluginList::const_iterator i = plugins.begin();
        i != plugins.end(); ++i)
   {
      if (pluginName == (*i)->getPluginName())
      {
         return *i;
      }
   }

   // plugin not found
   if (warnings)
   {
      warnings->push_back(Warning(Warning::DATA_NOT_AVAILABLE,
                                  "No matching plugin found",
                                  "Plugin::findPlugin"));
   }

   return NULL;
}

//*****************************************************************************
// Plugin::removePlugin
//*****************************************************************************
void Plugin::removePlugin(const std::string& pluginName,
                          WarningList* warnings)
{
   const Plugin* pluginPtr = findPlugin(pluginName, warnings);
   std::string myName("removePlugin");

   if (pluginPtr != NULL)
   {
      Impl::Locker locker(impl());

      // find and remove pointer-to-plugin from theList
      PluginList::iterator pos = std::find(theList->begin(),
                                           theList->end(),
                                           pluginPtr);
      if (theList->end() == pos)
      {
         throw Error(Error::BOUNDS,
                     "Plugin Name \"" + pluginName + "\" not found",
                     "Plugin::removePlugin");
      }

      theList->erase(pos);
   }

   // if not found, then there's nothing to remove
}

//*****************************************************************************
// Plugin::Plugin
//*****************************************************************************
Plugin::Plugin()
{
   //---
   // If the list of registered sensor model factories does not exist yet, then
   // create it.
   //---
   if (!theList) theList = new PluginList();

   //---
   // If the list of registered sensor model factories exists now (i.e., no
   // error occurred while creating it), then register the plugin factory in
   // theList by adding a pointer to this list.
   //    The pointer points to the static instance of the derived sensor
   // model plugin.
   //---
   if (theList)
   {
      Impl::Locker locker(impl());

      theList->push_back(this);
   }
}

//*****************************************************************************
// Plugin::impl
//*****************************************************************************
Plugin::Impl* Plugin::impl()
{
   // make the singleton if it hasn't already been made
   if (!theImpl) theImpl = new Impl();

   return theImpl;
}

} // namespace csm
