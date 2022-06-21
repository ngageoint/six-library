//#############################################################################
//
//    FILENAME:          Model.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for abstract base class that is to provide a common interface from
//    which all CSM models will inherit.  It may have other CSM API classes
//    derived from it.
//
//    LIMITATIONS:       None
//
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     02-Mar-2012   SCM      Initial creation
//     26-Sep-2012   JPK      Removed getModelType() and reference point.
//     30-Oct-2012   SCM      Renamed to Model.h
//     31-Oct-2012   SCM      Split getSensorTypeAndMode() into getSensorType()
//                            and getSensorMode().  Removed include for
//                            SensorTypeAndMode.h.  Moved
//                            getTrajectoryIdentifier() to this class.
//     06-Dec-2012   JPK      Added pure virtual method replaceModelState()
//     17-Dec-2012   BAH      Documentation updates.
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_MODEL_H
#define __CSM_MODEL_H

#include "csm.h"
#include "Warning.h"
#include "Version.h"
#include <string>

namespace csm
{

class CSM_EXPORT_API Model
{
public:
   Model() {}
   virtual ~Model() {}
      //> A virtual destructor is needed so derived class destructors will
      //  be called when the base class object is destroyed.
      //<

   //---
   // Basic model information
   //---
   virtual std::string getFamily() const = 0;
      //> This method returns the family of this model.  It can be used to cast
      //  to the appropriate derived class for family-specific processing.
      //<

   virtual Version getVersion() const = 0;
      //> This method returns the version of the model code.  The Version
      //  object can be compared to other Version objects with its comparison
      //  operators.  Not to be confused with the CSM API version.
      //<

   virtual std::string getModelName() const = 0;
      //> This method returns a string identifying the name of the model.
      //<

   virtual std::string getPedigree() const = 0;
      //> This method returns a string that identifies the sensor,
      //  the model type, its mode of acquisition and processing path.
      //  For example, an optical sensor model or a cubic rational polynomial
      //  model created from the same sensor's support data would produce
      //  different pedigrees for each case.
      //<

   //---
   // Basic collection information
   //---
   virtual std::string getImageIdentifier() const = 0;
      //> This method returns an identifier to uniquely indicate the imaging
      //  operation associated with this model.
      //  This is the primary identifier of the model.
      //
      //  This method may return an empty string if the ID is unknown.
      //<

   virtual void setImageIdentifier(const std::string& imageId,
                                   WarningList* warnings = NULL) = 0;
      //> This method sets an identifier to uniquely indicate the imaging
      //  operation associated with this model.  Typically used for models
      //  whose initialization does not produce an adequate identifier.
      //
      //  If a non-NULL warnings argument is received, it will be populated
      //  as applicable.
      //<

   virtual std::string getSensorIdentifier() const = 0;
      //> This method returns an identifier to indicate the specific sensor
      //  that was used to acquire the image.  This ID must be unique among
      //  sensors for a given model name.  It is used to determine parameter
      //  correlation and sharing.  Equivalent to camera or mission ID.
      //
      //  This method may return an empty string if the sensor ID is unknown.
      //<

   virtual std::string getPlatformIdentifier() const = 0;
      //> This method returns an identifier to indicate the specific platform
      //  that was used to acquire the image.  This ID must unique among
      //  platforms for a given model name.  It is used to determine parameter
      //  correlation sharing.  Equivalent to vehicle or aircraft tail number.
      //
      //  This method may return an empty string if the platform ID is unknown.
      //<

   virtual std::string getCollectionIdentifier() const = 0;
      //> This method returns an identifer to indicate a collection activity
      //  common to a set of images.  This ID must be unique among collection
      //  activities for a given model name.  It is used to determine parameter
      //  correlation and sharing.
      //<

   virtual std::string getTrajectoryIdentifier() const = 0;
      //> This method returns an identifier to indicate a trajectory common
      //  to a set of images.  This ID must be unique among trajectories
      //  for a given model name.  It is used to determine parameter
      //  correlation and sharing.
      //<

   virtual std::string getSensorType() const = 0;
      //> This method returns a description of the sensor type (EO, IR, SAR,
      //  etc).  See csm.h for a list of common types.  Should return
      //  CSM_SENSOR_TYPE_UNKNOWN if the sensor type is unknown.
      //<

   virtual std::string getSensorMode() const = 0;
      //> This method returns a description of the sensor mode (FRAME,
      //  PUSHBROOM, SPOT, SCAN, etc).  See csm.h for a list of common modes.
      //  Should return CSM_SENSOR_MODE_UNKNOWN if the sensor mode is unknown.
      //<

   virtual std::string getReferenceDateAndTime() const = 0;
      //> This method returns an approximate date and time at which the
      //  image was taken.  The returned string follows the ISO 8601 standard.
      //
      //-    Precision   Format           Example
      //-    year        yyyy             "1961"
      //-    month       yyyymm           "196104"
      //-    day         yyyymmdd         "19610420"
      //-    hour        yyyymmddThh      "19610420T20"
      //-    minute      yyyymmddThhmm    "19610420T2000"
      //-    second      yyyymmddThhmmss  "19610420T200000"
      //<

   //---
   // Sensor Model State
   //---
   virtual std::string getModelState() const = 0;
      //> This method returns a string containing the data to exactly recreate
      //  the current model.  It can be used to restore this model to a
      //  previous state with the replaceModelState method or create a new
      //  model object that is identical to this model.
      //  The string could potentially be saved to a file for later use.
      //  An empty string is returned if it is not possible to save the
      //  current state.
      //<

   virtual void replaceModelState(const std::string& argState) = 0;
      //> This method attempts to initialize the current model with the state
      //  given by argState.  The argState argument can be a string previously
      //  retrieved from the getModelState method.
      //
      //  If argState contains a valid state for the current model,
      //  the internal state of the model is updated.
      //
      //  If the model cannot be updated to the given state, a csm::Error is
      //  thrown and the internal state of the model is undefined.
      //
      //  If the argument state string is empty, the model remains unchanged.
      //<
};

} // namespace csm

#endif
