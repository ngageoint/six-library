//##################################################################
//
//    FILENAME:          CorrelationModel.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for the covariance model base class used in the CSM
//    interface.  Intended for replacement models to recreate cross
//    covariance calculations;  most calling applications will use
//    the CSM cross covariance method.  The covariance model is
//    passed as the base class and cast to the appropriate derived
//    class for use.
//
//    LIMITATIONS:       None
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     29-Mar-2012   SCM      Refactored interface
//     30-Oct-2012   SCM      Renamed to CorrelationModel.h
//     14-Nov-2012   SCM      Added NoCorrelationModel.
//     06-Dec 2012   JPK      Replaced "UNKNOWN" with CSM_UNKNOWN
//     17-Dec-2012   BAH      Documentation updates.
//     12-Feb-2013   JPK      Renamed CovarianceModel to
//                            CorrelationModel
//     18-Feb-2013   JPK      Added getDecorrelationEventTime() and
//                            setDecorrelationEventTime().  Implemented
//                            getNumCorrelationParameterGroups().
//     20-Sep-2019   JPK      Added setFormat() and
//                            setNumCorrelationParameterGroups()
//
//    NOTES:
//
//#####################################################################

#ifndef __CSM_CORRELATIONMODEL_H
#define __CSM_CORRELATIONMODEL_H

#include <string>
#include "csm.h"
#include "Error.h"

namespace csm
{

class CSM_EXPORT_API CorrelationModel
{
public:

   virtual ~CorrelationModel() {}
      //> A virtual destructor is needed so derived class destructors will
      //  be called when the base class object is destroyed.
      //<

   const std::string& format() const { return theFormat; }

   virtual size_t getNumSensorModelParameters() const = 0;
      //> Returns the number of model parameters. The returned value
      //  will be the same as the value of numSMParams passed to the
      //  constructor when the derived object was created.
      //<

   inline size_t getNumCorrelationParameterGroups() const;
                                                        
      //> Returns the number of correlation parameter groups. The returned
      //  value will be the same as the value of numCPGroups passed to the
      //  constructor when this object was created.
      //<

   virtual int getCorrelationParameterGroup(size_t smParamIndex) const = 0;
      //> Returns the index of the correlation parameter group to which the
      //  model parameter given by smParamIndex belongs.  If the model
      //  parameter does not belong to a group, the return value will be -1.
      //
      //  The smParamIndex must be less than getNumSensorModelParameters().
      //<

   virtual double getCorrelationCoefficient(size_t cpGroupIndex,
                                            double deltaTime) const = 0;
      //> Computes the correlation coefficient for the correlation parameter
      //  group given by cpGroupIndex for the given deltaTime.
      //  The deltaTime argument represents the difference in time, in seconds,
      //  for which the correlation is calculated.
      //
      //  The cpGroupIndex must be less than getNumCorrelationParameterGroups().
      //
      //  Notes:
      //
      //  The deltaTime argument should be positive, but the method uses the
      //  absolute value of the argument, so a negative deltaTime is acceptable.
      //
      //  If the computed correlation coefficient is outside the range [-1, 1],
      //  the method will "clamp" the returned value to the nearest number
      //  within that range.  For example, if the correlation coefficient
      //  equation evaluates to 1.1 for a given deltaTime,
      //  the value 1.0 will be returned.
      //<
      
   const std::string& getDecorrelationEventTime(size_t cpGroupIndex) const;
   //> This method returns the decorrelation event time for the specified
   //  parameter group index.  The returned string follows the ISO 8601 standard.
   //
   //-    Precision   Format           Example
   //-    year        yyyy             "1961"
   //-    month       yyyymm           "196104"
   //-    day         yyyymmdd         "19610420"
   //-    hour        yyyymmddThh      "19610420T20"
   //-    minute      yyyymmddThhmm    "19610420T2000"
   //-    second      yyyymmddThhmmss  "19610420T200000"
   //  If no decorrelation event time has been set for the specified
   //  parameter group, the returned string will be empty.
   //  If the specified parameter group index is outside the range of available
   //  parameter groups, an exception will be thrown.
   //<
   
   void setDecorrelationEventTime(const std::string& decorrelationEventTime,
                                  size_t             cpGroupIndex);
   //> This method sets the decorrelation event time for the specified
   //  parameter group index.  The provided string should follow the ISO 8601 
   //  standard.
   //
   //-    Precision   Format           Example
   //-    year        yyyy             "1961"
   //-    month       yyyymm           "196104"
   //-    day         yyyymmdd         "19610420"
   //-    hour        yyyymmddThh      "19610420T20"
   //-    minute      yyyymmddThhmm    "19610420T2000"
   //-    second      yyyymmddThhmmss  "19610420T200000"
   //  By default, the decorrelation event time for all groups is set to an
   //  empty string.  If the specified parameter group index is outside the 
   //  range of available parameter groups, an exception will be thrown.
   //<

protected:
      
   CorrelationModel(const std::string& format,
                    size_t numCPGroups);

   void setFormat(const std::string& format);

   void setNumCorrelationParameterGroups(size_t numCorrGroups);
   
 private:
   CorrelationModel()
      :
        theFormat                  (CSM_UNKNOWN),
        theDecorrelationEventTimes ()
        {}
   //> This is the default constructor.  It is declared private to prevent
   //  its use.
   //<

   std::string theFormat;
   //> This data member is a string identifying the "format" for the
   //  current derived CorrelationModel.
   //<
   std::vector<std::string> theDecorrelationEventTimes;
   //> This data member is a vector of strings which represent the
   //  decorrelation event times for each group of parameters.
   //<
};
   
inline size_t CorrelationModel::getNumCorrelationParameterGroups() const
{
   return theDecorrelationEventTimes.size();
}
   
//***
// CLASS: NoCorrelationModel
//> The NoCorrelationModel class is needed for sensor models that do not have
//  a covariance model because the RasterGM::getCorrelationModel() method
//  returns a const reference.
//<
//***
class CSM_EXPORT_API NoCorrelationModel : public CorrelationModel
{
public:
   NoCorrelationModel() : CorrelationModel("NONE",0) {}
   virtual ~NoCorrelationModel() {}

   virtual size_t getNumSensorModelParameters() const { return 0; }
   
   virtual int getCorrelationParameterGroup(size_t smParamIndex) const
   {
      // there can be no smParamIndex that is less than getNumSensorModelParameters()
      throw Error(Error::INDEX_OUT_OF_RANGE,
                  "Invalid index parameter",
                  "csm::NoCorrelationModel::getNumSensorModelParameters");
   }

   virtual double getCorrelationCoefficient(size_t cpGroupIndex, double deltaTime) const
   {
      // there can be no cpGroupIndex that is less than getNumCorrelationParameterGroups()
      throw Error(Error::INDEX_OUT_OF_RANGE,
                  "Invalid index parameter",
                  "csm::NoCorrelationModel::getCorrelationCoefficient");
   }
};
   
} // namespace csm

#endif
