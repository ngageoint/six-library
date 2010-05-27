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

// TODO if we change the definition of the typedefs in the schema, then we need
// to remove them as typedefs and add real structs that derive their bases

namespace six
{
namespace sidd
{
namespace sfa
{

struct Geometry
{
    virtual ~Geometry()
    {
    }

    inline std::string get() const
    {
        return m;
    }

protected:
    std::string m;

    Geometry(std::string typeName) :
        m(typeName)
    {
    }
};

struct Point : public Geometry
{
    double x, y, z, m;

    Point(double _x = 0, double _y = 0, double _z = ::six::Init::undefined<
            double>(), double _m = ::six::Init::undefined<double>()) :
        Geometry(TYPE_NAME), x(_x), y(_y), z(_z), m(_m)
    {
    }
    virtual ~Point()
    {
    }
private:
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
    }
private:
    static const char TYPE_NAME[];
};

typedef LineString Line;
typedef LineString LinearRing;

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
    }
private:
    static const char TYPE_NAME[];
};

struct Triangle : public Polygon
{
    Triangle() :
        Polygon()
    {
        m = TYPE_NAME;
    }
    ~Triangle()
    {
    }
private:
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
    }
private:
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
    }
private:
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
    }
private:
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
    }
private:
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
    }
private:
    static const char TYPE_NAME[];
};

}
}
}
#endif
