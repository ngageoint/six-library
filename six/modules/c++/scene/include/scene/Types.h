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
#ifndef __SCENE_COORDINATE_TYPES_H__
#define __SCENE_COORDINATE_TYPES_H__

#include <cmath>

#include "scene/sys_Conf.h"
#include <import/except.h>
#include <import/str.h>
#include <import/sys.h>
#include <types/RowCol.h>
#include <math/Constants.h>

#include "math/linear/MatrixMxN.h"
#include "math/linear/VectorN.h"

namespace scene
{
    typedef math::linear::VectorN<2> Vector2;
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
        AngleMagnitude() = default;
        AngleMagnitude(double _angle, double _magnitude = 0) :
            angle(_angle), magnitude(_magnitude)
        {
        }
        double angle = 0.0;
        double magnitude = 0.0;

        bool operator==(AngleMagnitude am) const
        {
            return (angle == am.angle && magnitude == am.magnitude);
        }

        bool operator!=(AngleMagnitude am) const
        {
            return (angle != am.angle || magnitude != am.magnitude);
        }
    };

    struct LatLonAlt;
    struct LatLon
    {
        LatLon() = default;
        LatLon(double scalar) :
            mLat(scalar), mLon(scalar)
        {
        }

        LatLon(double lat, double lon)
            : mLat(lat), mLon(lon)
        {
        }


        LatLon(const LatLon& lla)
        {
            mLat = lla.mLat;
            mLon = lla.mLon;
        }

        LatLon& operator=(const LatLon& lla)
        {
            if (this != &lla)
            {
                mLat = lla.mLat;
                mLon = lla.mLon;
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
            return mLat * math::Constants::DEGREES_TO_RADIANS;
        }
        double getLonRadians() const
        {
            return mLon * math::Constants::DEGREES_TO_RADIANS;
        }

        void setLat(double lat)
        {
            mLat = lat;
        }
        void setLon(double lon)
        {
            mLon = lon;
        }
        void setLatLon(const LatLonAlt&);

        void setLatRadians(double lat)
        {
            mLat = (lat * math::Constants::RADIANS_TO_DEGREES);
        }

        void setLonRadians(double lon)
        {
            mLon = (lon * math::Constants::RADIANS_TO_DEGREES);
        }

        bool operator==(const LatLon& x) const
        {
            return mLat == x.mLat && mLon == x.mLon;
        }

        bool operator!=(const LatLon& x) const
        {
            return !(*this == x);
        }

    protected:
        double mLat = 0.0;
        double mLon = 0.0;
    };

    struct LatLonAlt : public LatLon
    {
        LatLonAlt() = default;
        LatLonAlt(double scalar) : LatLon(scalar), mAlt(scalar)
        {
        }

        LatLonAlt(double lat, double lon, double alt = 0)
            : LatLon(lat, lon), mAlt(alt)
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

        using LatLon::getLat;
        using LatLon::getLon;
        using LatLon::getLatRadians;
        using LatLon::getLonRadians;
        using LatLon::setLat;
        using LatLon::setLon;
        using LatLon::setLatRadians;
        using LatLon::setLonRadians;

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
        double mAlt = 0.0;
    };

    inline void LatLon::setLatLon(const LatLonAlt& lla)
    {
        setLat(lla.getLat());
        setLon(lla.getLon());
    }
}

namespace str
{
template<>
    scene::SideOfTrack toType<scene::SideOfTrack>(const std::string& s);

template<>
    std::string toString(const scene::SideOfTrack& value);
}
#endif

