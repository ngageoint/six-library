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

#include <sys/Optional.h>
#include <mem/ScopedCopyablePtr.h>

#include <cphd/Enums.h>
#include <cphd/Types.h>

#include <six/Types.h>

namespace cphd
{
/*!
 *  \struct IARP
 *
 *  \brief Image Area Reference Point (IARP).
 *
 *  The IARP is the origin of the Image Area Coordinate system
 *  (IAX, IAY, IAZ)
 */
struct IARP
{
    //! Constructor
    IARP();

    //! Equality operator
    bool operator==(const IARP& other) const
    {
        return ecf == other.ecf && llh == other.llh;
    }
    bool operator!=(const IARP& other) const
    {
        return !((*this) == other);
    }

    //! IARP position in ECF coordinates.
    Vector3 ecf;

    //! IARP position in geodetic coordinates.
    LatLonAlt llh;
};

/*!
 *  \struct Planar
 *
 *  \brief Parameters for SurfaceType = PLANAR
 */
struct Planar
{
    //! Constructor
    Planar();

    //! Equality operator
    bool operator==(const Planar& other) const
    {
        return uIax == other.uIax && uIay == other.uIay;
    }
    bool operator!=(const Planar& other) const
    {
        return !((*this) == other);
    }

    //! Image Area X-coordinate (IAX) unit vector in
    //! ECF coordinates. Unit vector uIAX is in the
    //! +IAX direction.
    Vector3 uIax;

    //! Image Area Y-coordinate (IAY) unit vector in
    //! ECF coordinates. Unit vector uIAY is in the
    //! +IAY direction.
    Vector3 uIay;
};

/*!
 *  \struct HAE
 *
 *  \brief Parameters for SurfaceType = HAE
 */
struct HAE
{
    //! Constructor
    HAE();

    //! Equality operator
    bool operator==(const HAE& other) const
    {
        return uIax == other.uIax && uIay == other.uIay;
    }
    bool operator!=(const HAE& other) const
    {
        return !((*this) == other);
    }

    //! Image coordinate IAX "unit vector" expressed as
    //! an increment in latitude and longitude. The
    //! precise increments in LL for a 1.0 meter
    //! increment in image coordinate value IAX
    LatLon uIax;

    //! Image coordinate IAY "unit vector" expressed as
    //! an increment in latitude and longitude. The
    //! precise increments in LL for a 1.0 meter
    //! increment in image coordinate value IAY
    LatLon uIay;
};

/*
 *  \struct ReferenceSurface
 *
 *  \brief Parameters that define the Reference Surface
 *  used for the product.
 */
struct ReferenceSurface
{
    //! Constructor
    ReferenceSurface();

    //! Equality operator
    bool operator==(const ReferenceSurface& other) const
    {
        return planar == other.planar &&
            hae == other.hae;
    }
    bool operator!=(const ReferenceSurface& other) const
    {
        return !((*this) == other);
    }

    //! (Conditional) The reference surface is a plane that
    //! contains the IARP
    mem::ScopedCopyablePtr<Planar> planar;

    //! (Conditional) The reference surface is the surface
    //! of constant HAE = IARP_HAE.
    mem::ScopedCopyablePtr<HAE> hae;
};

/*!
 *  \struct AreaType
 *
 *  Image Area is defined by a rectangle aligned with
 *  Image Area coordinates (IAX, IAY). May be
 *  reduced by the optional polygon
 */
struct AreaType
{
    //! Constructor
    AreaType();

    //! Equality operator
    bool operator==(const AreaType& other) const
    {
        return x1y1 == other.x1y1 &&
               x2y2 == other.x2y2 &&
               polygon == other.polygon;
    }
    bool operator!=(const AreaType& other) const
    {
        return !((*this) == other);
    }

    //! Corner of the Image Area Rectangle in Image
    //! Area coordinates, (IA_X1, IA_Y1).
    Vector2 x1y1;

    //! Corner of the Image Area Rectangle in Image
    //! Area coordinates, (IA_X2, IA_Y2).
    Vector2 x2y2;

    //! (Optional) Polygon that reduces the full resolution
    //!  image area within the Image Area Rectangle
    std::vector<Vector2> polygon;
};

/*!
 *  \sturct LineSample
 *
 *  \brief Identifies a parent tag with children Line and Sample.
 *
 *  The children are each of type DBL.
 */
struct LineSample
{
    //! Constructor
    LineSample();

    //! Equality operator
    bool operator==(const LineSample& other) const
    {
        return line == other.line && sample == other.sample;
    }
    bool operator!=(const LineSample& other) const
    {
        return !((*this) == other);
    }

    /*!
     *  Get index associated with LineSample
     */
    size_t getIndex()
    {
        return mIndex;
    }

    /*!
     *  Set index associated with LineSample
     */
    void setIndex(size_t idx)
    {
        mIndex = idx;
    }

    //! Line
    double line;

    //! Sample
    double sample;

private:
    // Stores index of LineSample
    size_t mIndex;
};

/*!
 *  \struct ImageAreaXExtent
 *
 *  \brief Increasing line index is in the +IAX direction
 */
struct ImageAreaXExtent
{
    //! Constructor
    ImageAreaXExtent();

    //! Equality operator
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

    //! Line spacing (L_SP)
    double lineSpacing;

    //! Index of the first line (IG_L1)
    int firstLine;

    //! Number of lines
    size_t numLines;
};

/*!
 *  \struct ImageAreaYExtent
 *
 *  \brief Increasing line index is in the +IAY direction
 */
struct ImageAreaYExtent
{
    //! Constructor
    ImageAreaYExtent();

    //! Equality operator
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

    //! Sample spacing (S_SP)
    double sampleSpacing;

    //! Index of the first sample (IG_S1)
    int firstSample;

    //! Number of samples
    size_t numSamples;
};

/*!
 *  \struct Segment
 *
 *  \brief Each segment is a rectangle from Line = L1 to L2
 *  and from Sample = S1 to S2.
 */
struct Segment
{
    //! Constructor
    Segment();

    //! Equality operator
    bool operator==(const Segment& other) const
    {
        return identifier == other.identifier &&
               startLine == other.startLine &&
               startSample == other.startSample &&
               endLine == other.endLine &&
               endSample == other.endSample &&
               polygon == other.polygon;
    }
    bool operator!=(const Segment& other) const
    {
        return !((*this) == other);
    }

    //! Start line (L1) of the segment
    int startLine;

    //! Start sample (S1) of the segment
    int startSample;

    //! End line (L2) of the segment
    int endLine;

    //! End sample (S2) of the segment.
    int endSample;

    //! String that uniquely identifies the Image Segment
    //! (Seg_ID)
    std::string identifier;

    //! (Optional) Polygon that describes a portion of
    //! the segment rectangle
    std::vector<LineSample> polygon;
};

/*!
 *  \struct ImageGrid
 *
 *  Parameters that describe a geo-referenced image
 *  grid for image data products that may be formed
 *  from the CPHD signal array(s).
 */
struct ImageGrid
{
    //! Constructor
    ImageGrid();

    //! Equality operator
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

    //! (Optional) String uniquely identifies Image Grid
    std::string identifier;

    //! IARP grid location.
    LineSample iarpLocation;

    //! Increasing line index is in the +IAX direction
    ImageAreaXExtent xExtent;

    //! Increasing line index is in the +IAY direction
    ImageAreaYExtent yExtent;

    //! (Optional) List of 1 or more image grid segments defined
    //! relative to the image grid
    std::vector<Segment> segments;
};

/*!
 *  \struct SceeneCoordinates
 *
 *  \brief Parameters that define geographic coordinates for
 *  in the imaged scene.
 */
struct SceneCoordinates
{
    //! Constructor
    SceneCoordinates();

    //! Equality operator
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

    //! Specifies the earth model used for specifying
    //! geodetic coordinates
    EarthModelType earthModel;

    //! Image Area Reference Point (IARP). The IARP is
    //! the origin of the Image Area Coordinate system
    //! (IAX, IAY, IAZ)
    IARP iarp;

    //! Parameters that define the Reference Surface
    //! used for the product
    ReferenceSurface referenceSurface;

    //! Image Area is defined by a rectangle aligned with
    //! Image Area coordinates (IAX, IAY)
    AreaType imageArea;

    //! Set of 4 Image Area Corner Points (IACPs) that
    //! bound the full resolution image area
    LatLonCorners imageAreaCorners;

    //! (Optional) Extended Area is defined by a rectangle
    //! aligned with Image Area coordinates (IAX, IAY)
    mem::ScopedCopyablePtr<AreaType> extendedArea;

    //! (Optional) Parameters that describe a geo-referenced image
    //! grid for image data products that may be formed
    //! from the CPHD signal array(s)
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
