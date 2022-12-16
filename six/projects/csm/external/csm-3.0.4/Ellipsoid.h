//#############################################################################
//
//    FILENAME:          Ellipsoid.h
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    Header for abstract class that is to provide support for non-WGS-84
//    ellipsoids. This is needed to support for imagery of planetary bodies
//    in addition to the Earth.
//
//    The planet's ellipsoid is described by setting the semi-major and
//    semi-minor radii of the ellipsoid. If none is set, the default is the 
//    WGS-84 Earth ellipsoid.
//
//    The heights used in the methods csm::RasterGM::imageToGround() and
//    csm::RasterGM::getValidHeightRange() are then relative to the ellipsoid
//    defined here.
//    
//
//    LIMITATIONS: None
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     18-AUG-2016   Krinsky  Initial version.
//
//    NOTES:
//
//#############################################################################

#ifndef __CSM_ELLIPSOID_H
#define __CSM_ELLIPSOID_H

#include "csm.h"
#include "Warning.h"

#define CSM_WGS84_SEMI_MAJOR_AXIS 6378137.000
#define CSM_WGS84_SEMI_MINOR_AXIS 6356752.3142451794975639665996337

namespace csm 
{
   class CSM_EXPORT_API Ellipsoid
   {

   public:
   
      Ellipsoid();
	  //> This constructor defaults to the WGS-84 ellipsoid.
	  //<

      Ellipsoid(
         double semiMajorRadius,
         double semiMinorRadius);
	  //> This constructor sets the ellipsoid to that specified.
	  //<

      virtual ~Ellipsoid();
	  //> Virtual destructor allows for inheritance.
	  //<

      virtual double getSemiMajorRadius() const
      {
         return mSemiMajorAxis;
      }
      //> This method returns the semi-major radius
      //  describing the planetary ellipsoid.
      //<

      virtual double getSemiMinorRadius() const
      {
         return mSemiMinorAxis;
      }
      //> This method returns the semi-major radius
      //  describing the planetary ellipsoid.
      //<

      virtual double calculateHeight(
         const csm::EcefCoord& geocentric,
         double                desiredPrecision = 0.0001,
         double*               achievedPrecision = NULL,
         csm::WarningList*     warnings = NULL ) const;
      //> This method determines the height above the planetary
      //  ellipsoid of the input ground point.  Note that while 
      //  the input point uses an ECEF coordinate, the point is
      //  fixed to the same planetary object as the ellipsoid.
      //<

      virtual void setEllipsoid(
         double semiMajorRadius,
         double semiMinorRadius)
      {
         mSemiMajorAxis = semiMajorRadius;
         mSemiMinorAxis = semiMinorRadius;
      }
      //> This method sets the semi-major and semi-minor 
      //  radii describing the planetary ellipsoid.
      //<
 
   protected:

      double mSemiMajorAxis;
      double mSemiMinorAxis;
   };

} // namespace csm

#endif
