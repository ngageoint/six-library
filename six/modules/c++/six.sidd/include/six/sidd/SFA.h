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

    friend bool operator==(const SFATyped& lhs, const SFATyped& rhs)
    {
        return lhs.equalTo(rhs);
    }

    friend bool operator!=(const SFATyped& lhs, const SFATyped& rhs)
    {
        return !(lhs == rhs);
    }

protected:
    std::string mType;
    SFATyped(const std::string& typeName) :
        mType(typeName)
    {
    }

    virtual bool equalTo(const SFATyped& rhs) const = 0;
};

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

    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFAPoint const* point = dynamic_cast<SFAPoint const*>(&rhs);
        if (point != NULL)
        {
            return (x == point->x && y == point->y &&
                z == point->z && m == point->m);
        }
        return false;
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

    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFALineString const* lineString = dynamic_cast<SFALineString const*>(&rhs);
        if (lineString != NULL)
        {
            return (vertices == lineString->vertices);
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
    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFALine const* line = dynamic_cast<SFALine const*>(&rhs);
        if (line != NULL)
        {
            return (vertices == line->vertices);
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
    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFALinearRing const* linearRing = dynamic_cast<SFALinearRing const*>(&rhs);
        if (linearRing != NULL)
        {
            return (vertices == linearRing->vertices);
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

    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFAPolygon const* polygon = dynamic_cast<SFAPolygon const*>(&rhs);
        if (polygon != NULL)
        {
            return (rings == polygon->rings);
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

    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFATriangle const* triangle = dynamic_cast<SFATriangle const*>(&rhs);
        if (triangle != NULL)
        {
            return (rings == triangle->rings);
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

    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFAPolyhedralSurface const* surface = dynamic_cast<SFAPolyhedralSurface const*>(&rhs);
        if (surface != NULL)
        {
            return (patches == surface->patches);
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

    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFATriangulatedIrregularNetwork const* network = dynamic_cast<SFATriangulatedIrregularNetwork const*>(&rhs);
        if (network != NULL)
        {
            return (patches == network->patches);
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

    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFAMultiPoint const* multiPoint = dynamic_cast<SFAMultiPoint const*>(&rhs);
        if (multiPoint != NULL)
        {
            return (vertices == multiPoint->vertices);
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

    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFAMultiLineString const* string = dynamic_cast<SFAMultiLineString const*>(&rhs);
        if (string != NULL)
        {
            return (elements == string->elements);
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

    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFAMultiPolygon const* polygon = dynamic_cast<SFAMultiPolygon const*>(&rhs);
        if (polygon != NULL)
        {
            return (elements == polygon->elements);
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

struct SFAPrimeMeridian
{
    std::string name;
    double longitude;

    bool operator==(const SFAPrimeMeridian& rhs) const
    {
        return name == rhs.name && longitude == rhs.longitude;
    }
    bool operator!=(const SFAPrimeMeridian& rhs) const
    {
        return !(*this == rhs);
    }
};

struct SFASpheroid
{
    std::string name;
    double semiMajorAxis;
    double inverseFlattening;

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

struct SFAParameter
{
    std::string name;
    double value;

    bool operator==(const SFAParameter& rhs) const
    {
        return name == rhs.name && value == rhs.value;
    }

    bool operator!=(const SFAParameter& rhs) const
    {
        return !(*this == rhs);
    }
};

struct SFAProjection
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

struct SFADatum
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

    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFAGeocentricCoordinateSystem const* system = dynamic_cast<SFAGeocentricCoordinateSystem const*>(&rhs);
        if (system != NULL)
        {
            return (csName == system->csName && datum == system->datum &&
                primeMeridian == system->primeMeridian && linearUnit == system->linearUnit);
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

    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFAGeographicCoordinateSystem const* system = dynamic_cast<SFAGeographicCoordinateSystem const*>(&rhs);
        if (system != NULL)
        {
            return (csName == system->csName && datum == system->datum &&
                primeMeridian == system->primeMeridian && linearUnit == system->linearUnit &&
                angularUnit == system->angularUnit);
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

    virtual bool equalTo(const SFATyped& rhs) const
    {
        SFAProjectedCoordinateSystem const* system = dynamic_cast<SFAProjectedCoordinateSystem const*>(&rhs);
        if (system != NULL)
        {
            return (csName == system->csName && projection == system->projection &&
                parameter == system->parameter && linearUnit == system->linearUnit &&
                geographicCoordinateSystem == system->geographicCoordinateSystem);
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
};

}
}
#endif
