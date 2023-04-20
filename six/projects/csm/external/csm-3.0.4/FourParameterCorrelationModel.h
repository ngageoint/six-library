//#############################################################################
//
//    FILENAME:          FourParameterCorrelationModel.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header the four-parameter correlation model class derived from
//    the CorrelationModel base class.  This class is used to compute the
//    correlation between model parameters in a community sensor model (CSM).
//    Intended for replacement models to recreate cross covariance calculations;
//    most calling applications will use the CSM cross covariance method.
//
//    The class is a wrapper around the equation
//
//    rho = a * (alpha + ((1 - alpha)*(1 + beta)/(beta + exp(deltaT / tau)))),
//
//    where a, alpha, beta, and tau are the correlation parameters, deltaT is
//    the difference in time in seconds, and rho is the correlation coefficient.
//
//    The class allows model parameters to be divided into disjoint groups,
//    such that the correlation coefficient between any two parameters in the
//    same group is given by the equation above, and the correlation between
//    two parameters in different groups is 0.0.  This design provides a way to
//    assign model parameters to groups, to set the values of the correlation
//    parameters for a given group, and to compute the equation above.
//
//    LIMITATIONS:       None
//
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     29-Mar-2012   SCM      Refactored interface.
//     22-Jun-2012   SCM      Made Parameters public, added accessor.
//     30-Oct-2012   SCM      Renamed to FourParameterCorrelationModel.h
//     06-Nov-2012   JPK      Updated equation per CR 2012 10 17 001
//     17-Dec-2012   BAH      Documentation updates.
//     12-Feb-2013   JPK      Renamed CovarianceModel to CorrelationModel.
//     18-Feb-2013   JPK      Removed getNumCorrelationParameterGroups,
//                            now provided on base class.
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_FOURPARAMETERCORRELATIONMODEL_H_
#define __CSM_FOURPARAMETERCORRELATIONMODEL_H_

#include "CorrelationModel.h"

#include <vector>

namespace csm
{

class CSM_EXPORT_API FourParameterCorrelationModel : public CorrelationModel
{
public:
   // represents a set of four correlation parameters, grouped to simplify the implementation
   struct CSM_EXPORT_API Parameters
   {
      Parameters() : a(0.), alpha(0.), beta(0.), tau(0.) {}
      Parameters(double aA, double aAlpha, double aBeta, double aTau)
         : a(aA), alpha(aAlpha), beta(aBeta), tau(aTau) {}

      double a;     // unitless
      double alpha; // unitless
      double beta;  // unitless
      double tau;   // seconds
   };

   FourParameterCorrelationModel(size_t numSMParams, size_t numCPGroups);
      //> Constructor.  The number of model parameters and correlation
      //  parameter groups must be provided.
      //
      // Preconditions:
      // * numSMParams must be non-negative
      // * numCPGroups must be non-negative
      //<

   virtual ~FourParameterCorrelationModel();

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
      //> Returns the values of the correlation parameters for the group given
      //  by cpGroupIndex.
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
      //  The deltaTime argument should be positive, but the method uses the
      //  absolute value of the argument, so a negative deltaTime is acceptable.
      //
      //  If the computed correlation coefficient is outside the range [-1, 1],
      //  the method will "clamp" the returned value to the nearest number
      //  within that range.  For example, if the correlation coefficient
      //  equation evaluates to 1.1 for a given deltaTime,
      //  the value 1.0 will be returned.
      //<

   void setCorrelationParameterGroup(size_t smParamIndex,
                                     size_t cpGroupIndex);
      //> Assigns a model parameter to a correlation parameter group.
      //  The index of the model parameter is given by smParamIndex, and the
      //  index of the group to which it is assigned is given by cppGroupIndex.
      //
      //  Preconditions:
      //  * 0 <= smParamIndex < numSMParams
      //  * 0 <= cpGroupIndex < numCPGroups
      //
      //  Notes:
      //
      //  Although the getCorrelationParameterGroup method can return -1
      //  as a group index (indicating the group has not been set),
      //  it is an error to try to set the group index to -1.
      //<

   void setCorrelationGroupParameters(size_t cpGroupIndex,
                                      double a,
                                      double alpha,
                                      double beta,
                                      double tau);
      //> Sets the correlation parameter values for the group given by
      //  cpGroupIndex.  The correlation parameters a, alpha, and beta are
      //  unitless, and tau is in seconds.
      //
      //  Precondition:
      //  * 0 <= cpGroupIndex < numCPGroups
      //  * 0.0 <= a <= 1.0
      //  * 0.0 <= alpha <= 1.0
      //  * 0.0 <= beta <= 10.0
      //  * 0.0 < tau
      //<

   void setCorrelationGroupParameters(size_t            cpGroupIndex,
                                      const Parameters& params);
      //> Sets the values of the correlation parameters in params for the group
      //  given by cpGroupIndex.
      //
      //  Precondition:
      //  * 0 <= cpGroupIndex < numCPGroups
      //
      //  Throws a csm::Error if cpGroupIndex or any of the correlation
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
