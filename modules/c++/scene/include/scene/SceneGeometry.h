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
     *  A row and column vector for the image plane may optionally be
     *  provided, although it will not be used for initial computations.
     *  These may be passed by address, and this object will not take ownership
     *  of them
     *
     */
    SceneGeometry(Vector3 arpVel, Vector3 arpPos, Vector3 refPos, 
                  Vector3* row = NULL,
                  Vector3* col = NULL);

    //!  Destroy the object.  We do not take ownership of the row/col
    virtual ~SceneGeometry() {}

    /*!
     *  Set the image vectors.  This must be done prior to any computations
     *  involving the image geometry
     */
    virtual void setImageVectors(Vector3* row, Vector3* col);

    /*!
     *  This produces the image row vector as it is currently stored
     *
     */
    virtual Vector3 getRowVector() const;
    virtual Vector3 getColVector() const;
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
    virtual Vector3 getNorthVector() const;
    virtual double getNorthAngle() const;
    virtual Vector3 getLayoverVector() const;
    virtual AngleMagnitude getLayover() const;
    virtual Vector3 getShadowVector() const;
    virtual AngleMagnitude getShadow() const;
    virtual void getGroundResolution(double resRg, double resAz, 
                                     double& row, double& col) const;

protected:
    //! ARP Velocity vector
    Vector3 mVa;

    //! Ground track vector (nadir)
    Vector3 mVg;
    
    //! Range vector in ground nadir
    Vector3 mRg;

    //! ARP Position vector
    Vector3 mPa;
    
    //! ORP Position vector
    Vector3 mPo;

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

    //! Image plane row vector
    Vector3* mR;

    //! Image plane column vector
    Vector3* mC;


    //! North vector
    Vector3 mNorth;

};

}

#endif
