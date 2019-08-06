//#############################################################################
//
//    FILENAME:          csmPointCloud.cpp
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Additions were made to the CSM API in CSM 3.0.2 to handle point clouds.
//    The previous API did not change. Previously this code was in a separate
//    library, but has now been migrated into a single library as of
//    CSM3.0.3
//
//    LIMITATIONS:       None
//
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//
//    22FEB2018     JPK       Modified to use existing csm macros (since
//                            point cloud is no longer in a separate library).
//                            Other minor cleanup for consistency of code.
//     26-Sep-2018   SCM      Added define of CSM_LIBRARY for Windows.
//
//    NOTES:
//
//#############################################################################

#define CSM_LIBRARY
#include "csmPointCloud.h"
#include "Error.h"
#include <cmath>

namespace csm
{
//*****************************************************************************
// ModelBounds::ModelBounds
//*****************************************************************************
ModelBounds::ModelBounds()
{
}

//*****************************************************************************
// ModelBounds::ModelBounds
//*****************************************************************************
ModelBounds::ModelBounds(const ModelCoord& center,
                         double dim0,
                         double dim1,
                         double dim2)
   :
      theCenter (center),
      theA0     (1.0, 0.0, 0.0),
      theA1     (0.0, 1.0, 0.0),
      theA2     (0.0, 0.0, 1.0),
      theDim0   (std::abs(dim0)),
      theDim1   (std::abs(dim1)),
      theDim2   (std::abs(dim2))
{
   // check for errors in dimensions
   if ((theDim0 <= 0.0) || (theDim1 <= 0.0) || (theDim2 <= 0.0))
   {
      throw csm::Error(csm::Error::ILLEGAL_MATH_OPERATION,
                       "ModelBounds dimension is not a positive value",
                       "ModelBounds constructor");
   }
}

//*****************************************************************************
// ModelBounds::ModelBounds
//*****************************************************************************

ModelBounds::ModelBounds(const ModelCoord& center,
                         const ModelVector& axis0,
                         const ModelVector& axis1,
                         const ModelVector& axis2,
                         double dim0,
                         double dim1,
                         double dim2)
   :
      theCenter (center),
      theA0     (axis0),
      theA1     (axis1),
      theA2     (axis2),
      theDim0   (std::abs(dim0)),
      theDim1   (std::abs(dim1)),
      theDim2   (std::abs(dim2))
{
   // check for errors in dimensions
   if ((theDim0 <= 0.0) || (theDim1 <= 0.0) || (theDim2 <= 0.0))
   {
      throw csm::Error(csm::Error::ILLEGAL_MATH_OPERATION,
                       "ModelBounds dimension is not a positive value",
                       "ModelBounds constructor");
   }

   // orthonormalize _u, _v, and _w and simultaneously check for errors
   if (normalize(theA0) == 0.0)
   {
      throw csm::Error(csm::Error::ILLEGAL_MATH_OPERATION,
                       "ModelBounds axis-0 vector has zero length",
                       "ModelBounds constructor");
   }

   theA1 = orthogonalProjection(theA1, theA0);

   if (normalize(theA1) == 0.0)
   {
      throw csm::Error(csm::Error::ILLEGAL_MATH_OPERATION,
                       "ModelBounds axis-1 vector is linearly dependent on axis-0 vector",
                       "ModelBounds constructor");
   }

   theA2 = orthogonalProjection(orthogonalProjection(theA2, theA0), theA1);

   if (normalize(theA2) == 0.0)
   {
      throw csm::Error(csm::Error::ILLEGAL_MATH_OPERATION,
                       "ModelBounds axis-2 vector is linearly dependent on axis-0 and axis-1 vectors",
                       "ModelBounds constructor");
   }
}

//*****************************************************************************
// ModelBounds::contains
//*****************************************************************************
bool ModelBounds::contains(const ModelCoord& modelCoord) const
{
   ModelVector offset(modelCoord.m0 - theCenter.m0,
                      modelCoord.m1 - theCenter.m1,
                      modelCoord.m2 - theCenter.m2);

   double offset0 = offset.m0 * theA0.m0 + offset.m1 * theA0.m1 + offset.m2 * theA0.m2;
   double offset1 = offset.m0 * theA1.m0 + offset.m1 * theA1.m1 + offset.m2 * theA1.m2;
   double offset2 = offset.m0 * theA2.m0 + offset.m1 * theA2.m1 + offset.m2 * theA2.m2;

   return (std::abs(offset0) <= (0.5 * theDim0)) &&
          (std::abs(offset1) <= (0.5 * theDim1)) &&
          (std::abs(offset2) <= (0.5 * theDim2));
}

//*****************************************************************************
// ModelBounds::normalize()
//*****************************************************************************
double ModelBounds::normalize(ModelVector& v)
{
   double k = v.m0 * v.m0 + v.m1 * v.m1 + v.m2 * v.m2;
   if (k == 0.0)
   {
      return 0.0;
   }
   k = std::sqrt(k);
   v.m0 /= k;
   v.m1 /= k;
   v.m2 /= k;
   return k;
}

//*****************************************************************************
// ModelBounds::orthogonalProjection
//*****************************************************************************
ModelVector ModelBounds::orthogonalProjection(const ModelVector &u,
                                              const ModelVector &v)
{
   double vDotV = v.m0 * v.m0 + v.m1 * v.m1 + v.m2 * v.m2;
   if (vDotV == 0.0)
   {
      return u;
   }
   double k = (u.m0 * v.m0 + u.m1 * v.m1 + u.m2 * v.m2) / vDotV;
   return ModelVector(u.m0 - k * v.m0, u.m1 - k * v.m1, u.m2 - k * v.m2);
}

} // namespace csm
