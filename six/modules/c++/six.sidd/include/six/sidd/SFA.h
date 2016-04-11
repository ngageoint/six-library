/* =========================================================================
 * This file is part of six.sidd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six.sidd-c++ is free software; you can redistribute it and/or modify
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
#ifndef __SIX_SIDD_SFA_H__
#define __SIX_SIDD_SFA_H__

#include <import/six.h>

namespace six
{
namespace sidd
{

struct SFATyped
{
public:
    virtual ~SFATyped()
    {
    }

    virtual SFATyped* clone() const = 0;

    inline std::string getType() const
    {
        return mType;
    }

protected:
    std::string mType;
    SFATyped(const std::string& typeName) :
        mType(typeName)
    {
    }
};

struct SFAGeometry : public SFATyped
{
    virtual SFAGeometry* clone() const
    {
        return new SFAGeometry(*this);
    }

protected:
    SFAGeometry(const std::string& typeName) :
        SFATyped(typeName)
    {
    }
};

struct SFAPoint : public SFAGeometry
{
    double x, y, z, m;

    SFAPoint() :
        SFAGeometry(TYPE_NAME), 
        x(0), 
        y(0),
        z(::six::Init::undefined<double>()),
        m(::six::Init::undefined<double>())
    {
    }
    
    SFAPoint(double _x, double _y) :
        SFAGeometry(TYPE_NAME), 
        x(_x), 
        y(_y),
        z(::six::Init::undefined<double>()),
        m(::six::Init::undefined<double>())
    {
    }
    
    SFAPoint(double _x, double _y, double _z, double _m) :
        SFAGeometry(TYPE_NAME), 
        x(_x), 
        y(_y), 
        z(_z), 
        m(_m)
    {
    }

    virtual SFAPoint* clone() const
    {
        return new SFAPoint(*this);
    }

    static const char TYPE_NAME[];
};

//! Abstract type
struct SFACurve : public SFAGeometry
{
protected:
    SFACurve(const std::string& typeName) :
        SFAGeometry(typeName)
    {
    }
};

struct SFALineString : public SFACurve
{
public:
    SFALineString() :
        SFACurve(TYPE_NAME)
    {
    }
    virtual SFALineString* clone() const
    {
        return new SFALineString(*this);
    }
    std::vector<mem::ScopedCopyablePtr<SFAPoint> > vertices;
    static const char TYPE_NAME[];
};

struct SFALine : public SFALineString
{
public:
    SFALine() :
        SFALineString()
    {
        mType = TYPE_NAME;
    }
    virtual SFALine* clone() const
    {
        return new SFALine(*this);
    }
    static const char TYPE_NAME[];
};

struct SFALinearRing : public SFALineString
{
public:
    SFALinearRing() :
        SFALineString()
    {
        mType = TYPE_NAME;
    }
    virtual SFALinearRing* clone() const
    {
        return new SFALinearRing(*this);
    }
    static const char TYPE_NAME[];
};

//! Abstract type
struct SFASurface : public SFAGeometry
{
protected:
    SFASurface(const std::string& typeName) :
        SFAGeometry(typeName)
    {
    }
};

struct SFAPolygon : public SFASurface
{
public:
    SFAPolygon() :
        SFASurface(TYPE_NAME)
    {
    }
    virtual SFAPolygon* clone() const
    {
        return new SFAPolygon(*this);
    }

    std::vector<mem::ScopedCopyablePtr<SFALinearRing> > rings;
    static const char TYPE_NAME[];
};

struct SFATriangle : public SFAPolygon
{
public:
    SFATriangle() :
        SFAPolygon()
    {
        mType = TYPE_NAME;
    }

    virtual SFATriangle* clone() const
    {
        return new SFATriangle(*this);
    }

    static const char TYPE_NAME[];
};

struct SFAPolyhedralSurface : public SFASurface
{
public:
    SFAPolyhedralSurface() :
        SFASurface(TYPE_NAME)
    {
    }

    virtual SFAPolyhedralSurface* clone() const
    {
        return new SFAPolyhedralSurface(*this);
    }

    std::vector<mem::ScopedCloneablePtr<SFAPolygon> > patches;
    static const char TYPE_NAME[];
};

// note that we are deriving Surface rather than PolyhedralSurface
// this is to avoid the patches name clash
struct SFATriangulatedIrregularNetwork : public SFASurface
{
public:
    SFATriangulatedIrregularNetwork() :
        SFASurface(TYPE_NAME)
    {
    }

    virtual SFATriangulatedIrregularNetwork* clone() const
    {
        return new SFATriangulatedIrregularNetwork(*this);
    }

    std::vector<mem::ScopedCloneablePtr<SFAPolygon> > patches;
    static const char TYPE_NAME[];
};

//! Abstract type
struct SFAGeometryCollection : public SFAGeometry
{
protected:
    SFAGeometryCollection(const std::string& typeName) :
        SFAGeometry(typeName)
    {
    }
};

struct SFAMultiPoint : public SFAGeometryCollection
{
public:
    SFAMultiPoint() :
        SFAGeometryCollection(TYPE_NAME)
    {
    }
    virtual SFAMultiPoint* clone() const
    {
        return new SFAMultiPoint(*this);
    }
    std::vector<mem::ScopedCopyablePtr<SFAPoint> > vertices;
    static const char TYPE_NAME[];
};

//! Abstract type
struct SFAMultiCurve : public SFAGeometryCollection
{
protected:
    SFAMultiCurve(const std::string& typeName) :
        SFAGeometryCollection(typeName)
    {
    }
};

struct SFAMultiLineString : public SFAMultiCurve
{
public:
    SFAMultiLineString() :
        SFAMultiCurve(TYPE_NAME)
    {
    }

    virtual SFAMultiLineString* clone() const
    {
        return new SFAMultiLineString(*this);
    }

    std::vector<mem::ScopedCloneablePtr<SFALineString> > elements;
    static const char TYPE_NAME[];
};

//! Abstract type
struct SFAMultiSurface : public SFAGeometryCollection
{
protected:
    SFAMultiSurface(const std::string& typeName) :
        SFAGeometryCollection(typeName)
    {
    }
};

struct SFAMultiPolygon : public SFAMultiSurface
{
public:
    SFAMultiPolygon() :
        SFAMultiSurface(TYPE_NAME)
    {
    }

    virtual SFAMultiPolygon* clone() const
    {
        return new SFAMultiPolygon(*this);
    }

    std::vector<mem::ScopedCloneablePtr<SFAPolygon> > elements;
    static const char TYPE_NAME[];
};

struct SFACoordinateSystem : public SFATyped
{
public:
    virtual SFACoordinateSystem* clone() const
    {
        return new SFACoordinateSystem(*this);
    }

protected:
    SFACoordinateSystem(const std::string& typeName) :
        SFATyped(typeName)
    {
    }
};

struct SFAPrimeMeridian
{
    std::string name;
    double longitude;
};

struct SFASpheroid
{
    std::string name;
    double semiMajorAxis;
    double inverseFlattening;
};

struct SFAParameter
{
    std::string name;
    double value;
};

struct SFAProjection
{
    std::string name;
};

struct SFADatum
{
    SFASpheroid spheroid;
};

struct SFAGeocentricCoordinateSystem : public SFACoordinateSystem
{
    std::string csName;
    SFADatum datum;
    SFAPrimeMeridian primeMeridian;
    std::string linearUnit;

    SFAGeocentricCoordinateSystem() :
        SFACoordinateSystem(TYPE_NAME)
    {
    }

    virtual SFAGeocentricCoordinateSystem* clone() const
    {
        return new SFAGeocentricCoordinateSystem(*this); 
    }

    static const char TYPE_NAME[];
};

struct SFAGeographicCoordinateSystem : public SFACoordinateSystem
{
    std::string csName;
    SFADatum datum;
    SFAPrimeMeridian primeMeridian;
    std::string angularUnit;
    std::string linearUnit;

    SFAGeographicCoordinateSystem() :
        SFACoordinateSystem(TYPE_NAME)
    {
    }

    virtual SFAGeographicCoordinateSystem* clone() const
    {
        return new SFAGeographicCoordinateSystem(*this);
    }

    static const char TYPE_NAME[];
};

struct SFAProjectedCoordinateSystem : public SFACoordinateSystem
{
    std::string csName;
    mem::ScopedCopyablePtr<SFAGeographicCoordinateSystem> geographicCoordinateSystem;
    SFAProjection projection;
    SFAParameter parameter;
    std::string linearUnit;

    SFAProjectedCoordinateSystem() :
        SFACoordinateSystem(TYPE_NAME)
    {
    }

    virtual SFAProjectedCoordinateSystem* clone() const
    {
        return new SFAProjectedCoordinateSystem(*this);
    }

    static const char TYPE_NAME[];
};

struct SFAReferenceSystem
{
    mem::ScopedCloneablePtr<SFACoordinateSystem> coordinateSystem;
    std::vector<std::string> axisNames;
};

}
}
#endif
