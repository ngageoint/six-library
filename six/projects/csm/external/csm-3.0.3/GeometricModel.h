//#############################################################################
//
//    FILENAME:          GeometricModel.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for abstract class that is to provide a common interface from
//    which CSM geometric models will inherit.  It is derived from the Model
//    base class and may have other CSM API classes derived from it.
//
//    LIMITATIONS:       None
//
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     27-Jun-2003   LMT    Initial version.
//     01-Jul-2003   LMT    Remove constants, error/warning
//                          and make methods pure virtual.
//                          CharType enum.
//     31-Jul-2003   LMT    Change calls with a "&" to a "*",
//                          combined CharType with ParamType
//                          to create Param_CharType, //reordered
//                          methods to match API order, added
//                          systematic error methods.
//     06-Aug-2003   LMT    Removed all Characteristic calls.
//     08-Oct 2003   LMT    Added getImageSize calls
//     06-Feb-2004   KRW    Incorporates changes approved by
//                          January and February 2004
//                          configuration control board.
//     30-Jul-2004   PW     Initail API 3.1 version
//     01-Nov-2004   PW     October 2004 CCB
//     22 Oct 2010   DSL    CCB Change add getCurrentCrossCovarianceMatrix
//                                     and getOriginalCrossCovarianceMatrix
//     22 Oct 2010   DSL    CCB Change add getCurrentCrossCovarianceMatrix
//                                     and getOriginalCrossCovarianceMatrix
//     25 Oct 2010   DSL    CCB Change add getNumGeometricCorrectionSwitches,
//                                         getGeometricCorrectionName,
//                                         getCurrentGeometricCorrectionSwitch,
//                                     and setCurrentGeometricCorrectionSwitch
//     25 Oct 2010   DSL    CCB Change add getNumGeometricCorrectionSwitches,
//                                         getGeometricCorrectionName,
//                                         getCurrentGeometricCorrectionSwitch,
//                                     and setCurrentGeometricCorrectionSwitch
//     02-Mar-2012   SCM    Refactored interface.
//     02-Jul-2012   SCM    Made getUnmodeledError() be implemented inline.
//     26-Sep-2012   JPK    Split SensorModel class into GeometricModel and
//                          RasterGM classes.
//     26-Sep-2012   SCM    Moved all sensor partials to derived RasterGM
//                          class.
//     02-Oct-2012   SCM    Added getParameterUnits() method.
//     30-Oct-2012   SCM    Renamed to GeometricModel.h
//     31-Oct-2012   SCM    Added hasShareableParameters().  Moved unmodeled
//                          error methods to this class.
//     01-Nov-2012   SCM    Moved unmodeled error methods back to RasterGM.
//     29-Nov-2012   JPK    Added support for Parameter and SharingCriteria
//                          structs.  Added ParamSet parameters as appropriate
//                          to cross-covariance methods.  Added ability to
//                          discover which parameters belong to a set (i.e.
//                          which should be treated as FIXED or ADJUSTABLE).
//     06-Dec-2012   JPK    De-inlined destructor and getFamily() methods
//                          Added inline definition for convenience method
//                          getCovarianceMatrix().  Replaced ParamType
//                          and ParamSet with param::Type and param::Set
//                          respectively. Removed all getOriginal* and
//                          setOriginal* methods and renamed getCurrent*
//                          and setCurrent* to get* and set* respectively.
//                          Moved Parameter struct inside class definition to
//                          avoid name clashes.
//     17-Dec-2012   BAH    Documentation updates.
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_GEOMETRICMODEL_H
#define __CSM_GEOMETRICMODEL_H

#include "csm.h"
#include "Model.h"

#include <vector>
#include <string>

#define CSM_GEOMETRIC_MODEL_FAMILY "Geometric"

namespace csm
{
class CSM_EXPORT_API GeometricModel : public Model
{
public:

   //***
   // STRUCT: Parameter
   //> The parameter structure contains the set of properties unique
   // to a given adjustable parameter.  The variance for the parameter
   // is not included here since it is more appropriate to consider
   // a covariance matrix for an entire set of parameters.
   //<
   //***
   struct Parameter
   {
   public:
      std::string      name;
      double           value;
      std::string      units;
      param::Type      type;
      SharingCriteria  sharingCriteria;

      Parameter()
         :
            name            (CSM_UNKNOWN),
            value           (0.0),
            units           (CSM_UNKNOWN),
            type            (param::NONE) ,
            sharingCriteria ()
      {}

      Parameter(const std::string&      paramName,
                double                  paramValue,
                const std::string&      paramUnits,
                param::Type             paramType,
                const SharingCriteria&  paramSharingCriteria)
         :
            name            (paramName),
            value           (paramValue),
            units           (paramUnits),
            type            (paramType),
            sharingCriteria (paramSharingCriteria)
      {}
   };

   GeometricModel() {}
   virtual ~GeometricModel();

   virtual std::string getFamily() const;

   virtual EcefCoord getReferencePoint() const = 0;
      //> This method returns the ground point indicating the general
      //  location of the image.
      //<

   virtual void setReferencePoint(const EcefCoord& groundPt) = 0;
      //> This method sets the ground point indicating the general location
      //  of the image.
      //<

   //---
   // Sensor Model Parameters
   //---
   virtual int getNumParameters() const = 0;
      //> This method returns the number of adjustable parameters.
      //<

   virtual std::string getParameterName(int index) const = 0;
      //> This method returns the name for the adjustable parameter
      //  indicated by the given index.
      //
      //  If the index is out of range, a csm::Error may be thrown.
      //<

   virtual std::string getParameterUnits(int index) const = 0;
      //> This method returns the units for the adjustable parameter
      //  indicated by the given index.  This string is intended for human
      //  consumption, not automated analysis.  Preferred unit names are:
      //
      //-    meters                "m"
      //-    centimeters           "cm"
      //-    millimeters           "mm"
      //-    micrometers           "um"
      //-    nanometers            "nm"
      //-    kilometers            "km"
      //-    inches-US             "inch"
      //-    feet-US               "ft"
      //-    statute miles         "mi"
      //-    nautical miles        "nmi"
      //-
      //-    radians               "rad"
      //-    microradians          "urad"
      //-    decimal degrees       "deg"
      //-    arc seconds           "arcsec"
      //-    arc minutes           "arcmin"
      //-
      //-    seconds               "sec"
      //-    minutes               "min"
      //-    hours                 "hr"
      //-
      //-    steradian             "sterad"
      //-
      //-    none                  "unitless"
      //-
      //-    lines per second      "lines/sec"
      //-    samples per second    "samples/sec"
      //-    frames per second     "frames/sec"
      //-
      //-    watts                 "watt"
      //-
      //-    degrees Kelvin        "K"
      //-
      //-    gram                  "g"
      //-    kilogram              "kg"
      //-    pound - US            "lb"
      //-
      //-    hertz                 "hz"
      //-    megahertz             "mhz"
      //-    gigahertz             "ghz"
      //
      //  Units may be combined with "/" or "." to indicate division or
      //  multiplication.  The caret symbol "^" can be used to indicate
      //  exponentiation.  Thus "m.m" and "m^2" are the same and indicate
      //  square meters.  The return "m/sec^2" indicates an acceleration in
      //  meters per second per second.
      //
      //  Derived classes may choose to return additional unit names, as
      //  required.
      //<

   virtual bool hasShareableParameters() const = 0;
      //> This method returns true if there exists at least one adjustable
      //  parameter on the model that is shareable.  See the
      //  isParameterShareable() method.  This method should return false if
      //  all calls to isParameterShareable() return false.
      //<

   virtual bool isParameterShareable(int index) const = 0;
      //> This method returns a flag to indicate whether or not the adjustable
      //  parameter referenced by index is shareable across models.
      //<

   virtual csm::SharingCriteria getParameterSharingCriteria(int index) const = 0;
      //> This method returns characteristics to indicate how the adjustable
      //  parameter referenced by index is shareable across models.
      //<

   virtual double getParameterValue(int index) const = 0;
      //> This method returns the value of the adjustable parameter
      //  referenced by the given index.
      //<

   virtual void setParameterValue(int index, double value) = 0;
      //> This method sets the value for the adjustable parameter referenced by
      //  the given index.
      //<

   virtual param::Type getParameterType(int index) const = 0;
      //> This method returns the type of the adjustable parameter
      //  referenced by the given index.
      //<

   virtual void setParameterType(int index, param::Type pType) = 0;
      //> This method sets the type of the adjustable parameter
      //  reference by the given index.
      //<

   virtual Parameter getParameter(int index) const;
      //> This method returns a Parameter object for the given index.
      //  If the index is out of range, a csm::Error may be thrown.
      //  The default implementation simply calls the methods for each of
      //  the components of the Parameter. This method is declared virtual so
      //  derived classes may opt to provide their own (more efficient)
      //  implementation.
      //<

   virtual void setParameter(int index, const Parameter& parameter);
      //>  This method sets all of the properties of the Parameter indicated by
      //   the given index to those of the given parameter.
      //   If the index is out of range, a csm::Error may be thrown.
      //   The default implementation simply calls the methods for each of
      //   the components of the Parameter.  Note that this precludes setting of
      //   name, units,  and sharing criteria as these are set only by the
      //   derived classes. This method is declared virtual so derived classes
      //   may opt to provide their own (more efficient) implementation.
      //<

   std::vector<int> getParameterSetIndices(param::Set pSet = param::VALID) const;
      //> This method returns a vector of indices of the parameters contained in
      //  the given pSet.
      //<

   std::vector<Parameter> getParameters(param::Set pSet = param::VALID) const;
      //> This method returns a vector containing all parameters in the
      //  given pSet.  The default is all parameters marked as VALID.
      //<

   //---
   // Uncertainty Propagation
   //---
   virtual double getParameterCovariance(int index1,
                                         int index2) const = 0;
      //> This method returns the covariance between the parameters
      //  referenced by index1 and index2.  Variance of a single parameter
      //  is indicated by specifying the samve value for index1 and index2.
      //<

   virtual void setParameterCovariance(int index1,
                                       int index2,
                                       double covariance) = 0;
      //> This method is used to set the covariance between the parameters
      //  referenced by index1 and index2.  Variance of a single parameter
      //  is indicated by specifying the samve value for index1 and index2.
      //<

   //---
   // Error Correction
   //---
   virtual int getNumGeometricCorrectionSwitches() const = 0;
      //> This method returns the number of geometric correction switches
      //  implemented for the current model.
      //<

   virtual std::string getGeometricCorrectionName(int index) const = 0;
      //> This method returns the name for the geometric correction switch
      //  referenced by the given index.
      //<

   virtual void setGeometricCorrectionSwitch(int index,
                                             bool value,
                                             param::Type pType) = 0;
      //> This method is used to enable/disable the geometric correction switch
      //  referenced by the given index.
      //<

   virtual bool getGeometricCorrectionSwitch(int index) const = 0;
      //> This method returns the value of the geometric correction switch
      //  referenced by the given index.
      //<

   typedef std::vector<const GeometricModel*> GeometricModelList;

   inline std::vector<double> getCovarianceMatrix(param::Set pSet = param::VALID) const
   {
      return getCrossCovarianceMatrix(*this,pSet);
   }

   virtual std::vector<double> getCrossCovarianceMatrix(
          const GeometricModel& comparisonModel,
          param::Set pSet = param::VALID,
          const GeometricModelList& otherModels = GeometricModelList()) const = 0;
      //> This method returns a matrix containing the elements of the error
      //  cross covariance between this model and a given second model
      //  (comparisonModel).  The set of cross covariance elements returned is
      //  indicated by pSet, which, by default, is all VALID parameters.
      //
      //  If comparisonModel is the same as this model, the covariance for
      //  this model will be returned.  It is equivalent to calling
      //  getParameterCovariance() for the same set of elements.  Note that
      //  even if the cross covariance for a particular model type is always
      //  zero, the covariance for this model must still be supported.
      //
      //  The otherModels list contains all of the models in the current
      //  photogrammetric process; some cross-covariance implementations are
      //  influenced by other models.  It can be omitted if it is not needed
      //  by any models being used.
      //
      //  The returned vector will logically be a two-dimensional matrix of
      //  covariances, though for simplicity it is stored in a one-dimensional
      //  vector (STL has no two-dimensional structure).  The height (number of
      //  rows) of this matrix is the number of parameters on the current model,
      //  and the width (number of columns) is the number of parameters on
      //  the comparison model.  Thus, the covariance between p1 on this model
      //  and p2 on the comparison model is found in index (N*p1 + p2)
      //  in the returned vector.  N is the size of the vector returned by
      //  getParameterSetIndices() on the comparison model for the given pSet).
      //
      //  Note that cross covariance is often zero.  Non-zero cross covariance
      //  can occur for models created from the same sensor (or different
      //  sensors on the same platform).  While cross covariances can result
      //  from a bundle adjustment involving multiple models, no mechanism 
      //  currently exists within csm to "set" the cross covariance between 
      //  models.  It should thus be assumed that the returned cross covariance
      //  reflects the "un-adjusted" state of the models.
      //<
};

} // namespace csm

#endif
