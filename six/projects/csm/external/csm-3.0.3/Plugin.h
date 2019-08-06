//#############################################################################
//
//    FILENAME:          Plugin.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for abstract base class that is to provide a common interface from
//    which all Community Sensor Model (CSM) plugin factories shall inherit.
//
//    To use this for a plugin, the developer must simply inherit from this
//    class providing, at least, implementation for each pure virtual method.
//    In order to allow the plugin to self-register itself and be recognized
//    by the system as a "plugin", a static instance of the derived class must
//    invoke the Plugin constructor.
//
//    LIMITATIONS:       None
//
//    SOFTWARE HISTORY:
//
//     Date          Author   Comment
//     -----------   ------   -------
//     May-2003      J. Olson Received initial version from BAE.
//     20-Jun-2003   KFM      Revised to incorporate plugin list and automatic
//                            registration for derived types.
//     01-Jul-2003   KFM      Updated signatures.
//     06-Feb-2004   KRW      Incorporates changes approved by
//                            the January and February 2004
//                            configuration control board.
//     08-JUN-2004   TWC      API 3.1
//     19-Aug-2004   PW       Add throws
//     02-Mar-2012   SCM      Refactor interface.
//     21-Jun-2012   SCM      Added data directory.
//     26-Sep-2012   SCM      Changed getModelVersion() to return a
//                            csm::Version object
//     30-Oct-2012   SCM      Renamed to Plugin.h
//     06-Dec-2012   JPK      Renamed getCSMVersion() to getCsmVersion()
//     17-Dec-2012   BAH      Documentation updates.
//
//#############################################################################

#ifndef __CSM_PLUGIN_H
#define __CSM_PLUGIN_H

#include "csm.h"
#include "Isd.h"
#include "Error.h"
#include "Warning.h"
#include "Version.h"

#include <list>
#include <string>

namespace csm
{

class Model;
class Plugin;

typedef std::list<const Plugin*> PluginList;

class CSM_EXPORT_API Plugin
{
public:
   virtual ~Plugin() {}
   //--------------------------------------------------------------------------
   // List Managing Methods
   //--------------------------------------------------------------------------
   static const PluginList& getList();
      //> This method provides access to the list of all plugins that are
      //  currently registered.
      //<

   static const Plugin* findPlugin(const std::string& pluginName,
                                   WarningList* warnings = NULL);
      //> Returns a pointer to the first plugin found whose name is
      //  pluginName; returns NULL if no such plugin found.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   static void removePlugin(const std::string& pluginName,
                            WarningList* warnings = NULL);
      //> This method attempts to remove the plugin from the list.  This does
      //  not actually unload the plugin library.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   //--------------------------------------------------------------------------
   // Plugin Interface
   //--------------------------------------------------------------------------
   virtual std::string getPluginName() const = 0;
      //> This method returns the character std::string that identifies the
      //  plugin.
      //<

   //---
   // CSM Plugin Descriptors
   //---
   virtual std::string getManufacturer() const = 0;
      //> This method returns name of the organization that created the plugin.
      //<

   virtual std::string getReleaseDate() const = 0;
      //> This method returns release date of the plugin.
      //  The returned string follows the ISO 8601 standard.
      //
      //-    Precision   Format           Example
      //-    year        yyyy             "1961"
      //-    month       yyyymm           "196104"
      //-    day         yyyymmdd         "19610420"
      //<

   virtual Version getCsmVersion() const = 0;
      //> This method returns the CSM API version that the plugin conforms to.
      //<

   //---
   // Model Availability
   //---
   virtual size_t getNumModels() const = 0;
      //> This method returns the number of types of models that this plugin
      //  can create.
      //<

   virtual std::string getModelName(size_t modelIndex) const = 0;
      //> This method returns the name of the model for the given modelIndex.
      //  The order does not matter - the index is only used to cycle through
      //  all of the model names.
      //
      //  The model index must be less than getNumModels(), or an exception
      //  will be thrown.
      //<

   virtual std::string getModelFamily(size_t modelIndex) const = 0;
      //> This method returns the model "family" for the model for the given
      //  modelIndex.  This should be the same as what is returned from
      //  csm::Model::getFamily() for the model.
      //
      //  SETs can use this information to exclude models when searching for a
      //  model to create.
      //
      //  The model index must be less than getNumModels(), or an exception
      //  will be thrown.
      //<

   //---
   // Model Descriptors
   //---
   virtual Version getModelVersion(const std::string& modelName) const = 0;
      //> This method returns the version of the code for the model given
      //  by modelIndex.  The Version object can be compared to other Version
      //  objects with its comparison operators.  Not to be confused with the
      //  CSM API version.
      //<

   //---
   // Model Construction
   //---
   virtual bool canModelBeConstructedFromState(
      const std::string& modelName,
      const std::string& modelState,
      WarningList* warnings = NULL) const = 0;
      //> This method returns a boolean indicating whether or not a model of the
      //  given modelName can be constructed from the given modelState.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   virtual bool canModelBeConstructedFromISD(
      const Isd& imageSupportData,
      const std::string& modelName,
      WarningList* warnings = NULL) const = 0;
      //> This method returns a boolean indicating whether or not a model of the
      //  given modelName can be constructed from the given imageSupportData.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   virtual Model* constructModelFromState(
      const std::string& modelState,
      WarningList* warnings = NULL) const = 0;
      //> This method allocates and initializes an object of the appropriate
      //  derived Model class with the given modelState and returns a pointer to
      //  the Model base class.  The object is allocated by this method using
      //  new; it is the responsibility of the calling application to delete it.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   virtual Model* constructModelFromISD(
      const Isd& imageSupportData,
      const std::string& modelName,
      WarningList* warnings = NULL) const = 0;
      //> This method allocates and initializes an object of the appropriate
      //  derived Model class with the given imageSupportData and returns a
      //  pointer to the Model base class.  The object is allocated by this
      //  method using new; it is the responsibility of the calling
      //  application to delete it.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   virtual std::string getModelNameFromModelState(
      const std::string& modelState,
      WarningList* warnings = NULL) const = 0;
      // This method returns the model name for which the given modelState
      // is applicable.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   //---
   // Image Support Data Conversions
   //---
   virtual bool canISDBeConvertedToModelState(
      const Isd& imageSupportData,
      const std::string& modelName,
      WarningList* warnings = NULL) const = 0;
      //> This method returns a boolean indicating whether or not a model state
      //  of the given modelName can be constructed from the given
      //  imageSupportData.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   virtual std::string convertISDToModelState(
      const Isd& imageSupportData,
      const std::string& modelName,
      WarningList* warnings = NULL) const = 0;
      //> This method returns a model state string for the given modelName,
      //  constructed from the given imageSupportData.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   static const std::string& getDataDirectory() { return theDataDir; }
      //> This method returns the data directory set by the Sensor Exploitation
      //  Tool (SET) before plugins are loaded.  It contains the path to the
      //  common directory area that contains extra support files that derived
      //  plugin classes may need.  This directory should be used in preference
      //  to any environment variable.
      //<

   static void setDataDirectory(const std::string& dir) { theDataDir = dir; }
      //> This method sets the data directory string.  This directory contains
      //  files provided by plugin implementers that are necessary for the
      //  proper execution of the plugin and model code.
      //
      //  This method should be called by the SET before any plugins are loaded
      //  and should not be changed while any plugins are loaded.  Plugin
      //  implementations should not call this method.  They should only
      //  read the directory by calling getDataDirectory().
      //<

protected:
   Plugin();
      //> This special constructor is responsible for registering each plugin
      //  by adding it to theList. It is invoked by a special derived class
      //  constructor that is only used by the static instance of the derived
      //  class.
      //<

private:
   class Impl;

   static Impl* impl();
      //> This method returns the static implementation singleton.
      //<

   static PluginList* theList;
   static Impl*       theImpl;

   static std::string theDataDir;
};

} // namespace csm

#endif
