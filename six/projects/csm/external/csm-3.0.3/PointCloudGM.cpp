//#############################################################################
//
//    FILENAME:          PointCloudGM.cpp
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for abstract class that is to provide a common interface from
//    which CSM point cloud geometric models will inherit.  It is derived from
//    the GeometricModel class.
//
//    This sensor model represents a transformation between ECEF space and a
//    three-dimensional "model" space that is defined by the model.  Even
//    though model coordinates are defined by the model
//
//    LIMITATIONS:       None
//
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     18-Feb-2013   DME      Initial version.
//     27-May-2015   DME      Replaced getValidModelRange function with
//                            getValidModelBounds function.
//     22-Feb-2018   JPK      Replaced macros for inclusion in a single
//                            library.  Reformatted for readability
//     26-Sep-2018   SCM      Added define of CSM_LIBRARY for Windows.
//
//    NOTES:
//
//#############################################################################

#define CSM_LIBRARY

#include <algorithm>
#include "PointCloudGM.h"

namespace csm
{
//*****************************************************************************
// PointCloudGM::getFamily
//*****************************************************************************
std::string PointCloudGM::getFamily() const
{
   return GeometricModel::getFamily() + std::string(CSM_POINTCLOUD_FAMILY);
}

//*****************************************************************************
// PointCloudGM::computeAllSensorPartials
//*****************************************************************************
std::vector<PointCloudGM::SensorPartials>
PointCloudGM::computeAllSensorPartials(const EcefCoord& groundPt,
                                       param::Set pSet,
                                       double desiredPrecision,
                                       double* achievedPrecision,
                                       WarningList* warnings) const
{
   std::vector<int> paramIndices = this->getParameterSetIndices(pSet);
   std::vector<PointCloudGM::SensorPartials> partials(paramIndices.size());
   if (achievedPrecision == NULL)
   {
      for (std::size_t i = 0; i < paramIndices.size(); ++i)
      {
         partials[i] = computeSensorPartials(paramIndices[i],
                                             groundPt,
                                             desiredPrecision,
                                             achievedPrecision,
                                             warnings);
      }
   }
   else
   {
      *achievedPrecision = 0.0;
      for (std::size_t i = 0; i < paramIndices.size(); ++i)
      {
         double tmp;
         partials[i] = computeSensorPartials(paramIndices[i],
                                             groundPt,
                                             desiredPrecision,
                                             &tmp,
                                             warnings);
         
         *achievedPrecision = std::max(*achievedPrecision, tmp);
      }
   }
   return partials;
}

//*****************************************************************************
// PointCloudGM::computeAllSensorPartials
//*****************************************************************************
std::vector<PointCloudGM::SensorPartials>
PointCloudGM::computeAllSensorPartials(const ModelCoord& modelPt,
                                       const EcefCoord& groundPt,
                                       param::Set pSet,
                                       double desiredPrecision,
                                       double* achievedPrecision,
                                       WarningList* warnings) const
{
   std::vector<int> paramIndices = this->getParameterSetIndices(pSet);
   std::vector<PointCloudGM::SensorPartials> partials(paramIndices.size());
   if (achievedPrecision == NULL)
   {
      for (std::size_t i = 0; i < paramIndices.size(); ++i)
      {
         partials[i] = computeSensorPartials(paramIndices[i],
                                             modelPt,
                                             groundPt,
                                             desiredPrecision,
                                             achievedPrecision,
                                             warnings);
      }
   }
   else
   {
      *achievedPrecision = 0.0;
      for (std::size_t i = 0; i < paramIndices.size(); ++i)
      {
         double tmp;
         partials[i] = computeSensorPartials(paramIndices[i],
                                             modelPt,
                                             groundPt,
                                             desiredPrecision,
                                             &tmp,
                                             warnings);
         
         *achievedPrecision = std::max(*achievedPrecision, tmp);
      }
   }
   return partials;
}

} // namespace csm
