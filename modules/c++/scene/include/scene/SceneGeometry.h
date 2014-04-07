/* =========================================================================
 * This file is part of scene-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
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
    Vector3 getGroundTrack() const { return mVg; }
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

    double getGrazingAngle(const Vector3& normalVec) const;
    double getETPGrazingAngle() const
    {
        return getGrazingAngle(mZg);
    }
    double getOPGrazingAngle() const
    {
        return getGrazingAngle(getOPZVector());
    }

    double getTiltAngle(const Vector3& normalVec) const;
    double getETPTiltAngle() const
    {
        return getTiltAngle(mZg);
    }
    double getOPTiltAngle() const
    {
        return getTiltAngle(getOPZVector());
    }

    double getDopplerConeAngle() const;
    double getSquintAngle() const;

    double getSlopeAngle(const Vector3& normalVec) const;
    double getETPSlopeAngle() const
    {
        return getSlopeAngle(mZg);
    }
    double getOPSlopeAngle() const
    {
        return getSlopeAngle(getOPZVector());
    }

    double getAzimuthAngle() const;
    double getRotationAngle() const;
    Vector3 getMultiPathVector() const;
    double getMultiPathAngle() const;

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
     * The north angle (in [-180, 180] degrees) in the pixel grid
     */
    double getNorthAngle() const;

    double getHeadingAngle() const;

    /*!
     * The layover vector in the pixel grid
     */
    Vector3 getLayoverVector() const;

    /*!
     * The layover angle (in [-180, 180] degrees) and magnitude in the pixel
     * grid
     */
    AngleMagnitude getLayover() const;

    /*
     * The layover angle (in [-180, 180] degrees) in the earth tangent plane
     * (ETP)
     */
    double getETPLayoverAngle() const;

    Vector3 getShadowVector() const;
    AngleMagnitude getShadow() const;
    types::RowCol<double>
    getGroundResolution(const types::RgAz<double>& res) const;

private:
    // Noncopyable
    SceneGeometry(const SceneGeometry& );
    const SceneGeometry& operator=(const SceneGeometry& );

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
