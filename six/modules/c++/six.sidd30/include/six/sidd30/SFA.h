/* =========================================================================
* This file is part of six.sidd30-c++
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

#include <std/optional>

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

    virtual bool equalTo(const SFATyped& rhs) const = 0;

protected:
    std::string mType;
    SFATyped(const std::string& typeName) :
        mType(typeName)
    {
    }
};
inline bool operator==(const SFATyped& lhs, const SFATyped& rhs)
{
    return lhs.equalTo(rhs) && rhs.equalTo(lhs);
}
inline bool operator!=(const SFATyped& lhs, const SFATyped& rhs)
{
    return !(lhs == rhs);
}

//! Abstract type
struct SFAGeometry : public SFATyped
{
    virtual SFAGeometry* clone() const = 0;

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

    bool operator==(const SFAPoint& rhs) const // need member-function for SWIG
    {
        return static_cast<const SFATyped&>(*this) == static_cast<const SFATyped&>(rhs);
    }
private:
    bool operator_eq(const SFAPoint& rhs) const
    {
        return (x == rhs.x && y == rhs.y &&
            z == rhs.z && m == rhs.m);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFAPoint* point = dynamic_cast<const SFAPoint*>(&rhs);
        if (point != nullptr)
        {
            return this->operator_eq(*point);
        }
        return false;
    }


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

    bool operator==(const SFALineString& rhs) const // need member-function for SWIG
    {
        return static_cast<const SFATyped&>(*this) == static_cast<const SFATyped&>(rhs);
    }
private:
    bool operator_eq(const SFALineString& rhs) const
    {
        return (vertices == rhs.vertices);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFALineString* lineString = dynamic_cast<const SFALineString*>(&rhs);
        if (lineString != nullptr)
        {
            return this->operator_eq(*lineString);
        }
        return false;
    }
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

    bool operator==(const SFALine& rhs) const // need member-function for SWIG
    {
        return static_cast<const SFATyped&>(*this) == static_cast<const SFATyped&>(rhs);
    }
private:
    bool operator_eq(const SFALine& rhs) const
    {
        return (vertices == rhs.vertices);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFALine* line = dynamic_cast<const SFALine*>(&rhs);
        if (line != nullptr)
        {
            return this->operator_eq(*line);
        }
        return false;
    }
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

    bool operator==(const SFALinearRing& rhs) const // need member-function for SWIG
    {
        return static_cast<const SFATyped&>(*this) == static_cast<const SFATyped&>(rhs);
    }
private:
    bool operator_eq(const SFALinearRing& rhs) const
    {
        return (vertices == rhs.vertices);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFALinearRing* linearRing = dynamic_cast<const SFALinearRing*>(&rhs);
        if (linearRing != nullptr)
        {
            return this->operator_eq(*linearRing);
        }
        return false;
    }
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

private:
    bool operator_eq(const SFAPolygon& rhs) const
    {
        return (rings == rhs.rings);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFAPolygon* polygon = dynamic_cast<const SFAPolygon*>(&rhs);
        if (polygon != nullptr)
        {
            return this->operator_eq(*polygon);
        }
        return false;
    }
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

    bool operator==(const SFATriangle& rhs) const // need member-function for SWIG
    {
        return static_cast<const SFATyped&>(*this) == static_cast<const SFATyped&>(rhs);
    }
private:
    bool operator_eq(const SFATriangle& rhs) const
    {
        return (rings == rhs.rings);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFATriangle* triangle = dynamic_cast<const SFATriangle*>(&rhs);
        if (triangle != nullptr)
        {
            return this->operator_eq(*triangle);
        }
        return false;
    }
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

    bool operator==(const SFAPolyhedralSurface& rhs) const // need member-function for SWIG
    {
        return static_cast<const SFATyped&>(*this) == static_cast<const SFATyped&>(rhs);
    }
private:
    bool operator_eq(const SFAPolyhedralSurface& rhs) const
    {
        return (patches == rhs.patches);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFAPolyhedralSurface* surface = dynamic_cast<const SFAPolyhedralSurface*>(&rhs);
        if (surface != nullptr)
        {
            return this->operator_eq(*surface);
        }
        return false;
    }
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

    bool operator==(const SFATriangulatedIrregularNetwork& rhs) const // need member-function for SWIG
    {
        return static_cast<const SFATyped&>(*this) == static_cast<const SFATyped&>(rhs);
    }
private:
    bool operator_eq(const SFATriangulatedIrregularNetwork& rhs) const
    {
        return (patches == rhs.patches);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFATriangulatedIrregularNetwork* network = dynamic_cast<const SFATriangulatedIrregularNetwork*>(&rhs);
        if (network != nullptr)
        {
            return this->operator_eq(*network);
        }
        return false;
    }
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

    bool operator==(const SFAMultiPoint& rhs) const // need member-function for SWIG
    {
        return static_cast<const SFATyped&>(*this) == static_cast<const SFATyped&>(rhs);
    }
private:
    bool operator_eq(const SFAMultiPoint& rhs) const
    {
        return (vertices == rhs.vertices);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFAMultiPoint* multiPoint = dynamic_cast<const SFAMultiPoint*>(&rhs);
        if (multiPoint != nullptr)
        {
            return this->operator_eq(*multiPoint);
        }
        return false;
    }
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

    bool operator==(const SFAMultiLineString& rhs) const // need member-function for SWIG
    {
        return static_cast<const SFATyped&>(*this) == static_cast<const SFATyped&>(rhs);
    }
private:
    bool operator_eq(const SFAMultiLineString& rhs) const
    {
        return (elements == rhs.elements);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFAMultiLineString* string = dynamic_cast<const SFAMultiLineString*>(&rhs);
        if (string != nullptr)
        {
            return this->operator_eq(*string);
        }
        return false;
    }
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

    bool operator==(const SFAMultiPolygon& rhs) const // need member-function for SWIG
    {
        return static_cast<const SFATyped&>(*this) == static_cast<const SFATyped&>(rhs);
    }
private:
    bool operator_eq(const SFAMultiPolygon& rhs) const
    {
        return (elements == rhs.elements);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFAMultiPolygon* polygon = dynamic_cast<const SFAMultiPolygon*>(&rhs);
        if (polygon != nullptr)
        {
            return this->operator_eq(*polygon);
        }
        return false;
    }
};

//! Abstract type
struct SFACoordinateSystem : public SFATyped
{
public:
    virtual SFACoordinateSystem* clone() const = 0;

protected:
    SFACoordinateSystem(const std::string& typeName) :
        SFATyped(typeName)
    {
    }
};

struct SFAPrimeMeridian final
{
    std::string name;
    double longitude = 0.0;

    bool operator==(const SFAPrimeMeridian& rhs) const
    {
        return name == rhs.name && longitude == rhs.longitude;
    }
    bool operator!=(const SFAPrimeMeridian& rhs) const
    {
        return !(*this == rhs);
    }
};

struct SFASpheroid final
{
    std::string name;
    double semiMajorAxis = 0.0;
    double inverseFlattening = 0.0;

    bool operator==(const SFASpheroid& rhs) const
    {
        return (name == rhs.name && semiMajorAxis == rhs.semiMajorAxis &&
            inverseFlattening == rhs.inverseFlattening);
    }

    bool operator!=(const SFASpheroid& rhs) const
    {
        return !(*this == rhs);
    }
};

struct SFAParameter final
{
    std::string name;
    double value = 0.0;

    bool operator==(const SFAParameter& rhs) const
    {
        return name == rhs.name && value == rhs.value;
    }

    bool operator!=(const SFAParameter& rhs) const
    {
        return !(*this == rhs);
    }
};

struct SFAProjection final
{
    std::string name;

    bool operator==(const SFAProjection& rhs) const
    {
        return name == rhs.name;
    }

    bool operator!=(const SFAProjection& rhs) const
    {
        return !(*this == rhs);
    }
};

struct SFADatum final
{
    SFASpheroid spheroid;

    bool operator==(const SFADatum& rhs) const
    {
        return spheroid == rhs.spheroid;
    }

    bool operator!=(const SFADatum& rhs) const
    {
        return !(*this == rhs);
    }
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

    bool operator==(const SFAGeocentricCoordinateSystem& rhs) const // need member-function for SWIG
    {
        return static_cast<const SFATyped&>(*this) == static_cast<const SFATyped&>(rhs);
    }
private:
    bool operator_eq(const SFAGeocentricCoordinateSystem& rhs) const
    {
        return (csName == rhs.csName && datum == rhs.datum &&
            primeMeridian == rhs.primeMeridian && linearUnit == rhs.linearUnit);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFAGeocentricCoordinateSystem* system = dynamic_cast<const SFAGeocentricCoordinateSystem*>(&rhs);
        if (system != nullptr)
        {
            return this->operator_eq(*system);
        }
        return false;
    }
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

    bool operator==(const SFAGeographicCoordinateSystem& rhs) const // need member-function for SWIG
    {
        return static_cast<const SFATyped&>(*this) == static_cast<const SFATyped&>(rhs);
    }
private:
    bool operator_eq(const SFAGeographicCoordinateSystem& rhs) const
    {
        return (csName == rhs.csName && datum == rhs.datum &&
            primeMeridian == rhs.primeMeridian && linearUnit == rhs.linearUnit &&
            angularUnit == rhs.angularUnit);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFAGeographicCoordinateSystem* system = dynamic_cast<const SFAGeographicCoordinateSystem*>(&rhs);
        if (system != nullptr)
        {
            return this->operator_eq(*system);
        }
        return false;
    }
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

private:
    bool operator_eq(const SFAProjectedCoordinateSystem& rhs) const
    {
        return (csName == rhs.csName && projection == rhs.projection &&
            parameter == rhs.parameter && linearUnit == rhs.linearUnit &&
            geographicCoordinateSystem == rhs.geographicCoordinateSystem);
    }
    virtual bool equalTo(const SFATyped& rhs) const override
    {
        const SFAProjectedCoordinateSystem* system = dynamic_cast<const SFAProjectedCoordinateSystem*>(&rhs);
        if (system != nullptr)
        {
            return this->operator_eq(*system);
        }
        return false;
    }
};

struct SFAReferenceSystem
{
    mem::ScopedCloneablePtr<SFACoordinateSystem> coordinateSystem;
    std::vector<std::string> axisNames;

    virtual bool operator==(const SFAReferenceSystem& rhs) const
    {
        return (coordinateSystem == rhs.coordinateSystem &&
            axisNames == rhs.axisNames);
    }

    virtual bool operator!=(const SFAReferenceSystem& rhs) const
    {
        return !(*this == rhs);
    }

    virtual ~SFAReferenceSystem() = default;
};

}
}
#endif
