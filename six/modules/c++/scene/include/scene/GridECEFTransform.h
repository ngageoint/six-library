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
#ifndef __SCENE_GRID_ECEF_TRANSFORM_H__
#define __SCENE_GRID_ECEF_TRANSFORM_H__

#include <types/RowCol.h>
#include <scene/Types.h>

namespace scene
{
/*!
 * \class GridECEFTransform
 * \brief Used to convert from row/col pixel space to ECEF space and vice
 * versa
 */
class GridECEFTransform
{
public:
    virtual ~GridECEFTransform();

    /*
     * Converts from row/col pixel space to ECEF space
     * \param pixel row/col pixel
     *
     * \return Corresponding ECEF point
     */
    virtual
    Vector3 rowColToECEF(const types::RowCol<double>& pixel) const = 0;

    /*
     * Converts from row/col pixel space to ECEF space
     * \param row Row in pixel space
     * \param col Column in pixel space
     *
     * \return Corresponding ECEF point
     */
    Vector3 rowColToECEF(double row, double col) const
    {
        return rowColToECEF(types::RowCol<double>(row, col));
    }

    /*
     * Converts from ECEF space to row/col pixel
     * \param p3 ECEF point
     *
     * \return Corresponding row/col point
     */
    virtual types::RowCol<double> ecefToRowCol(const Vector3& p3) const = 0;
};

/*
 * \class PlanarGridECEFTransform
 * \brief Implementation for a planar gridded display (PGD)
 */
class PlanarGridECEFTransform : public GridECEFTransform
{
public:
    /*
     * The appropriate SICD fields to use are provided for if you're using
     * this to convert with respect to a SICD's output plane.  Note that this
     * only applies to SICDs that define the output plane via the optional
     * RadarCollection/Area element.
     * plane = sicdData->radarCollection->area->plane
     *
     * \param sampleSpacing Sample spacing in meters/pixel.
     * sampleSpacing.row = plane->xDirection->spacing
     * sampleSpacing.col = plane->yDirection->spacing
     * \param sceneCenter Scene center point in pixels
     * sceneCenter = plane->referencePoint->rowCol
     * \param row Output plane row unit vector
     * row = plane->xDirection->unitVector
     * \param col Output plane col unit vector
     * col = plane->yDirection->unitVector
     * \param refPt Scene center point in ECEF
     * refPt = plane->referencePoint->ecef
     *
     */
    PlanarGridECEFTransform(const types::RowCol<double>& sampleSpacing,
                            const types::RowCol<double>& sceneCenter,
                            const Vector3& row,
                            const Vector3& col,
                            const Vector3& refPt);

    using GridECEFTransform::rowColToECEF;

    virtual Vector3 rowColToECEF(const types::RowCol<double>& pixel) const;

    virtual types::RowCol<double> ecefToRowCol(const Vector3& p3) const;

private:
    types::RowCol<double> mSampleSpacing;
    types::RowCol<double> mSceneCenter;
    Vector3 mRow;
    Vector3 mCol;
    Vector3 mRefPt;
};

/*
 * \class CylindricalGridECEFTransform
 * \brief Implementation for a cylindrical gridded display (CGD)
 */
class CylindricalGridECEFTransform : public GridECEFTransform
{
public:
    CylindricalGridECEFTransform(const types::RowCol<double>& sampleSpacing,
                                 const types::RowCol<double>& sceneCenter,
                                 const Vector3& row,
                                 const Vector3& col,
                                 const Vector3& normal,
                                 const Vector3& refPt,
                                 double radiusOfCurvStripmap);

    using GridECEFTransform::rowColToECEF;

    virtual Vector3 rowColToECEF(const types::RowCol<double>& pixel) const;

    virtual types::RowCol<double> ecefToRowCol(const Vector3& p3) const;

private:
    types::RowCol<double> mSampleSpacing;
    types::RowCol<double> mSceneCenter;
    Vector3 mRow;
    Vector3 mCol;
    Vector3 mNormal;
    Vector3 mRefPt;
    double mRs;
};

/*
 * \class GeographicRowColECEFTransform
 * \brief Implementation for a geographic gridded display (GGD)
 */
struct GeographicGridECEFTransform : public GridECEFTransform
{
    GeographicGridECEFTransform(const types::RowCol<double>& sampleSpacing,
                                const types::RowCol<double>& sceneCenter,
                                const LatLonAlt& refPt);

    using GridECEFTransform::rowColToECEF;

    virtual Vector3 rowColToECEF(const types::RowCol<double>& pixel) const;

    // NOTE: The altitude information is ignored in this calculation - it is
    //       assumed to lie on the GGD surface.  If p3 is above or below the
    //       surface, the row/col that's returned will correspond to the same
    //       lat/lon, but its altitude will be assumed to be the same as the
    //       reference point.  Taking this row/col and converting back to ECEF
    //       via rowColToECEF() will then result in a different ECEF than p3.
    //       If p3 does lie on the GGD surface, round-tripping will result in
    //       the same ECEF.
    virtual types::RowCol<double> ecefToRowCol(const Vector3& p3) const;

private:
    types::RowCol<double> mSampleSpacing;
    types::RowCol<double> mSceneCenter;
    LatLonAlt mRefPt;
};
}

#endif
