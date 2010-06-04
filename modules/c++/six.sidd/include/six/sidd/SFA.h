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
namespace sfa
{

struct Typed
{
    virtual ~Typed()
    {
    }
    inline std::string getType() const
    {
        return mType;
    }

protected:
    std::string mType;
    Typed(std::string typeName) :
        mType(typeName)
    {
    }
};

struct Geometry : public Typed
{
    virtual ~Geometry()
    {
    }
protected:
    Geometry(std::string typeName) :
        Typed(typeName)
    {
    }
};

struct Point : public Geometry
{
    double x, y, z, m;

    Point() :
        Geometry(TYPE_NAME)
    {
        x = 0;
        y = 0;
        z = ::six::Init::undefined<double>();
        m = ::six::Init::undefined<double>();
    }
    
    Point(double _x, double _y) :
        Geometry(TYPE_NAME), x(_x), y(_y)
    {
        z = ::six::Init::undefined<double>();
        m = ::six::Init::undefined<double>();
    }
    
    Point(double _x, double _y, double _z, double _m) :
        Geometry(TYPE_NAME), x(_x), y(_y), z(_z), m(_m)
    {
    }
    virtual ~Point()
    {
    }
    static const char TYPE_NAME[];
};

//! Abstract type
struct Curve : public Geometry
{
    virtual ~Curve()
    {
    }
protected:
    Curve(std::string typeName) :
        Geometry(typeName)
    {
    }
};

struct LineString : public Curve
{
    std::vector<Point*> vertices;

    LineString() :
        Curve(TYPE_NAME)
    {
    }
    virtual ~LineString()
    {
        for (size_t i = 0, n = vertices.size(); i < n; ++i)
            if (vertices[i])
                delete vertices[i];
    }
    static const char TYPE_NAME[];
};

struct Line : public LineString
{
    Line() : LineString()
    {
        mType = TYPE_NAME;
    }
    virtual ~Line()
    {
    }
    static const char TYPE_NAME[];
};

struct LinearRing : public LineString
{
    LinearRing() : LineString()
    {
        mType = TYPE_NAME;
    }
    virtual ~LinearRing()
    {
    }
    static const char TYPE_NAME[];
};


//! Abstract type
struct Surface : public Geometry
{
    virtual ~Surface()
    {
    }
protected:
    Surface(std::string typeName) :
        Geometry(typeName)
    {
    }
};

struct Polygon : public Surface
{
    std::vector<LinearRing*> rings;

    Polygon() :
        Surface(TYPE_NAME)
    {
    }
    virtual ~Polygon()
    {
        for (size_t i = 0, n = rings.size(); i < n; ++i)
            if (rings[i])
                delete rings[i];
    }
    static const char TYPE_NAME[];
};

struct Triangle : public Polygon
{
    Triangle() :
        Polygon()
    {
        mType = TYPE_NAME;
    }
    ~Triangle()
    {
    }
    static const char TYPE_NAME[];
};

struct PolyhedralSurface : public Surface
{
    std::vector<Polygon*> patches;

    PolyhedralSurface() :
        Surface(TYPE_NAME)
    {
    }
    virtual ~PolyhedralSurface()
    {
        for (size_t i = 0, n = patches.size(); i < n; ++i)
            if (patches[i])
                delete patches[i];
    }
    static const char TYPE_NAME[];
};

// note that we are deriving Surface rather than PolyhedralSurface
// this is to avoid the patches name clash
struct TriangulatedIrregularNetwork : public Surface
{
    std::vector<Polygon*> patches;

    TriangulatedIrregularNetwork() :
        Surface(TYPE_NAME)
    {
    }
    virtual ~TriangulatedIrregularNetwork()
    {
        for (size_t i = 0, n = patches.size(); i < n; ++i)
            if (patches[i])
                delete patches[i];
    }
    static const char TYPE_NAME[];
};

//! Abstract type
struct GeometryCollection : public Geometry
{
    virtual ~GeometryCollection()
    {
    }
protected:
    GeometryCollection(std::string typeName) :
        Geometry(typeName)
    {
    }
};

struct MultiPoint : public GeometryCollection
{
    std::vector<Point*> vertices;

    MultiPoint() :
        GeometryCollection(TYPE_NAME)
    {
    }
    virtual ~MultiPoint()
    {
        for (size_t i = 0, n = vertices.size(); i < n; ++i)
            if (vertices[i])
                delete vertices[i];
    }
    static const char TYPE_NAME[];
};

//! Abstract type
struct MultiCurve : public GeometryCollection
{
    virtual ~MultiCurve()
    {
    }
protected:
    MultiCurve(std::string typeName) :
        GeometryCollection(typeName)
    {
    }
};

struct MultiLineString : public MultiCurve
{
    std::vector<LineString*> elements;

    MultiLineString() :
        MultiCurve(TYPE_NAME)
    {
    }
    virtual ~MultiLineString()
    {
        for (size_t i = 0, n = elements.size(); i < n; ++i)
            if (elements[i])
                delete elements[i];
    }
    static const char TYPE_NAME[];
};

//! Abstract type
struct MultiSurface : public GeometryCollection
{
    virtual ~MultiSurface()
    {
    }
protected:
    MultiSurface(std::string typeName) :
        GeometryCollection(typeName)
    {
    }
};

struct MultiPolygon : public MultiSurface
{
    std::vector<Polygon*> elements;

    MultiPolygon() :
        MultiSurface(TYPE_NAME)
    {
    }
    virtual ~MultiPolygon()
    {
        for (size_t i = 0, n = elements.size(); i < n; ++i)
            if (elements[i])
                delete elements[i];
    }
    static const char TYPE_NAME[];
};

struct CoordinateSystem : public Typed
{
    virtual ~CoordinateSystem()
    {
    }
protected:
    CoordinateSystem(std::string typeName) :
        Typed(typeName)
    {
    }
};

struct PrimeMeridian
{
    std::string name;
    double longitude;
};

struct Spheroid
{
    std::string name;
    double semiMajorAxis;
    double inverseFlattening;
};

struct Parameter
{
    std::string name;
    double value;
};

struct Projection
{
    std::string name;
};

struct Datum
{
    Spheroid spheroid;
};

struct GeocentricCoordinateSystem : public CoordinateSystem
{
    std::string name;
    Datum datum;
    PrimeMeridian primeMeridian;
    std::string linearUnit;

    GeocentricCoordinateSystem() :
        CoordinateSystem(TYPE_NAME)
    {
    }
    ~GeocentricCoordinateSystem()
    {
    }
    static const char TYPE_NAME[];
};

struct GeographicCoordinateSystem : public CoordinateSystem
{
    std::string name;
    Datum datum;
    PrimeMeridian primeMeridian;
    std::string angularUnit;
    std::string linearUnit;

    GeographicCoordinateSystem() :
        CoordinateSystem(TYPE_NAME)
    {
    }
    ~GeographicCoordinateSystem()
    {
    }
    static const char TYPE_NAME[];
};

struct ProjectedCoordinateSystem : public CoordinateSystem
{
    std::string name;
    GeographicCoordinateSystem *geographicCoordinateSystem;
    Projection projection;
    Parameter parameter;
    std::string linearUnit;

    ProjectedCoordinateSystem() :
        CoordinateSystem(TYPE_NAME), geographicCoordinateSystem(NULL)
    {
    }
    ~ProjectedCoordinateSystem()
    {
        if (geographicCoordinateSystem)
            delete geographicCoordinateSystem;
    }
    static const char TYPE_NAME[];
};

struct ReferenceSystem
{
    CoordinateSystem *coordinateSystem;
    std::vector<std::string> axisNames;

    ReferenceSystem() :
        coordinateSystem(NULL)
    {
    }
    virtual ~ReferenceSystem()
    {
        if (coordinateSystem)
            delete coordinateSystem;
    }
};

}
}
}
#endif
