/* =========================================================================
 * This file is part of cphd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2019, MDA Information Systems LLC
 *
 * cphd-c++ is free software; you can redistribute it and/or modify
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


#ifndef __CPHD_SCENE_COORDINATES_H__
#define __CPHD_SCENE_COORDINATES_H__

#include <ostream>
#include <vector>

#include <mem/ScopedCopyablePtr.h>
#include <cphd/Enums.h>
#include <cphd/Types.h>

namespace cphd
{
struct IARP
{
    IARP();

    bool operator==(const IARP& other) const
    {
        return ecf == other.ecf && llh == other.llh;
    }

    bool operator!=(const IARP& other) const
    {
        return !((*this) == other);
    }

    Vector3 ecf;
    LatLonAlt llh;
};

struct Planar
{
    Planar();

    bool operator==(const Planar& other) const
    {
        return uIax == other.uIax && uIay == other.uIay;
    }

    bool operator!=(const Planar& other) const
    {
        return !((*this) == other);
    }

    Vector3 uIax;
    Vector3 uIay;
};

struct HAE
{
    HAE();

    bool operator==(const HAE& other) const
    {
        return uIax == other.uIax && uIay == other.uIay;
    }

    bool operator!=(const HAE& other) const
    {
        return !((*this) == other);
    }

    LatLon uIax;
    LatLon uIay;
};

struct ReferenceSurface
{
    ReferenceSurface();

    bool operator==(const ReferenceSurface& other) const
    {
        return planar == other.planar &&
            hae == other.hae;
    }

    bool operator!=(const ReferenceSurface& other) const
    {
        return !((*this) == other);
    }

    mem::ScopedCopyablePtr<Planar> planar;
    mem::ScopedCopyablePtr<HAE> hae;
};

struct AreaType
{
    AreaType();

    bool operator==(const AreaType& other) const
    {
        return x1y1 == other.x1y1 &&
               x2y2 == other.x2y2;
    }

    bool operator!=(const AreaType& other) const
    {
        return !((*this) == other);
    }

    Vector2 x1y1;
    Vector2 x2y2;
    std::vector<Vector2> polygon;
};

struct LineSample
{
    LineSample();

    bool operator==(const LineSample& other) const
    {
        return line == other.line && sample == other.sample;
    }

    bool operator!=(const LineSample& other) const
    {
        return !((*this) == other);
    }

    size_t getIndex()
    {
        return mIndex;
    }

    void setIndex(size_t idx)
    {
        mIndex = idx;
    }

    double line;
    double sample;

private:
    size_t mIndex;
};

struct ImageAreaXExtent
{
    ImageAreaXExtent();

    bool operator==(const ImageAreaXExtent& other) const
    {
        return lineSpacing == other.lineSpacing &&
               firstLine == other.firstLine &&
               numLines == other.numLines;
    }

    bool operator!=(const ImageAreaXExtent& other) const
    {
        return !((*this) == other);
    }

    double lineSpacing;
    int firstLine;
    size_t numLines;
};

struct ImageAreaYExtent
{
    ImageAreaYExtent();

    bool operator==(const ImageAreaYExtent& other) const
    {
        return sampleSpacing == other.sampleSpacing &&
               firstSample == other.firstSample &&
               numSamples == other.numSamples;
    }

    bool operator!=(const ImageAreaYExtent& other) const
    {
        return !((*this) == other);
    }

    double sampleSpacing;
    int firstSample;
    size_t numSamples;
};

struct Segment
{
    Segment();

    bool operator==(const Segment& other) const
    {
        return startLine == other.startLine &&
               startSample == other.startSample &&
               endLine == other.endLine &&
               endSample == other.endSample &&
               polygon == other.polygon;
    }

    bool operator!=(const Segment& other) const
    {
        return !((*this) == other);
    }

    int startLine;
    int startSample;
    int endLine;
    int endSample;
    std::string identifier;
    std::vector<LineSample> polygon;
};

struct ImageGrid
{
    ImageGrid();

    bool operator==(const ImageGrid& other) const
    {
        return identifier == other.identifier &&
              iarpLocation == other.iarpLocation &&
              xExtent == other.xExtent &&
              yExtent == other.yExtent && 
              segments == other.segments;
    }

    bool operator!=(const ImageGrid& other) const
    {
        return !((*this) == other);
    }

    std::string identifier;
    LineSample iarpLocation;
    ImageAreaXExtent xExtent;
    ImageAreaYExtent yExtent;
    std::vector<Segment> segments;
};

struct SceneCoordinates
{
    SceneCoordinates();

    bool operator==(const SceneCoordinates& other) const
    {
        return earthModel == other.earthModel &&
               iarp == other.iarp &&
               referenceSurface == other.referenceSurface &&
               imageArea == other.imageArea &&
               imageAreaCorners == other.imageAreaCorners &&
               extendedArea == other.extendedArea &&
               imageGrid == other.imageGrid;
    }

    bool operator !=(const SceneCoordinates& other) const
    {
        return !((*this) == other);
    }

    EarthModelType earthModel;
    IARP iarp;
    ReferenceSurface referenceSurface;
    AreaType imageArea;
    LatLonCorners imageAreaCorners;
    mem::ScopedCopyablePtr<AreaType> extendedArea;
    mem::ScopedCopyablePtr<ImageGrid> imageGrid;
};

std::ostream& operator<< (std::ostream& os, const SceneCoordinates& d);
std::ostream& operator<< (std::ostream& os, const IARP& d);
std::ostream& operator<< (std::ostream& os, const ReferenceSurface& d);
std::ostream& operator<< (std::ostream& os, const Planar& d);
std::ostream& operator<< (std::ostream& os, const HAE& d);
std::ostream& operator<< (std::ostream& os, const AreaType& d);
std::ostream& operator<< (std::ostream& os, const ImageGrid& d);
std::ostream& operator<< (std::ostream& os, const LineSample& d);
std::ostream& operator<< (std::ostream& os, const ImageAreaXExtent& d);
std::ostream& operator<< (std::ostream& os, const ImageAreaYExtent& d);
std::ostream& operator<< (std::ostream& os, const Segment& d);
}

#endif
