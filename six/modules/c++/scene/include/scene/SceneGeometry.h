/* =========================================================================
 * This file is part of scene-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * scene-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SCENE_SCENE_GEOMETRY_H__
#define __SCENE_SCENE_GEOMETRY_H__

#include <scene/sys_Conf.h>
#include <mem/ScopedCopyablePtr.h>
#include <scene/Types.h>
#include <types/RgAz.h>
#include <types/RowCol.h>

namespace scene
{
class SceneGeometry
{
public:
    /*!
     *  Establishes the scene, computing the slant plane, ground plane
     *  normal, ground track and ground range vector.
     *
     *  No computations involving the image geometry can be performed prior
     *  to calling setImageVectors().
     *
     *  No computations involving the output plane geometry can be performed
     *  prior to calling setOutputPlaneVectors().
     *
     */
    SceneGeometry(const Vector3& arpVel,
                  const Vector3& arpPos,
                  const Vector3& refPos);

    /*!
     *
     *  Same as above but with a row and column vector for the image
     *  (i.e. slant) plane provided.  They will not be used for initial
     *  computations, but computations involving the image geometry can be
     *  immediately performed
     *  rather than needing to call setImageVectors() first.
     *
     */
    SceneGeometry(const Vector3& arpVel,
                  const Vector3& arpPos,
                  const Vector3& refPos,
                  const Vector3& imageRow,
                  const Vector3& imageCol);

    /*!
     *
     *  Same as above but with a row and column vector for the output plane
     *  provided as well.  They will not be used for initial computations, but
     *  computations involving the output plane geometry can be
     *  immediately performed rather than needing to call
     *  setOutputPlaneVectors() first.
     *
     */
    SceneGeometry(const Vector3& arpVel,
                  const Vector3& arpPos,
                  const Vector3& refPos,
                  const Vector3& imageRow,
                  const Vector3& imageCol,
                  const Vector3& opX,
                  const Vector3& opY);

    /*!
     *  Set the image (i.e. slant) vectors.  This must be done prior to any
     *  computations involving the image geometry if they were not provided at
     *  construction time.
     */
    void setImageVectors(const Vector3& row, const Vector3& col);

    /*!
     *  Set the output plane vectors.  This must be done prior to any
     *  computations involving the output plane geometry if they were not
     *  provided at construction time.
     */
    void setOutputPlaneVectors(const Vector3& opX, const Vector3& opY);

    /*!
     *  This produces the image row vector as it is currently stored
     *
     */
    Vector3 getImageRowVector() const;

    /*!
     *  This produces the image col vector as it is currently stored
     *
     */
    Vector3 getImageColVector() const;

    Vector3 getOPXVector() const;
    Vector3 getOPYVector() const;
    Vector3 getOPZVector() const;

    Vector3 getARPPosition() const { return mPa; }
    Vector3 getARPVelocity() const { return mVa; }
    Vector3 getReferencePosition() const { return mPo; }

    Vector3 getGroundTrack(const Vector3& normalVec) const;

    Vector3 getETPGroundTrack() const
    {
        return getGroundTrack(mZg);
    }

    Vector3 getOPGroundTrack() const
    {
        return getGroundTrack(getOPZVector());
    }

    Vector3 getGroundRange() const { return mRg; }
    SideOfTrack getSideOfTrack() const;
    double getImageAngle(const Vector3& vec) const;
    Vector3 getSlantPlaneX() const { return mXs; }
    Vector3 getSlantPlaneY() const { return mYs; }
    Vector3 getSlantPlaneZ() const { return mZs; }
    Vector3 getGroundPlaneNormal() const { return mZg; }

    // In general, if you are interested in populating a SICD/SIDD with
    // appropriate metadata, you want to use the ETP (Earth Tangent Plane)
    // versions of these functions.  If you are interested in populating a
    // TRE-based NITF with the appropriate metadata, you want to use the OP
    // (Output Plane) versions of these functions.

    /*
     * Returns the grazing angle with respect to an arbitrary normal vector in
     * [-90, 90] degrees
     */
    double getGrazingAngle(const Vector3& normalVec) const;

    /*
     * Returns the grazing angle with respect to the ETP in [-90, 90] degrees
     *
     * This implements Section 4.9 of SICD / Section 6.4.4 of SIDD and can
     * be assigned directly to sicdData.scpcoa->grazeAngle or
     * siddData.exploitationFeatures->collections[idx]->geometry->graze
     */
    double getETPGrazingAngle() const
    {
        return getGrazingAngle(mZg);
    }

    /*
     * Returns the grazing angle with respect to the output plane in [-90, 90]
     * degrees
     */
    double getOPGrazingAngle() const
    {
        return getGrazingAngle(getOPZVector());
    }

    /*
     * Returns the tilt (or twist) angle with respect to an arbitrary normal
     * vector in [-180, 180] degrees
     */
    double getTiltAngle(const Vector3& normalVec) const;

    /*
     * Returns the tilt (or twist) angle with respect to the ETP in
     * [-180, 180] degrees
     *
     * This implements Section 4.9 of SICD / Section 6.4.5 of SIDD and can be
     * assigned directly to sicdData.scpCoa->twistAngle or
     * siddData.exploitationFeatures->collections[idx]->geometry->tilt
     */
    double getETPTiltAngle() const
    {
        return getTiltAngle(mZg);
    }

    /*
     * Returns the tilt (or twist) angle with respect to the output plane in
     * [-180, 180] degrees
     */
    double getOPTiltAngle() const
    {
        return getTiltAngle(getOPZVector());
    }

    /*
     * Returns the doppler cone angle in [0, 180) degrees
     *
     * This implements Section 4.9 of SICD / Section 7.5.3 of SIDD 2.0
     * and can be assigned directly to sicdData.scpCoa->dopplerConeAngle or
     * siddData.exploitationFeatures->collections[idx]->geometry->dopplerConeAngle
     */
    double getDopplerConeAngle() const;

    /*
     * Returns the ground plane squint angle in [0, 180] degrees
     *
     * Defined as zero looking straight forward, +90 at broadside, and +180
     * looking straight backwards regardless of looking to the right or left
     *
     * This implements the ground plane squint angle in Section 6.4.3 of SIDD
     * and can be assigned directly to
     * siddData.exploitationFeatures->collections[idx]->geometry->squint
     */
    double getSquintAngle() const;

    /*
     * Returns the slope angle with respect to an arbitrary normal vector in
     * [0, 180] degrees
     */
    double getSlopeAngle(const Vector3& normalVec) const;

    /*
     * Returns the slope angle with respect to the ETP in [0, 180] degrees
     *
     * This implements Section 4.9 of SICD / Section 6.4.2 of SIDD and can be
     * assigned directly to sicdData.scpCoa->slopeAngle or
     * siddData.exploitationFeatures->collections[idx]->geometry->slope
     */
    double getETPSlopeAngle() const
    {
        return getSlopeAngle(mZg);
    }

    /*
     * Returns the slope angle with respect to the output plane in [0, 180]
     * degrees
     */
    double getOPSlopeAngle() const
    {
        return getSlopeAngle(getOPZVector());
    }

    /*
     * Returns the azimuth angle in [0, 360] degrees
     *
     * This implements Section 4.9 of SICD / Section 6.4.1 of SIDD and can be
     * assigned directly to sicdData.scpCoa->azimuthAngle or
     * siddData.exploitationFeatures->collections[idx]->geometry->azimuth
     */
    double getAzimuthAngle() const;

    double getRotationAngle() const;

    Vector3 getMultiPathVector(const Vector3& normalVec) const;

    Vector3 getMultiPathVector() const
    {
        return getMultiPathVector(getOPZVector());
    }

    /*
     * Returns the multipath angle in [-180, 180] degrees
     *
     * This implements Section 6.5.5 of SIDD and can be assigned directly to
     * siddData.exploitationFeatures->collections[idx]->phenomenology->multiPath
     */
    double getMultiPathAngle() const;

    /*!
     *
     * Returns the CCW angle from increasing row direction to ground track at
     * the center of the image in [-180, 180] degrees
     *
     * This implements Section 6.5.6 of SIDD and can be assigned directly to
     * siddData.exploitationFeatures->collections[idx]->phenomenology->groundTrack
     */
    double getOPGroundTrackAngle() const;

    /*!
     * The angle to vec (in [0, 360) degrees CW+) in the output plane from the
     * y vector of the output plane
     */
    double getOPAngle(const Vector3& vec) const;
    double getOPNorthAngle() const;
    double getOPLayoverAngle() const;
    double getOPShadowAngle() const;

    /*!
     * The north vector in the pixel grid
     */
    Vector3 getNorthVector() const;

    /*
     * The north vector at scene center
     */
    Vector3 getSceneCenterNorthVector() const
    {
        return mNorth;
    }

    /*!
     * The north angle in [-180, 180] degrees in the pixel grid
     *
     * This implements Section 6.5.3 of SIDD and can be assigned directly to
     * siddData.exploitationFeatures->product.north
     */
    double getNorthAngle() const;

    double getHeadingAngle() const;

    /*!
     * The layover vector in the pixel grid
     */
    Vector3 getLayoverVector(const Vector3& planeZ) const;

    /*!
     * The layover vector in the pixel grid
     * Uses the cross product of the image row and column vectors as the
     * z vector
     */
    Vector3 getLayoverVector() const;

    /*!
     * The layover angle (in [-180, 180) degrees) and magnitude in the pixel
     * grid
     *
     * This implements Section 6.5.2 of SIDD and can be assigned directly to
     * siddData.exploitationFeatures->collections[idx]->phenomenology->layover
     */
    AngleMagnitude getLayover() const;

    /*
     * The layover angle (in [0, 360] degrees) in the earth tangent plane
     * (ETP).  Note that this is different than the angle component of the
     * getLayover() method above for convenience when using to assign SICD
     * metadata.
     *
     * This implements Section 4.9 of SICD and can be assigned directly to
     * sicdData.scpCoa->layoverAngle
     */
    double getETPLayoverAngle() const;

    Vector3 getShadowVector() const;

    /*
     * The shadow angle (in [-180, 180) degrees) and magnitude
     *
     * This implements Section 6.5.1 of SIDD and can be assigned directly to
     * siddData.exploitationFeatures->collections[idx]->phenomenology->shadow
     */
    AngleMagnitude getShadow() const;

    types::RowCol<double>
    getGroundResolution(const types::RgAz<double>& res) const;

private:
    void initialize();

protected:
    //! ARP Velocity vector
    const Vector3 mVa;

    //! Ground track vector (nadir)
    Vector3 mVg;

    //! Range vector in ground nadir
    Vector3 mRg;

    //! ARP Position vector
    const Vector3 mPa;

    //! ORP Position vector
    const Vector3 mPo;

    //! Slant plane range vector (LOS)
    Vector3 mXs;

    //! Completes the right-handed system
    Vector3 mYs;

    //! Normal to slant plane
    Vector3 mZs;

    //! Normal to the ground (also Up vector)
    Vector3 mZg;

    //! AKA Look direction
    int mSideOfTrack;

    //! North vector
    Vector3 mNorth;

private:
    //! Image (i.e. slant) plane row and col vectors
    mem::ScopedCopyablePtr<const Vector3> mImageRow;
    mem::ScopedCopyablePtr<const Vector3> mImageCol;

    //! Output plane x/y/z vectors
    mem::ScopedCopyablePtr<const Vector3> mXo;
    mem::ScopedCopyablePtr<const Vector3> mYo;
    mem::ScopedCopyablePtr<const Vector3> mZo;
};
}

#endif
