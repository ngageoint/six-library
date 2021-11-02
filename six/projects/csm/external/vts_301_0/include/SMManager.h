//#############################################################################
//
//    FILENAME:   SMManager.h
//
//    DESCRIPTION:
//
//    Header file for the SMManager class.
//
//    NOTES:
//
//    This class is an example of a simple sensor model (SM) plugin manager.
//    It is responsible for loading all shared objects/DLLs found in the
//    specified directory.
//
//#############################################################################
#ifndef SMMANAGER_H
#define SMMANAGER_H

class SMManager
{
public:

   //---
   // Constructors/Destructor
   //---

   ~SMManager();

   //---
   // Modifiers
   //---

   static SMManager& instance();
      // Returns a handle to the instance of this singleton object.

   //---
   // Accessors
   //---

   static void loadLibraries(const char* dirName);
      // pre: dirName ends with a slash, e.g. "/usr/local/" or "C:\\Temp\\".
      // post: All shared object libraries in path dirName have been opened.

   static bool removePlugin(const char* pluginName);
      // pre: None.
      // post: Removes the first plugin found with name pluginName; returns
      //    true if successful, false otherwise.
      // Note: For now, we assume the plugin name is unique in the list.

private:

   //---
   // Types
   //---

   class Impl;

   //---
   // Constructors/Destructor
   //---

   SMManager();

   //---
   // Data Members
   //---

   Impl *theImpl;

}; // SMManager

#endif // SMMANAGER_H
