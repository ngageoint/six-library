//#############################################################################
//
//    FILENAME:          Ellipsoid.cpp
//
//    CLASSIFICATION:    Unclassified
//
//    DESCRIPTION:
//
//    This file provides implementation for methods declared in the
//    Ellipsoid class.
//
//    SOFTWARE HISTORY:
//     Date          Author   Comment
//     -----------   ------   -------
//     06-Aug-2016   Krinsky  Initial Coding
//
//#############################################################################

#ifndef CSM_LIBRARY
#define CSM_LIBRARY
#endif

#include "Ellipsoid.h"
#include <math.h>

namespace csm 
{
   //*****************************************************************************
   // Ellipsoid::Ellipsoid()
   //*****************************************************************************
   Ellipsoid::Ellipsoid()
   {
       setEllipsoid(CSM_WGS84_SEMI_MAJOR_AXIS, CSM_WGS84_SEMI_MINOR_AXIS);
   }

   //**************************************************************************
   // Ellipsoid::Ellipsoid()
   //**************************************************************************
   Ellipsoid::Ellipsoid(
      double semiMajorRadius,
      double semiMinorRadius)
   {
       setEllipsoid( semiMajorRadius, semiMinorRadius );
   }

   //**************************************************************************
   // Ellipsoid::~Ellipsoid()
   //**************************************************************************
   Ellipsoid::~Ellipsoid() {}

   double Ellipsoid::calculateHeight(
      const csm::EcefCoord& geocentric,
      double                desiredPrecision,
      double*               achievedPrecision,
      csm::WarningList*     warnings) const
   {
      const int MKTR = 10;
      double eccSquare = 1.0 - mSemiMinorAxis * mSemiMinorAxis / mSemiMajorAxis / mSemiMajorAxis;
      double ep2 = 1.0 - eccSquare;
      double d2 = geocentric.x * geocentric.x + geocentric.y * geocentric.y;
      double d = sqrt(d2);
      double h = 0.0;
      int ktr = 0;
      double hPrev, r;

      // Suited for points near equator
      if (d >= geocentric.z)
      {
         double tt, zz, n;
         double tanPhi = geocentric.z / d;
         do
         {
            hPrev = h;
            tt = tanPhi * tanPhi;
            r = mSemiMajorAxis / sqrt(1.0 + ep2 * tt);
            zz = geocentric.z + r * eccSquare * tanPhi;
            n = r * sqrt(1.0 + tt);
            h = sqrt(d2 + zz * zz) - n;
            tanPhi = zz / d;
            ktr++;
         } while (MKTR > ktr && fabs(h - hPrev) > desiredPrecision);
      }

      // Suited for points near the poles
      else
      {
         double cc, dd, nn;
         double cotPhi = d / geocentric.z;
         do
         {
            hPrev = h;
            cc = cotPhi * cotPhi;
            r = mSemiMajorAxis / sqrt(ep2 + cc);
            dd = d - r * eccSquare * cotPhi;
            nn = r * sqrt(1.0 + cc) * ep2;
            h = sqrt(dd * dd + geocentric.z * geocentric.z) - nn;
            cotPhi = dd / geocentric.z;
            ktr++;
         } while (MKTR > ktr && fabs(h - hPrev) > desiredPrecision);
      }

      if (achievedPrecision)
      {
         *achievedPrecision = fabs(h - hPrev);
      }

      return h;
   }

} // namespace csm
