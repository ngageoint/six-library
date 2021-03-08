//#############################################################################
//
//    FILENAME:          SettableEllipsoid.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for abstract class that is to provide support for sensor modeling 
//    for imagery of planets and moons.  A planetary sensor model would inherit
//    from both the specific type of model (for example, csm::RasterGM)
//    and this class.  Any model that does not inherit from this class implements
//    the WGS-84 ellipsoid. Since the family name does not change with the use of 
//    this class, the only way to determine if a sensor model allows non WGS-84  
//    ellipsoids is to attempt a dynamic cast to this class.
//
//    The planet's ellipsoid is described by setting the semi-major and
//    semi-minor radii of the ellipsoid. If none is set, the default is the 
//    WGS-84 Earth ellipsoid.
//
//    The heights used in the methods csm::RasterGM::imageToGround() and
//    csm::RasterGM::getValidHeightRange() are then relative to the ellipsoid
//    reported here.
//
//    LIMITATIONS: None
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     18-SEP-2016   Krinsky  Initial version.
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_SETTABLE_ELLIPSOID_H
#define __CSM_SETTABLE_ELLIPSOID_H

#include "csm.h"
#include "Ellipsoid.h"

namespace csm 
{
   class Model;

   class CSM_EXPORT_API SettableEllipsoid
   {
   public:

      virtual csm::Ellipsoid getEllipsoid() const;
      //> This method returns the planetary ellipsoid.
      //<

      static Ellipsoid getEllipsoid(
         const csm::Model *model);
      //> This method returns the planetary ellipsoid.
      //  If the input model can not be cast to SettableEllipsoid*,
      //  then the ellipsoid is assumed to be WGS-84.
      //<

   protected:

      SettableEllipsoid();
      //> Constructor is protected method.
      //<

      SettableEllipsoid( const csm::Ellipsoid &ellipsoid );
      //> Constructor is protected method.
      //<
 
      virtual ~SettableEllipsoid() {}

      virtual void setEllipsoid(
         const csm::Ellipsoid &ellipsoid);
      //> This method sets the planetary ellipsoid.
      //<


   protected:
      Ellipsoid mEllipsoid;
   };

} // namespace csm

#endif
