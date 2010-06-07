/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
 *
 * six-c++ is free software; you can redistribute it and/or modify
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
    virtual ~SFATyped()
    {
    }
    inline std::string getType() const
    {
        return mType;
    }

protected:
    std::string mType;
    SFATyped(std::string typeName) :
        mType(typeName)
    {
    }
};

struct SFAGeometry : public SFATyped
{
    virtual ~SFAGeometry()
    {
    }
protected:
    SFAGeometry(std::string typeName) :
        SFATyped(typeName)
    {
    }
};

struct SFAPoint : public SFAGeometry
{
    double x, y, z, m;

    SFAPoint() :
        SFAGeometry(TYPE_NAME)
    {
        x = 0;
        y = 0;
        z = ::six::Init::undefined<double>();
        m = ::six::Init::undefined<double>();
    }
    
    SFAPoint(double _x, double _y) :
        SFAGeometry(TYPE_NAME), x(_x), y(_y)
    {
        z = ::six::Init::undefined<double>();
        m = ::six::Init::undefined<double>();
    }
    
    SFAPoint(double _x, double _y, double _z, double _m) :
        SFAGeometry(TYPE_NAME), x(_x), y(_y), z(_z), m(_m)
    {
    }
    virtual ~SFAPoint()
    {
    }
    static const char TYPE_NAME[];
};

//! Abstract type
struct SFACurve : public SFAGeometry
{
    virtual ~SFACurve()
    {
    }
protected:
    SFACurve(std::string typeName) :
        SFAGeometry(typeName)
    {
    }
};

struct SFALineString : public SFACurve
{
    std::vector<SFAPoint*> vertices;

    SFALineString() :
        SFACurve(TYPE_NAME)
    {
    }
    virtual ~SFALineString()
    {
        for (size_t i = 0, n = vertices.size(); i < n; ++i)
            if (vertices[i])
                delete vertices[i];
    }
    static const char TYPE_NAME[];
};

struct SFALine : public SFALineString
{
    SFALine() :
        SFALineString()
    {
        mType = TYPE_NAME;
    }
    virtual ~SFALine()
    {
    }
    static const char TYPE_NAME[];
};

struct SFALinearRing : public SFALineString
{
    SFALinearRing() :
        SFALineString()
    {
        mType = TYPE_NAME;
    }
    virtual ~SFALinearRing()
    {
    }
    static const char TYPE_NAME[];
};

//! Abstract type
struct SFASurface : public SFAGeometry
{
    virtual ~SFASurface()
    {
    }
protected:
    SFASurface(std::string typeName) :
        SFAGeometry(typeName)
    {
    }
};

struct SFAPolygon : public SFASurface
{
    std::vector<SFALinearRing*> rings;

    SFAPolygon() :
        SFASurface(TYPE_NAME)
    {
    }
    virtual ~SFAPolygon()
    {
        for (size_t i = 0, n = rings.size(); i < n; ++i)
            if (rings[i])
                delete rings[i];
    }
    static const char TYPE_NAME[];
};

struct SFATriangle : public SFAPolygon
{
    SFATriangle() :
        SFAPolygon()
    {
        mType = TYPE_NAME;
    }
    ~SFATriangle()
    {
    }
    static const char TYPE_NAME[];
};

struct SFAPolyhedralSurface : public SFASurface
{
    std::vector<SFAPolygon*> patches;

    SFAPolyhedralSurface() :
        SFASurface(TYPE_NAME)
    {
    }
    virtual ~SFAPolyhedralSurface()
    {
        for (size_t i = 0, n = patches.size(); i < n; ++i)
            if (patches[i])
                delete patches[i];
    }
    static const char TYPE_NAME[];
};

// note that we are deriving Surface rather than PolyhedralSurface
// this is to avoid the patches name clash
struct SFATriangulatedIrregularNetwork : public SFASurface
{
    std::vector<SFAPolygon*> patches;

    SFATriangulatedIrregularNetwork() :
        SFASurface(TYPE_NAME)
    {
    }
    virtual ~SFATriangulatedIrregularNetwork()
    {
        for (size_t i = 0, n = patches.size(); i < n; ++i)
            if (patches[i])
                delete patches[i];
    }
    static const char TYPE_NAME[];
};

//! Abstract type
struct SFAGeometryCollection : public SFAGeometry
{
    virtual ~SFAGeometryCollection()
    {
    }
protected:
    SFAGeometryCollection(std::string typeName) :
        SFAGeometry(typeName)
    {
    }
};

struct SFAMultiPoint : public SFAGeometryCollection
{
    std::vector<SFAPoint*> vertices;

    SFAMultiPoint() :
        SFAGeometryCollection(TYPE_NAME)
    {
    }
    virtual ~SFAMultiPoint()
    {
        for (size_t i = 0, n = vertices.size(); i < n; ++i)
            if (vertices[i])
                delete vertices[i];
    }
    static const char TYPE_NAME[];
};

//! Abstract type
struct SFAMultiCurve : public SFAGeometryCollection
{
    virtual ~SFAMultiCurve()
    {
    }
protected:
    SFAMultiCurve(std::string typeName) :
        SFAGeometryCollection(typeName)
    {
    }
};

struct SFAMultiLineString : public SFAMultiCurve
{
    std::vector<SFALineString*> elements;

    SFAMultiLineString() :
        SFAMultiCurve(TYPE_NAME)
    {
    }
    virtual ~SFAMultiLineString()
    {
        for (size_t i = 0, n = elements.size(); i < n; ++i)
            if (elements[i])
                delete elements[i];
    }
    static const char TYPE_NAME[];
};

//! Abstract type
struct SFAMultiSurface : public SFAGeometryCollection
{
    virtual ~SFAMultiSurface()
    {
    }
protected:
    SFAMultiSurface(std::string typeName) :
        SFAGeometryCollection(typeName)
    {
    }
};

struct SFAMultiPolygon : public SFAMultiSurface
{
    std::vector<SFAPolygon*> elements;

    SFAMultiPolygon() :
        SFAMultiSurface(TYPE_NAME)
    {
    }
    virtual ~SFAMultiPolygon()
    {
        for (size_t i = 0, n = elements.size(); i < n; ++i)
            if (elements[i])
                delete elements[i];
    }
    static const char TYPE_NAME[];
};

struct SFACoordinateSystem : public SFATyped
{
    virtual ~SFACoordinateSystem()
    {
    }
protected:
    SFACoordinateSystem(std::string typeName) :
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
    std::string name;
    SFADatum datum;
    SFAPrimeMeridian primeMeridian;
    std::string linearUnit;

    SFAGeocentricCoordinateSystem() :
        SFACoordinateSystem(TYPE_NAME)
    {
    }
    ~SFAGeocentricCoordinateSystem()
    {
    }
    static const char TYPE_NAME[];
};

struct SFAGeographicCoordinateSystem : public SFACoordinateSystem
{
    std::string name;
    SFADatum datum;
    SFAPrimeMeridian primeMeridian;
    std::string angularUnit;
    std::string linearUnit;

    SFAGeographicCoordinateSystem() :
        SFACoordinateSystem(TYPE_NAME)
    {
    }
    ~SFAGeographicCoordinateSystem()
    {
    }
    static const char TYPE_NAME[];
};

struct SFAProjectedCoordinateSystem : public SFACoordinateSystem
{
    std::string name;
    SFAGeographicCoordinateSystem *geographicCoordinateSystem;
    SFAProjection projection;
    SFAParameter parameter;
    std::string linearUnit;

    SFAProjectedCoordinateSystem() :
        SFACoordinateSystem(TYPE_NAME), geographicCoordinateSystem(NULL)
    {
    }
    ~SFAProjectedCoordinateSystem()
    {
        if (geographicCoordinateSystem)
            delete geographicCoordinateSystem;
    }
    static const char TYPE_NAME[];
};

struct SFAReferenceSystem
{
    SFACoordinateSystem *coordinateSystem;
    std::vector<std::string> axisNames;

    SFAReferenceSystem() :
        coordinateSystem(NULL)
    {
    }
    virtual ~SFAReferenceSystem()
    {
        if (coordinateSystem)
            delete coordinateSystem;
    }
};

}
}
#endif
