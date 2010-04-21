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
#ifndef __SCENE_COORDINATE_TYPES_H__
#define __SCENE_COORDINATE_TYPES_H__

#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>

#ifdef WIN32
#define _USE_MATH_DEFINES
#endif
#include <math.h>

#include "math/linear/MatrixMxN.h"
#include "math/linear/VectorN.h"

namespace scene
{

    const double METERS_TO_FEET = 3.280839895013122;
    const double DEGREES_TO_RADIANS = M_PI/180.0;
    const double RADIANS_TO_DEGREES = 180.0/M_PI;

    typedef math::linear::VectorN<3> Vector3;

    enum SideOfTrack {
        TRACK_LEFT = 1,
        TRACK_RIGHT = -1
    };

    enum Units {
        FEET,
        METERS,
        INVALID_UNITS
    };
    
    enum AngularUnits {
        DEGREES,
        RADIANS,
        INVALID_ANGULAR_UNITS
    };
    
    enum XYZEnum
    {
        XYZ_X, XYZ_Y, XYZ_Z
    };
    
    struct AngleMagnitude
    {
        AngleMagnitude(double _angle = 0, double _magnitude = 0) :
            angle(_angle), magnitude(_magnitude)
        {
        }
        double angle;
        double magnitude;

        bool operator==(AngleMagnitude am) const
        {
            return (angle == am.angle && magnitude == am.magnitude);
        }

        bool operator!=(AngleMagnitude am) const
        {
            return (angle != am.angle || magnitude != am.magnitude);
        }
    };

    class LatLonAlt
    {
    public:
        LatLonAlt() : mLat(0), mLon(0), mAlt(0) {}

        LatLonAlt(double lat, double lon, double alt = 0)
            : mLat(lat), mLon(lon), mAlt(alt)
        {
        }


        LatLonAlt(const LatLonAlt& lla)
        {
            mLat = lla.mLat;
            mLon = lla.mLon;
            mAlt = lla.mAlt;
            
        }

        LatLonAlt& operator=(const LatLonAlt& lla)
        {
            if (this != &lla)
            {
                mLat = lla.mLat;
                mLon = lla.mLon;
                mAlt = lla.mAlt;

            }
            return *this;
        }

        double getLat() const
        {
            return mLat;
        }
        double getLon() const
        {
            return mLon;
        }

        double getLatRadians() const
        {
            return mLat * DEGREES_TO_RADIANS;
        }
        double getLonRadians() const
        {
            return mLon * DEGREES_TO_RADIANS;
        }

        void setLat(double lat)
        {
            mLat = lat;
        }
        void setLon(double lon)
        {
            mLon = lon;
        }

        void setLatRadians(double lat)
        {
            mLat = (lat * RADIANS_TO_DEGREES);
        }
        void setLonRadians(double lon)
        {
            mLon = (lon * RADIANS_TO_DEGREES);
        }

        double getAlt() const
        {
            return mAlt;
        }

        void setAlt(double alt)
        {
            mAlt = alt;
        }

        bool operator==(const LatLonAlt& x) const
        {
            return mLat == x.mLat && mLon == x.mLon && mAlt == x.mAlt;
        }

    protected:
        double mLat;
        double mLon;
        double mAlt;
    };

}

namespace str
{
template<> 
    scene::SideOfTrack toType<scene::SideOfTrack>(const std::string& s);

template<>
    std::string toString(const scene::SideOfTrack& value);
}
#endif
