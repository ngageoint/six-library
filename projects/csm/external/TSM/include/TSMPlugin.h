//#############################################################################
//
//    FILENAME:   TSMPlugin.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for abstract base class that is to provide a common interface from
//    which all Tactical Sensor Model (TSM) plugin factories shall inherit.
//
//    LIMITATIONS:       None
// 
//    SOFTWARE HISTORY: 
// 
//     Date          Author   Comment   
//     -----------   ------   ------- 
//     May-2003	     J. Olson Received initial version from BAE.
//     20-Jun-2003   KFM      Revised to incorporate plugin list and automatic
//                            registration for derived types.
//     01-Jul-2003   KFM      Updated signatures.
//     06-Feb-2004   KRW      Incorporates changes approved by
//			      the January and February 2004
//			      configuration control board.
//     08-JUN-2004   TWC      API 3.1
//     19-Aug-2004   PW       Add throws
//
//    NOTES:
//
//    Initial coding of this class was accomplished by BAE Corporation. This
//    version contains modifications by Harris Corporation with the primary
//    goal of altering the method by which derived factories are "registered"
//    with the base plugin class.
//
//    To use this for a plugin, the developer must simply inherit from this
//    class providing, at least, implementation for each pure virtual function.
//    In order to allow the plugin to self-register itself and be recognized
//    by the system as a "plugin", a static instance of the derived class must
//    invoke the TSMPlugin constructor.
//
//#############################################################################
#ifndef __TSMPLUGIN_H
#define __TSMPLUGIN_H

#ifdef _WIN32
#pragma warning( disable : 4290 )
#endif
#include <list>
#include <string>
#include "TSMImageSupportData.h"
#include "TSMMisc.h"
#include "TSMError.h"

class TSMWarning;
class TSMSensorModel;
class tsm_ISD;

//-----------------------------------------------------------------------------
// This is an example factory for plug ins. In the real world, we might have
// multiple different classes in each shared library that are made to work
// together. All these classes must be created by this factory class.
//-----------------------------------------------------------------------------

class TSM_EXPORT_API TSMPlugin
{
public:

   //--------------------------------------------------------------------------
   // Types
   //--------------------------------------------------------------------------

   typedef std::list < const TSMPlugin* > TSMPluginList;

   class Impl;

   //--------------------------------------------------------------------------
   // Constructors/Destructor
   //--------------------------------------------------------------------------

   virtual ~TSMPlugin() {}

   //--------------------------------------------------------------------------
   // List Managing Methods
   //--------------------------------------------------------------------------

   static TSMWarning* getList(TSMPluginList*&  aTSMPluginList)
      throw (TSMError);
      //>This method provides access to the list of all plugins that are
      // currently registered.
      //<

   static TSMWarning* findPlugin(
      const std::string&  pluginName,
      TSMPlugin*& aTSMPlugin) 
      throw (TSMError);

      // pre: None.
      // post: Returns a pointer to the first plugin found whose name is
      //    aName; returns NULL if no such plugin found.

   static TSMWarning* removePlugin( 
      const  std::string&  pluginName) 
      throw (TSMError);

   //--------------------------------------------------------------------------
   // Plugin Interface
   //--------------------------------------------------------------------------

   virtual TSMWarning* getPluginName(
      std::string& pluginName)  
      const throw (TSMError) = 0;

      //>This method returns the character std::string that identifies the plugin.
      //<

   //---
   // TSM Plugin Descriptors
   //---

   virtual TSMWarning* getManufacturer(
      std::string& manufacturer_name) 
      const throw (TSMError) = 0;

   virtual TSMWarning* getReleaseDate(
      std::string&  release_date) 
      const throw (TSMError) = 0;

   //---
   // Sensor Model Availability
   //---

   virtual TSMWarning* getNSensorModels(int& n_sensor_models) const throw (TSMError) = 0;
   
   virtual TSMWarning* getSensorModelName(
      const int& sensor_model_index,
      std::string& sensor_model_name) 
      const throw (TSMError) = 0;
   
   //---
   // Sensor Model Descriptors
   //---

   virtual TSMWarning* getSensorModelVersion(
      const std::string &sensor_model_name,
      int& version) 
      const throw (TSMError) = 0;

   //---
   // Sensor Model Construction
   //---

   virtual TSMWarning* canSensorModelBeConstructedFromState(
      const std::string& sensor_model_name,
      const std::string& sensor_model_state,
      bool& constructible) 
      const throw (TSMError) = 0;


   virtual TSMWarning* canSensorModelBeConstructedFromISD(
      const tsm_ISD&  Image_support_data,
      const std::string&  sensor_model_name,
      bool&  constructible) 
      const throw (TSMError) = 0;


   virtual TSMWarning* constructSensorModelFromState(
      const std::string& sensor_model_state,
      TSMSensorModel*& sensor_model) 
      const throw (TSMError) = 0;

   virtual TSMWarning* constructSensorModelFromISD(
      const tsm_ISD&   image_support_data,
      const std::string& sensor_model_name,
      TSMSensorModel*& sensor_model) 
      const throw (TSMError) = 0;
   
   virtual TSMWarning* getSensorModelNameFromSensorModelState(
      const std::string& sensor_model_state,
      std::string& sensor_model_name) 
      const throw (TSMError) = 0;
   
   //---
   // Image Support Data Conversions
   //---

   virtual TSMWarning* canISDBeConvertedToSensorModelState(
      const tsm_ISD&   image_support_data,
      const std::string& sensor_model_name,
      bool& convertible) 
      const throw (TSMError) = 0;

   virtual TSMWarning* convertISDToSensorModelState(
      const tsm_ISD& image_support_data,
      const std::string& sensor_model_name,
      std::string& sensor_model_state) 
      const throw (TSMError) = 0;

protected:

   //--------------------------------------------------------------------------
   // Constructors
   //--------------------------------------------------------------------------

   TSMPlugin();
      //>This special constructor is responsible for registering each plugin
      // by adding it to theList. It is invoked by a special derived class
      // constructor that is only used by the static instance of the derived
      // class. (Refer to the example plugins to see how this is accomplished.)
      //<

private:

   //--------------------------------------------------------------------------
   // Data Members
   //--------------------------------------------------------------------------

   static TSMPluginList* theList;
   static Impl*          theImpl;

}; // TSMPlugin

#endif // __TSMPLUGIN_H
