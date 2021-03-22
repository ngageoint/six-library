//#############################################################################
//
//    FILENAME:          LinearDecayCorrelationModel.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header the piecewise linear decay correlation model class derived from
//    the CorrelationModel base class.  This class is used to compute the
//    correlation between model parameters in a community sensor model (CSM).
//    Intended for replacement models to recreate cross covariance calculations;
//    most calling applications will use the CSM cross covariance method.
//
//    The class holds correlation data in the form of two lists.  The first is
//    a list of times that specify the start times of each of a series of time
//    segments.  The second list specifies the correlation at the specified
//    time.  Between specified times, the correlation is a linear interpolation
//    in time of the correlation at each end of the time segment.
//
//    The class allows model parameters to be divided into disjoint groups,
//    such that the correlation coefficient between any two parameters in the
//    same group is given by the equation above, and the correlation between
//    two parameters in different groups is 0.0.  This design provides a way to
//    assign model parameters to groups, to set the values of the correlation
//    parameters for a given group, and to compute the correlation as
//    specified above.
//
//    LIMITATIONS:       None
//
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     20-Nov-2012   ISK      Initial Release.
//     30-Oct-2012   SCM      Renamed to LinearDecayCorrelationModel.h
//     17-Dec-2012   BAH      Documentation updates.
//     12-Feb-2013   JPK      Renamed CovarianceModel to CorrelationModel.
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_LINEARDECAYCORRELATIONMODEL_H_
#define __CSM_LINEARDECAYCORRELATIONMODEL_H_

#include "CorrelationModel.h"

#include <vector>

namespace csm
{

class CSM_EXPORT_API LinearDecayCorrelationModel : public CorrelationModel
{
public:
   // represents a set of correlation parameters to be associated with one group
   // The two vectors must be the same size.  The correlations in
   // theInitialCorrsPerSegment are unitless; the times in theTimesPerSegment
   // are in seconds.
   //  * timesPerSegment[i] < timesPerSegment[i+1]
   //  * initialCorrsPerSegment[i] >= initialCorrsPerSegment[i+1]
   //  * 1 >= initialCorrsPerSegment[i] >= 0
   struct CSM_EXPORT_API Parameters
   {
      Parameters() {}
      Parameters(const std::vector<double>& initialCorrsPerSegment,
                 const std::vector<double>& timesPerSegment)
         : theInitialCorrsPerSegment(initialCorrsPerSegment),
           theTimesPerSegment(timesPerSegment) {}

      std::vector<double> theInitialCorrsPerSegment;
      std::vector<double> theTimesPerSegment;
   };

   LinearDecayCorrelationModel(size_t numSMParams, size_t numCPGroups);
      //> Constructor.  The number of model parameters and correlation
      //  parameter groups must be provided.
      //
      // Preconditions:
      // * numSMParams must be non-negative
      // * numCPGroups must be non-negative
      //<

   virtual ~LinearDecayCorrelationModel();

   virtual size_t getNumSensorModelParameters() const;
      //> Returns the number of model parameters. The returned value
      //  will be the same as the value of numSMParams passed to the
      //  constructor when the object was created.
      //<

   virtual int getCorrelationParameterGroup(size_t smParamIndex) const;
      //> Returns the index of the correlation parameter group to which the
      //  model parameter given by smParamIndex belongs.  If the model
      //  parameter does not belong to a group, the return value will be -1.
      //
      //  Precondition:
      //  * 0 <= smParamIndex < numSMParams
      //<

   const Parameters& getCorrelationGroupParameters(size_t cpGroupIndex) const;
      //> Returns the values of the correlation parameters for the group
      //  given by cpGroupIndex.
      //
      //  Throws an exception if cpGroupIndex is out of range.
      //<

   virtual double getCorrelationCoefficient(size_t cpGroupIndex,
                                            double deltaTime) const;
      //> Computes the correlation coefficient for the correlation parameter
      //  group given by cpGroupIndex for the given deltaTime.
      //  The deltaTime argument represents the difference in time, in seconds,
      //  for which the correlation is calculated.
      //
      //  Preconditions:
      //  * 0 <= cpGroupIndex < numCPGroups
      //
      //  Notes:
      //
      //  The deltaTime argument should be positive, but the methods uses the
      //  absolute value of the argument, so a negative deltaTime is acceptable.
      //
      //  If the computed correlation coefficient is outside the range [-1, 1],
      //  the methods will "clamp" the returned value to the nearest number
      //  within that range.  For example, if the correlation coefficient
      //  equation evaluates to 1.1 for a given deltaTime,
      //  the value 1.0 will be returned.
      //<

   void setCorrelationParameterGroup(size_t smParamIndex,
                                     size_t cpGroupIndex);
      //> Assigns a model parameter to a correlation parameter group.
      //  The index of the model parameter is given by smParamIndex, and the
      //  index of the group to which it is assigned is given by cpGroupIndex.
      //
      //  Preconditions:
      //  * 0 <= smParamIndex < numSMParams
      //  * 0 <= cpGroupIndex < numCPGroups
      //
      //  Notes:
      //
      //  Although the getCorrelationParameterGroup methods can return -1
      //  as a group index (indicating the group has not been set),
      //  it is an error to try to set the group index to -1.
      //<

   void setCorrelationGroupParameters(
      size_t cpGroupIndex,
      const std::vector<double>& initialCorrsPerSegment,
      const std::vector<double>& timesPerSegment);
      //> Sets the correlation parameter values for the group given by
      //  cpGroupIndex.  The correlations in initialCorrsPerSegment are
      //  unitless; the times in timesPerSegment are in seconds.
      //
      //  Precondition:
      //  * 0 <= cpGroupIndex < numCPGroups
      //  * initialCorrsPerSegment and timesPerSegment are the same size
      //  * timesPerSegment[i] < timesPerSegment[i+1]
      //  * initialCorrsPerSegment[i] >= initialCorrsPerSegment[i+1]
      //  * 1 >= initialCorrsPerSegment[i] >= 0
      //<

   void setCorrelationGroupParameters(size_t            cpGroupIndex,
                                      const Parameters& params);
      //> Sets the values of the correlation parameters in params for the
      //  group given by cpGroupIndex.
      //
      //  Precondition:
      //  * 0 <= cpGroupIndex < numCPGroups
      //
      //  Throws an exception if cpGroupIndex or any of the correlation
      //  parameters is out of range.
      //<

protected:
   std::vector<int> theGroupMapping;
      //> This data member stores the mapping between model parameter
      //  indices and correlation parameter group indices.
      //<

   std::vector<Parameters> theCorrParams;
      //> This data member stores the values of the correlation parameters for
      //  each parameter group.
      //<

   void checkSensorModelParameterIndex(size_t             smParamIndex,
                                       const std::string& functionName) const;
      //> This method throws a csm::Error if the given smParamIndex is not
      //  in the range [0, numSMParams).  The method name supplied to the
      //  csm::Error object is functionName.
      //<

   void checkParameterGroupIndex(size_t             groupIndex,
                                 const std::string& functionName) const;
      //> This method throws a csm::Error if the given groupIndex is not
      //  in the range [0, numCPGroups).  The method name supplied to the
      //  csm::Error object is functionName.
      //<
};

} // namespace csm

#endif
