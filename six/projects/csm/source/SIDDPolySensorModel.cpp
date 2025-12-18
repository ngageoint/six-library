/* =========================================================================
 * This file is part of the CSM SICD Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2025, Arka Group, L.P.
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
#include <assert.h>
#include <io/StringStream.h>
#include <six/XmlLite.h>
#include <six/csm/SIDDPolySensorModel.h>
#include <six/sidd/Utilities.h>

#include "Error.h"

namespace
{
class NullProjectionModel : public scene::ProjectionModel
{
public:
    NullProjectionModel(const six::sidd::DerivedData* data) :
        ProjectionModel(
                scene::Vector3(),
                data->measurement->projection.get()->referencePoint.ecef,
                data->measurement->arpPoly,
                {},
                six::sidd::Utilities::getSideOfTrack(data))
    {
    }

    virtual types::RowCol<double> computeImageCoordinates(
            const scene::Vector3& imagePlanePoint) const
    {
        throw csm::Error(csm::Error::UNSUPPORTED_FUNCTION,
                         "Function not implemented",
                         "NullProjectionModel::computeImageCoordinates");
    }

    virtual scene::Vector3 imageGridToECEF(
            const types::RowCol<double> gridPt) const
    {
        throw csm::Error(csm::Error::UNSUPPORTED_FUNCTION,
                         "Function not implemented",
                         "NullProjectionModel::computeImageCoordinates");
    }

    virtual void computeContour(const scene::Vector3& arpCOA,
                                const scene::Vector3& velCOA,
                                double timeCOA,
                                const types::RowCol<double>& imageGridPoint,
                                double* r,
                                double* rDot) const
    {
        throw csm::Error(csm::Error::UNSUPPORTED_FUNCTION,
                         "Function not implemented",
                         "NullProjectionModel::computeImageCoordinates");
    }
};
}

namespace six
{
namespace CSM
{
// The VERSION field should be kept in sync with that in SICDSensorModel.  The
// SICD version is used to determine the filename for cmake builds.
const csm::Version SIDDPolySensorModel::VERSION(1, 2, 0);
const char SIDDPolySensorModel::NAME[] = "SIDD_POLY_SENSOR_MODEL";

SIDDPolySensorModel::SIDDPolySensorModel(const csm::Isd& isd,
                                         const std::string& dataDir)
{
    setSchemaDir(dataDir);
    initializeFromISD(isd);
}

SIDDPolySensorModel::SIDDPolySensorModel(const std::string& sensorModelState,
                                         const std::string& dataDir)
{
    setSchemaDir(dataDir);
    replaceModelStateImpl(sensorModelState);
}

csm::Version SIDDPolySensorModel::getVersion() const
{
    return VERSION;
}

std::string SIDDPolySensorModel::getModelName() const
{
    return NAME;
}

std::string SIDDPolySensorModel::getPedigree() const
{
    return (mData->getSource() + "_" + NAME + "_SAR");
}

double SIDDPolySensorModel::getImageTime(const csm::ImageCoord& imagePt) const
{
    return 0.0;
}

csm::EcefCoord SIDDPolySensorModel::imageToGround(
        const csm::ImageCoord& imagePt,
        double height,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    try
    {
        std::pair<csm::ImageCoord, csm::ImageCoord> validRange =
                getValidImageRange();
        if (imagePt.line < validRange.first.line ||
            imagePt.line > validRange.second.line ||
            imagePt.samp < validRange.first.samp ||
            imagePt.samp > validRange.second.samp)
        {
            if (warnings)
            {
                warnings->push_back(
                        csm::Warning(csm::Warning::IMAGE_COORD_OUT_OF_BOUNDS,
                                     "Image coordinate is out of bounds and "
                                     "may not be valid",
                                     "SIDDPolySensorModel::imageToGround"));
            }
        }

        scene::LatLonAlt lla;
        lla.setLat(mPolyProj->rowColToLat(imagePt.line, imagePt.samp));
        lla.setLon(mPolyProj->rowColToLon(imagePt.line, imagePt.samp));
        if (!mPolyProj->rowColToAlt.empty())
        {
            lla.setAlt(mPolyProj->rowColToAlt(imagePt.line, imagePt.samp));
        }
        else
        {
            lla.setAlt(mECEFToLLA.transform(mGeometry->getReferencePosition())
                               .getAlt());
        }

        if (lla.getAlt() != height && warnings)
        {
            warnings->push_back(
                    csm::Warning(csm::Warning::NO_INTERSECTION,
                                 "Specified height does not match model height",
                                 "SIDDPolySensorModel::imageToGround"));
        }

        const scene::Vector3 groundPt = mLLAToECEF.transform(lla);

        if (achievedPrecision)
        {
            // Return 0 for non-iterative result
            *achievedPrecision = 0;
        }

        return toEcefCoord(groundPt);
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                         ex.getMessage(),
                         "SIDDPolySensorModel::imageToGround");
    }
}

csm::EcefCoordCovar SIDDPolySensorModel::imageToGround(
        const csm::ImageCoordCovar& imagePt,
        double height,
        double heightVariance,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    try
    {
        csm::ImageCoord imagePtNoCovar(imagePt.line, imagePt.samp);
        csm::EcefCoord groundPtNoCovar = imageToGround(imagePtNoCovar,
                                                       height,
                                                       desiredPrecision,
                                                       achievedPrecision,
                                                       warnings);
        csm::EcefCoordCovar groundPt(groundPtNoCovar.x,
                                     groundPtNoCovar.y,
                                     groundPtNoCovar.z);
        return groundPt;
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                         ex.getMessage(),
                         "SIDDPolySensorModel::imageToGround");
    }
}

csm::ImageCoord SIDDPolySensorModel::groundToImage(
        const csm::EcefCoord& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    try
    {
        scene::LatLonAlt lla = mECEFToLLA.transform(toVector3(groundPt));
        csm::ImageCoord ic;
        ic.line = mPolyProj->latLonToRow(lla.getLat(), lla.getLon());
        ic.samp = mPolyProj->latLonToCol(lla.getLat(), lla.getLon());

        if (achievedPrecision)
        {
            // Return 0 for non-iterative result
            *achievedPrecision = 0;
        }

        std::pair<csm::ImageCoord, csm::ImageCoord> validRange =
                getValidImageRange();
        if (ic.line < validRange.first.line ||
            ic.line > validRange.second.line ||
            ic.samp < validRange.first.samp || ic.samp > validRange.second.samp)
        {
            if (warnings)
            {
                warnings->push_back(
                        csm::Warning(csm::Warning::IMAGE_COORD_OUT_OF_BOUNDS,
                                     "Calculated image coordinate is out of "
                                     "bounds and may not be valid",
                                     "SIDDPolySensorModel::groundToImage"));
            }
        }

        return ic;
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                         ex.getMessage(),
                         "SIDDPolySensorModel::groundToImage");
    }
}

csm::ImageCoordCovar SIDDPolySensorModel::groundToImage(
        const csm::EcefCoordCovar& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    try
    {
        csm::EcefCoord groundPtNoCovar(groundPt.x, groundPt.y, groundPt.z);
        csm::ImageCoord imagePtNoCovar = groundToImage(groundPtNoCovar,
                                                       desiredPrecision,
                                                       achievedPrecision,
                                                       warnings);
        csm::ImageCoordCovar imagePt(imagePtNoCovar.line, imagePtNoCovar.samp);
        return imagePt;
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                         ex.getMessage(),
                         "SIDDPolySensorModel::groundToImage");
    }
}

std::vector<double>
SIDDPolySensorModel::computeGroundPartials(const csm::EcefCoord& groundPt) const
{
    double offset_m = 0.001;  // meters
    csm::ImageCoord center = groundToImage(groundPt, 0.0, nullptr, nullptr);

    csm::EcefCoord offsetGroundPt = groundPt;
    offsetGroundPt.x += offset_m;
    csm::ImageCoord offset_x = groundToImage(offsetGroundPt, 0.0, nullptr, nullptr);

    offsetGroundPt = groundPt;
    offsetGroundPt.y += offset_m;
    csm::ImageCoord offset_y = groundToImage(offsetGroundPt, 0.0, nullptr, nullptr);

    offsetGroundPt = groundPt;
    offsetGroundPt.z += offset_m;
    csm::ImageCoord offset_z = groundToImage(offsetGroundPt, 0.0, nullptr, nullptr);

    std::vector<double> groundPartialsVec(6);
    groundPartialsVec[0] = (offset_x.line - center.line) / offset_m;
    groundPartialsVec[1] = (offset_y.line - center.line) / offset_m;
    groundPartialsVec[2] = (offset_z.line - center.line) / offset_m;
    groundPartialsVec[3] = (offset_x.samp - center.samp) / offset_m;
    groundPartialsVec[4] = (offset_y.samp - center.samp) / offset_m;
    groundPartialsVec[5] = (offset_z.samp - center.samp) / offset_m;
    return groundPartialsVec;
}

std::vector<double> SIDDPolySensorModel::getUnmodeledCrossCovariance(
        const csm::ImageCoord& pt1, const csm::ImageCoord& pt2) const
{
    return {0.0, 0.0, 0.0, 0.0};
}

std::pair<csm::ImageCoord, csm::ImageCoord>
SIDDPolySensorModel::getValidImageRange() const
{
    // We do not know how the polynomial fit might break down outside of it, so
    // indicate that the model is valid only over the exact image array.
    return std::pair<csm::ImageCoord, csm::ImageCoord>(
            getImageStart(),
            {getImageStart().line + getImageSize().line,
             getImageStart().samp + getImageSize().samp});
}

std::pair<double, double> SIDDPolySensorModel::getValidHeightRange() const
{
    // If optional rowColToAlt polynomial is provided, evaluate it across a grid
    // of points to determine the valid height range.  Otherwise, use the SCP
    // height as the only valid height.
    if (!mPolyProj->rowColToAlt.empty())
    {
        constexpr size_t numPts = 21;
        double l0 = getImageStart().line;
        double nl = getImageSize().line;
        double s0 = getImageStart().samp;
        double ns = getImageSize().samp;

        double minAlt = mPolyProj->rowColToAlt(l0, s0);
        double maxAlt = minAlt;
        for (size_t li = 0; li < numPts; li++)
        {
            double line = l0 + li / (numPts - 1) * nl;
            for (size_t si = 0; si < numPts; si++)
            {
                double samp = s0 + si / (numPts - 1) * ns;
                double alt = mPolyProj->rowColToAlt(line, samp);
                minAlt = std::min(minAlt, alt);
                maxAlt = std::max(maxAlt, alt);
            }
        }
        return std::pair<double, double>(minAlt, maxAlt);
    }
    else
    {
        double scpAlt = mECEFToLLA.transform(mGeometry->getReferencePosition())
                                .getAlt();
        return std::pair<double, double>(scpAlt, scpAlt);
    }
}

void SIDDPolySensorModel::reinitialize(SIXSensorModelState& modelState)
{
    mGeometry = six::sidd::Utilities::getSceneGeometry(mData.get());

    if (mData->measurement->projection->projectionType !=
        six::ProjectionType::POLYNOMIAL)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                         "Image projection type is not Polynomial",
                         "SIDDPolySensorModel::reinitialize");
    }
    mPolyProj = dynamic_cast<six::sidd::PolynomialProjection*>(
            mData->measurement->projection.get());

    if (!modelState.getDatasetName().empty())
    {
        mData->setName(modelState.getDatasetName());
    }

    mProjection.reset(new NullProjectionModel(mData.get()));

    // Force sensor covariance override, and force values to 0, so adjustable
    // parameters get disabled and the user can't see or change them
    modelState.setOverrideSensorCovariance(true);
    for (size_t idx1 = 0; idx1 < scene::AdjustableParams::NUM_PARAMS; idx1++)
    {
        for (size_t idx2 = 0; idx2 < scene::AdjustableParams::NUM_PARAMS;
             idx2++)
        {
            modelState.setSensorCovariance(idx1, idx2, 0.0);
        }
    }
    SIXSensorModel::reinitialize(modelState);
}
}
}
