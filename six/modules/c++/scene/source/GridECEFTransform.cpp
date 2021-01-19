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
#include <scene/GridECEFTransform.h>

#include <nitf/coda-oss.hpp>
#include <except/Exception.h>
#include <scene/Utilities.h>

namespace scene
{
GridECEFTransform::~GridECEFTransform()
{
}

PlanarGridECEFTransform::PlanarGridECEFTransform(
        const types::RowCol<double>& sampleSpacing,
        const types::RowCol<double>& sceneCenter,
        const Vector3& row,
        const Vector3& col,
        const Vector3& refPt) :
    mSampleSpacing(sampleSpacing),
    mSceneCenter(sceneCenter),
    mRow(row),
    mCol(col),
    mRefPt(refPt)
{
}

Vector3 PlanarGridECEFTransform::rowColToECEF(
        const types::RowCol<double>& pixel) const
{
    const scene::Vector3 rowDisp =
        mRow * mSampleSpacing.row * (pixel.row - mSceneCenter.row);

    const scene::Vector3 colDisp =
        mCol * mSampleSpacing.col * (pixel.col - mSceneCenter.col);

    return (mRefPt + rowDisp + colDisp);
}

types::RowCol<double>
PlanarGridECEFTransform::ecefToRowCol(const Vector3& p3) const
{
    const scene::Vector3 disp(p3 - mRefPt);
    const types::RowCol<double> rgAz(disp.dot(mRow), disp.dot(mCol));
    const types::RowCol<double>
            pixel(rgAz.row / mSampleSpacing.row + mSceneCenter.row,
                  rgAz.col / mSampleSpacing.col + mSceneCenter.col);
    return pixel;
}

CylindricalGridECEFTransform::CylindricalGridECEFTransform(
        const types::RowCol<double>& sampleSpacing,
        const types::RowCol<double>& sceneCenter,
        const Vector3& row,
        const Vector3& col,
        const Vector3& normal,
        const Vector3& refPt,
        double radiusOfCurvStripmap) :
    mSampleSpacing(sampleSpacing),
    mSceneCenter(sceneCenter),
    mRow(row),
    mCol(col),
    mNormal(normal),
    mRefPt(refPt),
    mRs(radiusOfCurvStripmap)
{
}

Vector3 CylindricalGridECEFTransform::rowColToECEF(
        const types::RowCol<double>& pixel) const
{
    const scene::Vector3 rowDisp =
            mRow * mSampleSpacing.row * (pixel.row - mSceneCenter.row);

    const double theta =
            mSampleSpacing.col * (pixel.col - mSceneCenter.col) / mRs;
    const scene::Vector3 colDisp = mCol * mRs * sin(theta);

    const scene::Vector3 hgtDisp = mNormal * mRs * (cos(theta) - 1);
    return (mRefPt + rowDisp + colDisp + hgtDisp);
}

types::RowCol<double>
CylindricalGridECEFTransform::ecefToRowCol(const Vector3& ) const
{
    // TODO
    throw except::NotImplementedException(Ctxt("Not implemented"));
}

GeographicGridECEFTransform::GeographicGridECEFTransform(
        const types::RowCol<double>& sampleSpacing,
        const types::RowCol<double>& sceneCenter,
        const LatLonAlt& refPt) :
    mSampleSpacing(sampleSpacing),
    mSceneCenter(sceneCenter),
    mRefPt(refPt)
{
}

Vector3 GeographicGridECEFTransform::rowColToECEF(
        const types::RowCol<double>& pixel) const
{
    const scene::LatLonAlt lla(
        mRefPt.getLat() - (pixel.row - mSceneCenter.row) *
            mSampleSpacing.row / 3600.0,
        mRefPt.getLon() + (pixel.col - mSceneCenter.col) *
            mSampleSpacing.col / 3600.0,
        mRefPt.getAlt());

    return scene::Utilities::latLonToECEF(lla);
}

types::RowCol<double>
GeographicGridECEFTransform::ecefToRowCol(const Vector3& p3) const
{
    const scene::LatLonAlt lla(scene::Utilities::ecefToLatLon(p3));

    const types::RowCol<double> rowCol(
        (mRefPt.getLat() - lla.getLat()) * 3600.0 / mSampleSpacing.row +
            mSceneCenter.row,
        (lla.getLon() - mRefPt.getLon()) * 3600.0 / mSampleSpacing.col +
            mSceneCenter.col);

    return rowCol;
}
}
