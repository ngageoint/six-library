//#############################################################################
//
//    FILENAME:   TSMPlugin.cpp
//
//    DESCRIPTION:
//
//    This file provides implementation for methods declared in the
//    TSMPlugin class.
//
//    NOTES:
//
//
//    Refer to TSMPlugin.h for more information.
//
//#############################################################################
#define TSM_LIBRARY

#include <algorithm>
#include <iostream>

#ifdef _WIN32          //exports the symbols to be used (KJR)
#  include <windows.h>
#  include "TSMPlugin.h"
#  include "TSMSensorModel.h"
#  include "TSMISDFilename.h"
#  include "TSMISDByteStream.h"
#  include "TSMISDNITF20.h"
#  include "TSMISDNITF21.h"
#else
#  include <pthread.h>
#endif
#include "TSMPlugin.h"
#include "TSMWarning.h"
#include "TSMError.h"

//***************************************************************************
TSMPlugin::TSMPluginList* TSMPlugin::theList = NULL;
TSMPlugin::Impl*          TSMPlugin::theImpl = NULL;

//***************************************************************************
// TSMPlugin::Impl
//***************************************************************************
class TSMPlugin::Impl
{
public:

   //---
   // Modifiers
   //---

   void initializeMutex();
      // pre: None.
      // post: The mutex has been initialized.

   TSMWarning * lockList(void);
      // pre: The list is unlocked.
      // post: The list has been locked.

   TSMWarning * unlockList(void);
      // pre: The list is locked.
      // post: The list has been unlocked.

   //---
   // Data Members
   //---

#ifdef _WIN32
   typedef HANDLE          Mutex;
#else
   typedef pthread_mutex_t Mutex;
#endif

   Mutex mutex;
};

//***************************************************************************
// TSMPlugin::Impl::initializeMutex
//***************************************************************************
void TSMPlugin::Impl::initializeMutex()
{
#ifdef _WIN32
   mutex = CreateMutex(NULL, TRUE, NULL); // TBD: handle errors
#else
   pthread_mutex_init(&mutex, NULL); // TBD: handle errors
#endif
}

//***************************************************************************
// TSMPlugin::Impl::lockList
//***************************************************************************
TSMWarning *TSMPlugin::Impl::lockList(void)
{
#ifdef _WIN32
  WaitForSingleObject(mutex, INFINITE); 
#else
   pthread_mutex_lock(&mutex); // TBD: handle error returns
#endif
   return NULL;
}

//***************************************************************************
// TSMPlugin::Impl::unlockList
//***************************************************************************
TSMWarning *TSMPlugin::Impl::unlockList(void)
{
#ifdef _WIN32
   ReleaseMutex(mutex); // TBD: handle errors
#else
   pthread_mutex_unlock(&mutex); // TBD: handle error returns
#endif
   return NULL;
}

//***************************************************************************
// TSMPlugin::getList
//***************************************************************************
TSMWarning * TSMPlugin::getList(TSMPluginList*&  aTSMPluginList) throw (TSMError)
{
   aTSMPluginList = theList;
   return NULL;
}

//***************************************************************************
// TSMPlugin::findPlugin
//***************************************************************************
TSMWarning *TSMPlugin::findPlugin(const std:: string& pluginName,
                           TSMPlugin*& aTSMPlugin) throw (TSMError)
{
  

  TSMWarning *tsmWarn;
  theImpl->lockList();
  TSMWarning *warn = new TSMWarning();
  TSMPlugin::TSMPluginList* models = NULL;

  try {
    tsmWarn = TSMPlugin::getList(models);
  }
  catch (TSMError *err) {
     std::cout << err->getError() << '\n';
     std::cout << err->getMessage() << '\n';
   }
   catch (...) {
     std::cout << "&&&&& UNKNOWN error thrown by getList\n";
   }

  
  bool found = false;
  for (TSMPluginList::const_iterator i = models->begin();
       i != models->end();
       ++i)
  {
    std::string apluginName;

    try {
      tsmWarn = (*i)->getPluginName(apluginName);
    }
    catch (TSMError *err) {
      std::cout << err->getError() << '\n';
      std::cout << err->getMessage() << '\n';
    }
    catch (...) {
      std::cout << "&&&&& UNKNOWN error thrown by getPluginName\n";
    }

    if (std::string(apluginName) == std::string(pluginName))
      {
	aTSMPlugin = const_cast < TSMPlugin* > (*i);
	found = true;
	break;
      }
  }      

  try {
    tsmWarn = theImpl->unlockList();
  }
  catch (...) {
    std::cout << "&&&&& ERROR thrown by unlockList\n";
  }

  if (!found)
  {
    warn->setTSMWarning (TSMWarning::DATA_NOT_AVAILABLE,
			   "No matching plugin found\n", 
			   "TSMPlugin::findPlugin");
    return warn;
  }
  return tsmWarn;
}

//***************************************************************************
// TSMPlugin::removePlugin
//***************************************************************************
TSMWarning *TSMPlugin::removePlugin(const std::string& pluginName) throw (TSMError)
                             
{
   TSMWarning *tsmWarn;
   TSMPlugin* pluginPtr = NULL;

   TSMError tsmErr;
   std::string myName("removePlugin");
   
 
   try {
     tsmWarn = findPlugin(pluginName, pluginPtr);
   }
   catch (TSMError *err) {
     std::cout << err->getError() << '\n';
     std::cout << err->getMessage() << '\n';
   }
   catch (...) {
     std::cout << "&&&&& UNKNOWN error thrown by findPlugin\n";
   }

   if (pluginPtr !=NULL)
   {
      try {
        tsmWarn = theImpl->lockList();
      }
      catch (...) {
        std::cout << "&&&&& ERROR thrown by lockList\n";
      }

      // find and remove pointer-to-plugin from theList

      TSMPluginList::iterator pos = std::find(theList->begin(),
						theList->end(),
						pluginPtr);
      if (theList->end() != pos)
      {
	theList->erase(pos);
      }
      else
      {
	std::cout << "TSMPlugin::removePlugin: Plugin " << pluginName
		  << " not found" << std::endl;
	// THROW A NOT FOUND EXCEPTION
	tsmErr.setTSMError (
			    TSMError::UNKNOWN_ERROR,
			    "Plugin Name Not Found",
			    myName);
	throw tsmErr;
      }

      try {
	tsmWarn = theImpl->unlockList();
      }
      catch (...) {
        std::cout << "&&&&& ERROR thrown by unlockList\n";
      }
   }
   else
   {
     std::cout << "TSMPlugin::removePlugin: Plugin " << pluginName
              << " not found" << std::endl;
     // THROW A NOT FOUND EXCEPTION
     tsmErr.setTSMError (
			 TSMError::UNKNOWN_ERROR,
			 "Plugin Name Not Found",
			 myName);
     throw tsmErr;

   }
   return tsmWarn;
} // removePlugin

//***************************************************************************
// TSMPlugin::TSMPlugin
//***************************************************************************
TSMPlugin::TSMPlugin()
{
   //---
   // If the list of registered sensor model factories does not exist yet, then
   // create it.
   //---

   if (!theList)
   {
      theList = new TSMPluginList;
   }

   if (!theImpl)
   {
      theImpl = new Impl;
      theImpl->initializeMutex();
   }

   //---
   // If the list of registered sensor model factories exists now (i.e., no
   // error occurred while creating it), then register the plugin factory in
   // theList by adding a pointer to this list.
   //    The pointer points to the static instance of the derived sensor 
   // model plugin.
   //---

   if (theList)
   {
     TSMWarning *tsmWarn = NULL;

     try {
       tsmWarn = theImpl->lockList();
     }
     catch (...) {
       std::cout << "&&&&& ERROR thrown by lockList\n";
     }

     if (tsmWarn == NULL)
       {
         theList->push_back(this);

	 try {
	   tsmWarn = theImpl->unlockList();
	 }
	 catch (...) {
	   std::cout << "&&&&& ERROR thrown by unlockList\n";
	 }

       }
   }
}
