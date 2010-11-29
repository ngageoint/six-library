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

#include <cmath>

#include "math/linear/MatrixMxN.h"
#include "math/linear/VectorN.h"

namespace scene
{

    const double METERS_TO_FEET = 3.280839895013122;
    const double DEGREES_TO_RADIANS = M_PI/180.0;
    const double RADIANS_TO_DEGREES = 180.0/M_PI;

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

    class LatLon
    {
    public:
        LatLon(double scalar = 0.0) :
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

        bool operator==(const LatLon& x) const
        {
            return mLat == x.mLat && mLon == x.mLon;
        }

    protected:
        double mLat;
        double mLon;
    };

    class LatLonAlt : public LatLon
    {
    public:
        LatLonAlt(double scalar = 0.0) : LatLon(scalar), mAlt(scalar)
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
        double mAlt;
    };

    /*!
     *  \struct RowCol
     *  \brief Row/Col pair type
     *
     *  Templated pair storage for row/col values.  This is typedef'd
     *  below to prevent incorrect type assigment (e.g., RowCol<int> where
     *  type should be RowCol<long>)
     */
    
    template<typename T> struct RowCol
    {
        T row;
        T col;
        
        // Splitting these in two prevents probable unintended effect
        // row being set and column being zero, instead of implicit 
        // promotion
        RowCol(T r, T c) :
            row(r), col(c) {}
        
        
        RowCol() : row((T)0.0), col((T)0.0) {}
                       
        template<typename Other_T> RowCol(const RowCol<Other_T>& p)
        {
            row = p.row;
            col = p.col;
        }
        
        RowCol(const std::pair<T, T>& p)
        {
            row = p.first;
            col = p.second;
        }
        
        template<typename Other_T> RowCol& operator=(const RowCol<Other_T>& p)
        {
            if (this != &p)
            {
                row = (T) p.row;
                col = (T) p.col;
            }
            return *this;
        }
            
        RowCol& operator=(const std::pair<T, T>& p)
        {
            row = p.first;
            col = p.second;
            return *this;
        }
            
        template<typename Other_T> RowCol& operator+=(const RowCol<Other_T>& p)
        {
            row += (T) p.row;
            col += (T) p.col;
            return *this;
        }
        
        template<typename Other_T> RowCol operator+(const RowCol<Other_T>& p)
        {
            RowCol copy(*this);
            return copy += p;
        }
        
        template<typename Other_T> RowCol& operator-=(const RowCol<Other_T>& p)
        {
            row -= (T) p.row;
            col -= (T) p.col;
            return *this;
        }
        
        template<typename Other_T> RowCol operator-(const RowCol<Other_T>& p)
        {
            RowCol copy(*this);
            return copy -= p;
        }
        
        RowCol& operator+=(T scalar)
        {
            row += scalar;
            col += scalar;
            return *this;
        }
        
        RowCol operator+(T scalar)
        {
            RowCol copy(*this);
            return copy += scalar;
        }
        
        RowCol& operator-=(T scalar)
        {
            row -= scalar;
            col -= scalar;
            return *this;
        }
        
        RowCol operator-(T scalar)
        {
            RowCol copy(*this);
            return copy -= scalar;
        }
        
        RowCol& operator*=(T scalar)
        {
            row *= scalar;
            col *= scalar;
            return *this;
        }
        
        RowCol operator*(T scalar)
        {
            RowCol copy(*this);
            return copy *= scalar;
        }
        
        RowCol& operator/=(T scalar)
        {
            row /= scalar;
            col /= scalar;
            return *this;
        }
        
        RowCol operator/(T scalar)
        {
            RowCol copy(*this);
            return copy /= scalar;
        }
        
        /*!
         *  Compare the types considering that some
         *  specializations (e.g., double)
         *  are not exact
         */
        bool operator==(const RowCol<T>& p) const
        {
            return math::linear::equals(row, p.row) && math::linear::equals(col,
                                                                            p.col);
        }
        
        bool operator!=(const RowCol<T>& p) const
        {
            return !(RowCol::operator==(p));
        }
        
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
