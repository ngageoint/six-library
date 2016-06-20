/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#ifndef __SIX_RMA_H__
#define __SIX_RMA_H__

#include <mem/ScopedCopyablePtr.h>

#include "six/Types.h"
#include "six/Init.h"

namespace six
{
namespace sicd
{
class GeoData;
struct Position;
/*!
 *  \struct  RMAT
 *  \brief   Parameters for Range Migration with Along Track phase
 *           stabilization.
 */
struct RMAT
{
    //!  Constructor
    RMAT();

    //! Reference time when the reference position and velocity
    //  are defined.
    //  depricated in 1.0.0
    double refTime;

    //! Platform reference position used to establish the reference
    //  trajectory line.  
    Vector3 refPos;

    //! Reference unit velocity vector used to establish the
    //  reference trajectory line.
    //  'UnitVelRef' changed to 'VelRef' in 1.0.0
    Vector3 refVel;

    //! Polynomial function that yields the distance along the
    //  reference straight line trajectory as function of time.
    //  depricated in 1.0.0
    Poly1D distRefLinePoly;

    //! Polynomial function that yields the cosine of the Doppler
    //  cone angle at COA as a function of the image location.
    //  depricated in 1.0.0
    Poly2D cosDCACOAPoly;

    //!  all depricated in 1.0.0
    double kx1;
    double kx2;
    double ky1;
    double ky2;

    //! Reference Doppler Cone Angle (degrees)
    //  added in 1.0.0
    double dopConeAngleRef;

    //! Equality operator
    bool operator==(const RMAT& rhs) const;
    bool operator!=(const RMAT& rhs) const
    {
        return !(*this == rhs);
    }

    void fillDerivedFields(const Vector3& scp);
    int look() const;
    void setSCP(const Vector3& scp);
    const Vector3& scp() const;
    Vector3 uLOS() const;
private:
    mem::ScopedCopyablePtr<Vector3> mScp;
};

/*!
 *  \struct  RMCR
 *  \brief   Parameters for Range Migration with 
 *           Cross Range Motion Compensation
 *           Added in 1.0.0
 *
 *  Note: this is a repeat of parameters in RMAT but 
 *        we recreate them to avoid confusion
 */
struct RMCR
{
    //!  Constructor
    RMCR();

    //! Platform reference position (ECF) used
    //  to establish the reference slant plane
    //  and the range direction in the image. (meters)
    Vector3 refPos;

    //! Platform reference velocity vector
    //  (ECF) used to establish the reference
    //  slant plane. (meters / sec)
    Vector3 refVel;

    //! Reference Doppler Cone Angle (degrees)
    double dopConeAngleRef;

    //! Equality operator
    bool operator==(const RMCR& rhs) const
    {
        return (refPos == rhs.refPos && refVel == rhs.refVel && 
            dopConeAngleRef == rhs.dopConeAngleRef);
    }

    bool operator!=(const RMCR& rhs) const
    {
        return !(*this == rhs);
    }

    void fillDerivedFields(const Vector3& scp);
    void setSCP(const Vector3& scp);
    const Vector3& scp() const;
    Vector3 uLOS() const;
    int look() const;
private:
    mem::ScopedCopyablePtr<Vector3> mScp;
};

/*!
 *  \struct  INCA
 *  \brief   Parameters for Imaging Near Closest Approach.
 */
struct INCA
{
    //!  Constructor
    INCA();

    //! Polynomial function that yields time of closest approach as
    //  function of image column coordinate.
    Poly1D timeCAPoly;

    //! Range as closest approach for the SCP.
    double rangeCA;

    //! RF frequency used for computing Doppler centroid values.
    double freqZero;

    //! Polynomial function that yields doppler rate scale factor as
    //  a function of image location.
    //  Poly1D deprecated in 0.4.0 --
    //      NOTE: Poly1D is contained in dopplerRateScaleFactorPoly[0]
    //  Poly2D added in 0.4.1
    Poly2D dopplerRateScaleFactorPoly;

    //! (Optional) Polynomial that yields Doppler Centroid value as a function
    //  of image location.
    Poly2D dopplerCentroidPoly;

    //! (Optional) Flag indicating that the COA at peak signal.
    BooleanType dopplerCentroidCOA;

    //! Equality operator
    bool operator==(const INCA& rhs) const;
    bool operator!=(const INCA& rhs) const
    {
        return !(*this == rhs);
    }

    void fillDerivedFields(const Vector3& scp, double fc,
            const Position& position);

    void setSCP(const Vector3& scp);
    const Vector3& scp() const;

    void setArpPoly(const PolyXYZ& arpPoly);
    const PolyXYZ& arpPoly() const;

    Vector3 caPos() const;
    Vector3 caVel() const;

    void fillDefaultFields(double fc);
private:
    mem::ScopedCopyablePtr<Vector3> mScp;
    mem::ScopedCopyablePtr<PolyXYZ> mArpPoly;
};

/*!
 *  \struct  RMA
 *  \brief   Range Migration Algorithm (RMA) parameters, included when
 *           the image is formed with RMA.
 */
struct RMA
{
    //! Constructor
    RMA();

    //!  Type of migration algorithm used.
    RMAlgoType algoType;

    //! (Choice) Parameters for range migration with along track motion
    //  compensation -- if this is present, rmcr & inca should be NULL.
    mem::ScopedCopyablePtr<RMAT> rmat;

    //! (Choice) Parameters for range migration with cross range motion
    //  compensation -- if this is present, rmat & inca should be NULL.
    //  added in version 1.0.0
    mem::ScopedCopyablePtr<RMCR> rmcr;

    //! (Choice) Parameter for imaging near closest approach image
    //  description -- if this is present, rmat & rmcr should be NULL.
    mem::ScopedCopyablePtr<INCA> inca;

    bool operator==(const RMA& rhs) const
    {
        return (algoType == rhs.algoType && rmat == rhs.rmat &&
            rmcr == rhs.rmcr && inca == rhs.inca);
    }

    bool operator!=(const RMA& rhs) const
    {
        return !(*this == rhs);
    }

    void fillDerivedFields(const GeoData& geoData,
            const Position& position,
            double fc);
    void fillDefaultFields(double fc);
};

}
}
#endif

