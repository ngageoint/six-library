/* =========================================================================
 * This file is part of the CSM SICD Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
 *
 * The CSM SICD Plugin is free software; you can redistribute it and/or modify
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
 * License along with this program; if not,
 * see <http://www.gnu.org/licenses/>.
 *
 */

#include "Error.h"
#include "Warning.h"

#include <scene/Utilities.h>
#include <six/csm/Utilities.h>
#include <six/csm/SIDDPolyProjectionHelper.h>

namespace six
{
namespace CSM
{
SIDDPolyProjectionHelper::SIDDPolyProjectionHelper(
        mem::SharedPtr<six::sidd::DerivedData> data) :
    mData(data)
{
    const six::sidd::Projection* const projection =
            mData->measurement->projection.get();
    if (projection->projectionType == six::ProjectionType::POLYNOMIAL)
    {
        mProjection = reinterpret_cast<const six::sidd::PolynomialProjection*>(
                projection);
    }
    else
    {
        // This should never happen as we're checking the projection type in
        // SIDdSensorModel
        throw csm::Error(csm::Error::INVALID_SENSOR_MODEL_STATE,
                         "Unexpected projection type",
                         "SIDDPolyProjectionHelper constructor");
    }
}

csm::ImageCoord SIDDPolyProjectionHelper::groundToImage(
        const csm::EcefCoord& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    try
    {
        const scene::LatLonAlt latLon =
                scene::Utilities::ecefToLatLon(toVector3(groundPt));

        const csm::ImageCoord coord(
                mProjection->latLonToRow(latLon.getLat(), latLon.getLon()),
                mProjection->latLonToCol(latLon.getLat(), latLon.getLon()));

        if (achievedPrecision)
        {
            *achievedPrecision = desiredPrecision;
        }

        if (warnings)
        {
            const csm::Warning warning(
                    csm::Warning::PRECISION_NOT_MET,
                    "This method is imprecise.  Altitude not taken into "
                    "account.",
                    "SIDDPolyProjectionHelper::groundToImage");
            warnings->push_back(warning);
        }

        return coord;
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "SIDDPolyProjectionHelper::groundToImage");
    }
}

csm::ImageCoordCovar SIDDPolyProjectionHelper::groundToImage(
        const csm::EcefCoordCovar& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    const csm::EcefCoord& groundPtNoCovar = groundPt;

    const csm::ImageCoord imageCoord = groundToImage(groundPtNoCovar,
                                                     desiredPrecision,
                                                     achievedPrecision,
                                                     warnings);

    // Let it default construct to zero out the covariance
    csm::ImageCoordCovar imageCoordCovar;
    imageCoordCovar.line = imageCoord.line;
    imageCoordCovar.samp = imageCoord.samp;
    return imageCoordCovar;
}

csm::EcefCoord SIDDPolyProjectionHelper::imageToGround(
        const csm::ImageCoord& imagePt,
        double height,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    try
    {
        // TODO: Since they send in the height, we'll never use the optional
        //       rowColToAlt poly.  Is this correct?
        const scene::LatLonAlt latLon(
                mProjection->rowColToLat(imagePt.line, imagePt.samp),
                mProjection->rowColToLon(imagePt.line, imagePt.samp),
                height);

        if (achievedPrecision)
        {
            *achievedPrecision = desiredPrecision;
        }

        if (warnings)
        {
            const csm::Warning warning(
                    csm::Warning::PRECISION_NOT_MET,
                    "This method is imprecise.  Altitude poly not taken into "
                    "account.",
                    "SIDDPolyProjectionHelper::imageToGround");
            warnings->push_back(warning);
        }

        return toEcefCoord(scene::Utilities::latLonToECEF(latLon));
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "SIDDPolyProjectionHelper::imageToGround");
    }
}

csm::EcefCoordCovar SIDDPolyProjectionHelper::imageToGround(
        const csm::ImageCoordCovar& imagePt,
        double height,
        double ,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    const csm::EcefCoord groundPt = imageToGround(imagePt,
                                                  height,
                                                  desiredPrecision,
                                                  achievedPrecision,
                                                  warnings);

    // Let it default construct to set the covariance to all 0's
    csm::EcefCoordCovar groundPtCovar;
    groundPtCovar.x = groundPt.x;
    groundPtCovar.y = groundPt.y;
    groundPtCovar.z = groundPt.z;

    return groundPtCovar;
}

double
SIDDPolyProjectionHelper::getImageTime(const csm::ImageCoord& ) const
{
    // We don't have a time COA poly so approximate with the center time
    return getCenterTime();
}

csm::EcefCoord SIDDPolyProjectionHelper::getSensorPosition(
        const csm::ImageCoord& imagePt) const
{
    try
    {
        return toEcefCoord(mData->measurement->arpPoly(getCenterTime()));
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                         ex.getMessage(),
                         "SIDDPolyProjectionHelper::getSensorPosition");
    }
}

csm::EcefCoord SIDDPolyProjectionHelper::getSensorPosition(double time) const
{
    try
    {
        return toEcefCoord(mData->measurement->arpPoly(time));
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                         ex.getMessage(),
                         "SIDDPolyProjectionHelper::getSensorPosition");
    }
}

csm::EcefVector SIDDPolyProjectionHelper::getSensorVelocity(
        const csm::ImageCoord& imagePt) const
{
    try
    {
        const six::PolyXYZ arpVel = mData->measurement->arpPoly.derivative();
        return toEcefVector(arpVel(getCenterTime()));
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                         ex.getMessage(),
                         "SIDDPolyProjectionHelper::getSensorVelocity");
    }
}

csm::EcefVector SIDDPolyProjectionHelper::getSensorVelocity(double time) const
{
    try
    {
        const six::PolyXYZ arpVel = mData->measurement->arpPoly.derivative();
        return toEcefVector(arpVel(time));
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                         ex.getMessage(),
                         "SIDDPolyProjectionHelper::getSensorVelocity");
    }
}
}
}
