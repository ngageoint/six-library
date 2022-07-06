//#############################################################################
//
//    FILENAME:   SMManager.cpp
//
//    DESCRIPTION:
//
//    This file provides implementation for methods declared in the SMManager
//    class.
//
//    NOTES:
//
//    Refer to SMManager.h for more information.
//
//#############################################################################
#include <iostream>
#include <algorithm>
#include <list>
#include <vector>

#ifdef _WIN32
#include <windows.h>
# include <io.h>
#else
# include <cstdio>  // for sprintf
# include <cstring> // for strstr
# include <dirent.h>
# include <dlfcn.h> // for dlopen, dlclose
# include <errno.h> // for errno
#endif

#include "SMManager.h"
#include "Plugin.h"
#include "Error.h"
#include "Warning.h"

using namespace std;
using namespace csm;

//*****************************************************************************
// SMManager::Impl
//*****************************************************************************
class SMManager::Impl
   //---
   // This inner class is needed to localize the implementation details of the
   // SMManager; in particular, its data members. This allows us to define
   // whether the DllList mentioned below is a list of void* or a list of
   // HANDLEs, without the user of the class caring about the implementation.
   //---
{
public:

   //---
   // Types
   //---

#ifdef _WIN32
   typedef HINSTANCE DllHandle;
#else
   typedef void* DllHandle;
#endif

   typedef std::list < DllHandle > DllList;

   //---
   // Data Members
   //---

   DllList theDlls;

};

//*****************************************************************************
// SMManager::instance
//*****************************************************************************
SMManager& SMManager::instance()
{
   static SMManager singletonObject;

   return singletonObject;
}

//*****************************************************************************
// std::string formatError
//*****************************************************************************
#ifdef _WIN32
std::string formatError(int errorCode)
{
   LPTSTR      errorMessage = NULL;
   std::string result;

   if (0 != FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
                          FORMAT_MESSAGE_FROM_SYSTEM |
                          FORMAT_MESSAGE_IGNORE_INSERTS,
                          NULL,             // Instance
                          errorCode,        // Message Number
                          0,                // Language
                          errorMessage,     // Buffer
                          0,                // Min/Max Buffer size
                          NULL))            // Arguments
   {
      result = std::string(errorMessage);
      // TBD: free errorMessage using LocalFree
   }
   else
   {
      std::cout << "SMManager::formatError: error formatting error message! "
         << "GetLastError returned " << GetLastError() << std::endl;
   }

   return result;
}
#endif // _WIN32

//*****************************************************************************
// SMManager::loadLibraries
//*****************************************************************************
void SMManager::loadLibraries(const char* dirName)
{
#if !defined(NDEBUG)
    fprintf(stderr, "SMManager::loadLibraries(): %s\n", dirName);
#endif

   //---
   // PDL:
   // Go to specified directory.
   // For each file in the directory:
   //    If file is a shared object/DLL, then:
   //       Attempt to "load" the shared object/DLL.
   //       If successful, then:
   //          Add the shared object's/DLL's handle to the list maintained by
   //             the implementation of the singleton instance of this class
   //             (for closing when the program ends).
   //       Else:
   //          Error.
   //       End if.
   //    End if.
   // End for.
   //---

   // Note: The idea behind the singleton instance is this: When loadLibraries
   // is called--one time, or many times--it will get an instance to an object
   // of this class. It will add any DLLs that it opens to the list maintained
   // by this object.
   //    The object will go out of scope when the program ends. At this point,
   // the object's destructor will be called. This destructor is responsible
   // for closing each opened DLL.

#ifdef _WIN32

   TCHAR           szDir[MAX_PATH+1];

   // Get the proper directory path
   sprintf(szDir, "%s\\*.dll", dirName);
//	System.IO.Directory dir;
   WIN32_FIND_DATA FileData;
   HANDLE          hList;

   // Get the first file
   hList = FindFirstFile(szDir, &FileData);
   if (hList == INVALID_HANDLE_VALUE)
   { 
	   int err = GetLastError();
      std::cout << "SMManager: No files found." << " Err = " << err << std::endl;
   }
   else
   {
      // Traverse through the directory structure
      BOOL fFinished = FALSE;
      while (!fFinished)
      {
         // Check the object is a directory or not
         if (! (FileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
         {
            Impl::DllHandle dll = LoadLibrary(FileData.cFileName);
            if (NULL != dll)
            {
               instance().theImpl->theDlls.push_back(dll);
            }
            else
            {
               std::cout << "SMManager:   error: "
                  << formatError(GetLastError()) << std::endl;
            }
         }

         if (!FindNextFile(hList, &FileData))
         {
            if (GetLastError() == ERROR_NO_MORE_FILES)
            {
               fFinished = TRUE;
            }
         }
      }
   }

   FindClose(hList);

#else

   DIR* dfd;
   std::vector<std::string> libList;
   int i;
   
   if (NULL != (dfd = opendir(dirName)))
   {
      dirent* dp;

      while (NULL != (dp = readdir(dfd)))
      {
         char* sopos = strstr(dp->d_name, ".so"); // TBD: This can be refined.

         if (NULL != sopos) 
         {
            libList.push_back(dp->d_name);
         }
      }

      if (0 != closedir(dfd))
      {
         std::cout << "SMManager: Error closing directory (errno=" << errno
            << ')' << std::endl;
      }

      std::sort(libList.begin(), libList.end());
      for(i=0;i<(int)libList.size();i++)
      {
         std::cout << libList[i] << std::endl;
         Impl::DllHandle dll = dlopen((std::string(dirName) +
                                       libList[i]).c_str(),
                                       RTLD_NOW);
            // TBD: Recommend RTLD_NOW | RTLD_GLOBAL (and maybe |
            // RTLD_NODELETE). Need to research which options (current,
            // proposed, or something else from man pages) are appropriate.

         if (NULL != dll)
         {
            instance().theImpl->theDlls.push_back(dll);
         }
         else
         {
            std::cout << "SMManager:   error: " << dlerror() << std::endl;
         }
      }
   }
   else
   {
      std::cout << "SMManager: Unable to open directory " << dirName
         << std::endl;
   }

#endif
}

//*****************************************************************************
// SMManager::removePlugin
//*****************************************************************************
bool SMManager::removePlugin(const char* pluginName)
{
   WarningList warnings;
   
   //csmWarn = Plugin::removePlugin(pluginName);
   Plugin::removePlugin(pluginName, &warnings);
           
   if(warnings.size() == 0)
   //if (!warnings)
   {
      // TBD: remove the shared object/DLL from theImpl->theDlls
      // TBD: close the shared object/DLL
      // TBD: need to research dlclose man page to see the effect if data/
      //      functions from DLL are still being used

     return true;
   }
   else
     std::cout << "SMManager::removePlugin: error removing plugin "
	       << pluginName << std::endl;
   return false;
}

//*****************************************************************************
// SMManager::SMManager
//*****************************************************************************
SMManager::SMManager()
  :
      theImpl(new Impl)
{
   // Do nothing.
}

//*****************************************************************************
// SMManager::~SMManager
//*****************************************************************************
SMManager::~SMManager()
{
   for (Impl::DllList::iterator i = theImpl->theDlls.begin();
        i != theImpl->theDlls.end();
        ++i)
   {
      if(*i)
      {
#ifdef _WIN32
         if(!FreeLibrary(*i))
         {
            std::cout << "~SMManager:    error: " << formatError(GetLastError())
               << std::endl;
         }
#else
         if (0 != dlclose(*i))
         {
            std::cout << "~SMManager:    error: " << dlerror() << std::endl;
         }
#endif
      }
   }

   delete theImpl;
}

size_t SMManager::pluginCount() const
{
    return theImpl->theDlls.size();
}