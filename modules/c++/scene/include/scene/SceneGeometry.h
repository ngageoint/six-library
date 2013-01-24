/* =========================================================================
 * This file is part of scene-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include <memory>

#include "scene/Types.h"
// For ECEF/LLA conversions
#include "scene/Utilities.h"

namespace scene
{

//! \todo Remove LocalPlane*.  This should be an ETP.  Using another plane
//! may produce incorrect results.
//! To compute non-scene angles, you must have an image plane!
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
     */
    SceneGeometry(const Vector3& arpVel,
                  const Vector3& arpPos,
                  const Vector3& refPos);

    /*!
     *
     *  Same as above but with a row and column vector for the image plane
     *  provided.  They will not be used for initial computations, but
     *  computations involving the image geometry can be immediately performed
     *  rather than needing to call setImageVectors() first.
     *
     */
    SceneGeometry(const Vector3& arpVel,
                  const Vector3& arpPos,
                  const Vector3& refPos,
                  const Vector3& row,
                  const Vector3& col);

    virtual ~SceneGeometry();

    /*!
     *  Set the image vectors.  This must be done prior to any computations
     *  involving the image geometry if they were not provided at construction
     *  time.
     */
    void setImageVectors(const Vector3& row, const Vector3& col);

    /*!
     *  This produces the image row vector as it is currently stored
     *
     */
    Vector3 getRowVector() const;
    Vector3 getColVector() const;
    virtual Vector3 getARPPosition() const { return mPa; }
    virtual Vector3 getARPVelocity() const { return mVa; }
    virtual Vector3 getReferencePosition() const { return mPo; }
    virtual Vector3 getGroundTrack() const { return mVg; }
    virtual Vector3 getGroundRange() const { return mRg; }
    virtual SideOfTrack getSideOfTrack() const;
    virtual double getImageAngle(Vector3 vec) const;
    virtual Vector3 getSlantPlaneX() const { return mXs; }
    virtual Vector3 getSlantPlaneY() const { return mYs; }
    virtual Vector3 getSlantPlaneZ() const { return mZs; }
    virtual Vector3 getGroundPlaneNormal() const { return mZg; }
    virtual double getGrazingAngle() const;
    virtual double getTiltAngle() const;
    virtual double getDopplerConeAngle() const;
    virtual double getSquintAngle() const;
    virtual double getSlopeAngle() const;
    virtual double getAzimuthAngle() const;
    virtual double getRotationAngle() const;
    virtual Vector3 getMultiPathVector() const;
    virtual double getMultiPathAngle() const;

    /*!
     * The north vector in the pixel grid
     */
    virtual Vector3 getNorthVector() const;

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
    virtual double getNorthAngle() const;

    virtual double getHeadingAngle() const;

    /*!
     * The layover vector in the pixel grid
     */
    virtual Vector3 getLayoverVector() const;

    /*!
     * The layover angle (in [-180, 180] degrees) and magnitude in the pixel
     * grid
     */
    virtual AngleMagnitude getLayover() const;

    /*
     * The layover angle (in [-180, 180] degrees) in the earth tangent plane
     * (ETP)
     */
    double getETPLayoverAngle() const;

    virtual Vector3 getShadowVector() const;
    virtual AngleMagnitude getShadow() const;
    virtual void getGroundResolution(double resRg, double resAz,
                                     double& row, double& col) const;

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
    //! Image plane row vector
    std::auto_ptr<const Vector3> mR;

    //! Image plane column vector
    std::auto_ptr<const Vector3> mC;
};

}

#endif
