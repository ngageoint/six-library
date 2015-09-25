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

#include <math/linear/Eigenvalue.h>
#include <scene/EllipsoidModel.h>
#include <six/csm/Utilities.h>
#include <six/csm/ProjectionModelBasedSensorModelHelper.h>

namespace
{
inline
double square(double val)
{
    return (val * val);
}

inline
double cube(double val)
{
    return (val * val * val);
}

math::linear::MatrixMxN<3, 3>
pseudoInverse(const math::linear::MatrixMxN<3, 3>& m)
{
    const math::linear::Matrix2D<double> input(m);
    const math::linear::Eigenvalue<double> Veig(input.transpose() * input);
    const math::linear::Vector<double> eigs = Veig.getRealEigenvalues();
    const double eigMax = std::max(std::max(std::sqrt(eigs[0]),
                                            std::sqrt(eigs[1])),
                                   std::sqrt(eigs[2]));

    if (eigMax <= std::numeric_limits<double>::epsilon() * 3.0)
    {
        return math::linear::MatrixMxN<3, 3>(0.0);
    }

    math::linear::Matrix2D<double> D(3, 3, 0.0);
    for (size_t ii = 0; ii < 3; ii++)
    {
        if  (std::sqrt(eigs[ii]) >=
             std::numeric_limits<double>::epsilon() * 3.0 * eigMax)
        {
            D[ii][ii] = 1.0 / std::sqrt(eigs[ii]);
        }
    }

    const math::linear::Eigenvalue<double> Ueig(input * input.transpose());

    const math::linear::Matrix2D<double> V = Veig.getV();
    const math::linear::Matrix2D<double> U = Ueig.getV();
    const math::linear::Matrix2D<double> returnMat = U * D * V.transpose();

    math::linear::MatrixMxN<3, 3> returnMatFixedSize;
    for (size_t ii = 0; ii < 3; ++ii)
    {
        for (size_t jj = 0; jj < 3; ++jj)
        {
            returnMatFixedSize[ii][jj] = returnMat[ii][jj];
        }
    }

    return returnMatFixedSize;
}

math::linear::MatrixMxN<3, 3>
inverse(const math::linear::MatrixMxN<3, 3>& mat)
{
    math::linear::MatrixMxN<3, 3> invMat;
    try
    {
        invMat = math::linear::inverse(mat);
    }
    catch (const except::Exception& ex)
    {
        invMat = pseudoInverse(mat);
    }

    return invMat;
}
}

namespace six
{
namespace CSM
{
ProjectionModelBasedSensorModelHelper::ProjectionModelBasedSensorModelHelper(
        const scene::SceneGeometry& geometry,
        mem::SharedPtr<scene::ProjectionModel> projection) :
    mProjection(projection),
    // NOTE: See member variable definition in header for why we're doing this
    mSensorCovariance(mProjection->getErrorCovariance(
            geometry.getReferencePosition()))
{
}

std::vector<double>
ProjectionModelBasedSensorModelHelper::computeGroundPartials(
        const csm::EcefCoord& groundPt) const
{
    try
    {
        const scene::Vector3 sceneGroundPt(toVector3(groundPt));
        const types::RowCol<double> imagePt =
                mProjection->sceneToImage(sceneGroundPt);

        const math::linear::MatrixMxN<2, 3> groundPartials =
                mProjection->sceneToImagePartials(sceneGroundPt, imagePt);

        // sceneToImagePartials() return value is in m/m,
        // computeGroundPartials wants pixels/m
        const types::RowCol<double> ss = getSampleSpacing();

        std::vector<double> groundPartialsVec(6);
        groundPartialsVec[0] = groundPartials[0][0] / ss.row;
        groundPartialsVec[1] = groundPartials[0][1] / ss.row;
        groundPartialsVec[2] = groundPartials[0][2] / ss.row;
        groundPartialsVec[3] = groundPartials[1][0] / ss.col;
        groundPartialsVec[4] = groundPartials[1][1] / ss.col;
        groundPartialsVec[5] = groundPartials[1][2] / ss.col;
        return groundPartialsVec;
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "ProjectionModelBasedSensorModelHelper::"
                           "computeGroundPartials");
    }
}

int ProjectionModelBasedSensorModelHelper::getNumParameters() const
{
    return scene::AdjustableParams::NUM_PARAMS;
}

double
ProjectionModelBasedSensorModelHelper::getParameterValue(int index) const
{
    if (index < 0 || index >= getNumParameters())
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " ProjectionModelBasedSensorModelHelper::getParameterValue");
    }
    return mProjection->getAdjustableParams()[index];
}

void ProjectionModelBasedSensorModelHelper::setParameterValue(int index,
                                                              double value)
{
    if (index < 0 || index >= getNumParameters())
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                "ProjectionModelBasedSensorModelHelper::setParameterValue");
    }
    mProjection->getAdjustableParams().mParams[index] = value;
}

double ProjectionModelBasedSensorModelHelper::getParameterCovariance(
        int index1,
        int index2) const
{
    if (index1 < 0 || index1 >= getNumParameters() ||
        index2 < 0 || index2 >= getNumParameters())
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                "ProjectionModelBasedSensorModelHelper::"
                "getParameterCovariance");
    }

    return mSensorCovariance(index1, index2);
}

void ProjectionModelBasedSensorModelHelper::setParameterCovariance(
        int index1,
        int index2,
        double covariance)
{
    if (index1 < 0 || index1 >= getNumParameters() ||
        index2 < 0 || index2 >= getNumParameters() )
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                "ProjectionModelBasedSensorModelHelper::"
                "setParameterCovariance");
    }

    mSensorCovariance(index1, index2) = covariance;
}

std::vector<double> ProjectionModelBasedSensorModelHelper::getUnmodeledError(
        const csm::ImageCoord& imagePt) const
{
    try
    {
        const types::RowCol<double> pixelPt = fromPixel(imagePt);
        const math::linear::MatrixMxN<2, 2, double> unmodeledError =
                mProjection->getUnmodeledErrorCovariance(pixelPt);

        // Get in the right units
        const types::RowCol<double> ss = getSampleSpacing();

        std::vector<double> unmodeledErrorVec(4);
        unmodeledErrorVec[0] = unmodeledError[0][0] / (ss.row * ss.row);
        unmodeledErrorVec[1] = unmodeledError[0][1] / (ss.row * ss.col);
        unmodeledErrorVec[2] = unmodeledError[1][0] / (ss.row * ss.col);
        unmodeledErrorVec[3] = unmodeledError[1][1] / (ss.col * ss.col);
        return unmodeledErrorVec;
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "ProjectionModelBasedSensorModelHelper::"
                           "getUnmodeledError");
    }
}

csm::RasterGM::SensorPartials
ProjectionModelBasedSensorModelHelper::computeSensorPartials(
        int index,
        const csm::EcefCoord& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    try
    {
        const scene::Vector3 sceneGroundPt(toVector3(groundPt));
        const types::RowCol<double> imagePt =
                mProjection->sceneToImage(sceneGroundPt);
        const types::RowCol<double> pixelPt = toPixel(imagePt);
        const csm::ImageCoord imageCoordPt(pixelPt.row, pixelPt.col);
        return computeSensorPartials(index,
                                     imageCoordPt,
                                     groundPt,
                                     desiredPrecision,
                                     achievedPrecision,
                                     warnings);
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                ex.getMessage(),
                "ProjectionModelBasedSensorModelHelper::"
                "computeSensorPartials");
    }
}

csm::RasterGM::SensorPartials
ProjectionModelBasedSensorModelHelper::computeSensorPartials(
        int index,
        const csm::ImageCoord& imagePt,
        const csm::EcefCoord& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    if (index < 0 || index >= getNumParameters())
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                        "Invalid index in call in function call",
                        "ProjectionModelBasedSensorModelHelper::"
                        "computeSensorPartials");
    }

    try
    {
        const scene::Vector3 sceneGroundPt(toVector3(groundPt));

        const types::RowCol<double> pixelPt = fromPixel(imagePt);

        const math::linear::MatrixMxN<2, 7> sensorPartials =
                mProjection->sceneToImageSensorPartials(sceneGroundPt,
                                                        pixelPt);

        // TODO: Currently no way to determine the actual precision that was
        //       achieved, so setting it to the desired precision
        if (achievedPrecision)
        {
            *achievedPrecision = desiredPrecision;
        }

        // Return value is in pixels / sensor units
        types::RowCol<double> ss = getSampleSpacing();
        return csm::RasterGM::SensorPartials(
                sensorPartials[0][index] / ss.row,
                sensorPartials[1][index] / ss.col);
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                ex.getMessage(),
                "ProjectionModelBasedSensorModelHelper::"
                "computeSensorPartials");
    }
}

std::vector<csm::RasterGM::SensorPartials>
ProjectionModelBasedSensorModelHelper::computeAllSensorPartials(
        const csm::EcefCoord& groundPt,
        csm::param::Set pSet,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    try
    {
        const scene::Vector3 sceneGroundPt(toVector3(groundPt));
        const types::RowCol<double> imagePt =
                mProjection->sceneToImage(sceneGroundPt);
        const types::RowCol<double> pixelPt = toPixel(imagePt);
        const csm::ImageCoord imageCoordPt(pixelPt.row, pixelPt.col);
        return computeAllSensorPartials(imageCoordPt,
                                        groundPt,
                                        pSet,
                                        desiredPrecision,
                                        achievedPrecision,
                                        warnings);
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                ex.getMessage(),
                "ProjectionModelBasedSensorModelHelper::"
                "computeSensorPartials");
    }
}

std::vector<csm::RasterGM::SensorPartials>
ProjectionModelBasedSensorModelHelper::computeAllSensorPartials(
        const csm::ImageCoord& imagePt,
        const csm::EcefCoord& groundPt,
        csm::param::Set pSet,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    try
    {
        const scene::Vector3 sceneGroundPt(toVector3(groundPt));

        const types::RowCol<double> pixelPt = fromPixel(imagePt);
        const math::linear::MatrixMxN<2, 7> sensorPartials =
                mProjection->sceneToImageSensorPartials(sceneGroundPt,
                                                        pixelPt);

        // TODO: Currently no way to determine the actual precision that was
        //       achieved, so setting it to the desired precision
        if (achievedPrecision)
        {
            *achievedPrecision = desiredPrecision;
        }

        // Return value is in pixels/sensor units
        std::vector<csm::RasterGM::SensorPartials>
                sensorPartialsVec(7, csm::RasterGM::SensorPartials(0,0));
        types::RowCol<double> ss = getSampleSpacing();
        for (size_t idx = 0; idx < 7; ++idx)
        {
            sensorPartialsVec[idx].first =
                    sensorPartials[0][idx] / ss.row;
            sensorPartialsVec[idx].second =
                    sensorPartials[1][idx] / ss.col;
        }
        return sensorPartialsVec;
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                ex.getMessage(),
                "ProjectionModelBasedSensorModelHelper::"
                "computeAllSensorPartials");
    }
}

csm::ImageCoord ProjectionModelBasedSensorModelHelper::groundToImageImpl(
        const csm::EcefCoord& groundPt,
        double desiredPrecision,
        double* achievedPrecision) const
{
    const scene::Vector3 sceneGroundPt(toVector3(groundPt));

    // TODO: Currently no way to specify desiredPrecision when calling
    //       sceneToImage()
    const types::RowCol<double> imagePt =
            mProjection->sceneToImage(sceneGroundPt);
    const types::RowCol<double> pixelPt = toPixel(imagePt);

    // TODO: Currently no way to determine the actual precision that was
    //       achieved, so setting it to the desired precision
    if (achievedPrecision)
    {
        *achievedPrecision = desiredPrecision;
    }

    return toImageCoord(pixelPt);
}

csm::ImageCoord ProjectionModelBasedSensorModelHelper::groundToImage(
        const csm::EcefCoord& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* ) const
{
    try
    {
        return groundToImageImpl(groundPt,
                                 desiredPrecision,
                                 achievedPrecision);
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "ProjectionModelBasedSensorModelHelper::"
                           "groundToImage");
    }
}

csm::ImageCoordCovar ProjectionModelBasedSensorModelHelper::groundToImage(
        const csm::EcefCoordCovar& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* ) const
{
    try
    {
        const csm::ImageCoord imagePt = groundToImageImpl(groundPt,
                                                          desiredPrecision,
                                                          achievedPrecision);
        const scene::Vector3 scenePt(toVector3(groundPt));
        types::RowCol<double> pixelPt(fromPixel(imagePt));
        // m^2
        // NOTE: See mSensorCovariance member variable definition in header
        //       for why we're not computing the sensor covariance for this
        //       point
        const math::linear::MatrixMxN<3, 3> userCovar(groundPt.covariance);
        const math::linear::MatrixMxN<2, 7> sensorPartials =
                mProjection->sceneToImageSensorPartials(scenePt);
        const math::linear::MatrixMxN<2, 3> imagePartials =
                mProjection->sceneToImagePartials(scenePt);
        const math::linear::MatrixMxN<2, 2> unmodeledCovar =
                mProjection->getUnmodeledErrorCovariance(pixelPt);
        const math::linear::MatrixMxN<2, 2> errorCovar =
                unmodeledCovar +
                (imagePartials * userCovar * imagePartials.transpose()) +
                (sensorPartials * mSensorCovariance *
                 sensorPartials.transpose());
        csm::ImageCoordCovar csmErrorCovar;
        types::RowCol<double> ss = getSampleSpacing();
        csmErrorCovar.line = imagePt.line;
        csmErrorCovar.samp = imagePt.samp;
        csmErrorCovar.covariance[0] =
                errorCovar[0][0] / (ss.row * ss.row);
        csmErrorCovar.covariance[1] =
                errorCovar[0][1] /
                (ss.row *
                 ss.col);
        csmErrorCovar.covariance[2] =
                errorCovar[1][0] /
                (ss.row *
                 ss.col);
        csmErrorCovar.covariance[3] =
                errorCovar[1][1] / (ss.col * ss.col);
        return csmErrorCovar;
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "ProjectionModelBasedSensorModelHelper::"
                           "groundToImage");
    }
}

csm::EcefCoord ProjectionModelBasedSensorModelHelper::imageToGround(
        const csm::ImageCoord& imagePt,
        double height,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* ) const
{
    try
    {
        const types::RowCol<double> imagePtMeters = fromPixel(imagePt);

        // TODO: imageToScene() supports specifying a height threshold in
        //       meters but it's not obvious how to convert that to a desired
        //       precision in pixels.  Likewise, not clear how to determine
        //       the achieved precision in pixels afterwards.
        const scene::Vector3 groundPt =
                mProjection->imageToScene(imagePtMeters, height);

        if (achievedPrecision)
        {
            *achievedPrecision = desiredPrecision;
        }

        return toEcefCoord(groundPt);
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "ProjectionModelBasedSensorModelHelper::"
                           "imageToGround");
    }
}

csm::EcefCoordCovar ProjectionModelBasedSensorModelHelper::imageToGround(
        const csm::ImageCoordCovar& imagePt,
        double height,
        double heightVariance,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    try
    {
        const csm::EcefCoord groundPt = imageToGround(imagePt,
                                                      height,
                                                      desiredPrecision,
                                                      achievedPrecision,
                                                      warnings);

        const double a = scene::WGS84EllipsoidModel::EQUATORIAL_RADIUS_METERS;
        const double b = scene::WGS84EllipsoidModel::POLAR_RADIUS_METERS;
        const scene::Vector3 scenePt(toVector3(groundPt));
        types::RowCol<double> pixelPt(fromPixel((csm::ImageCoord&) imagePt));

        // NOTE: See mSensorCovariance member variable definition in header
        //       for why we're not computing the sensor covariance for this
        //       point
        const math::linear::MatrixMxN<2, 2> userCovar(imagePt.covariance);
        math::linear::MatrixMxN<2, 2> unmodeledCovar =
                mProjection->getUnmodeledErrorCovariance(pixelPt);
        math::linear::MatrixMxN<2, 3> groundPartials =
                mProjection->sceneToImagePartials(scenePt);
        math::linear::MatrixMxN<2, 7> sensorPartials =
                mProjection->sceneToImageSensorPartials(scenePt);

        math::linear::MatrixMxN<10, 10> fullCovar(0.0);
        unmodeledCovar = unmodeledCovar + userCovar;
        fullCovar.addInPlace(unmodeledCovar, 0, 0);
        fullCovar[2][2] = heightVariance;
        fullCovar.addInPlace(mSensorCovariance, 3, 3);
        types::RowCol<double> ss = getSampleSpacing();
        for (size_t ii = 0; ii < 3; ++ii)
        {
            groundPartials[0][ii] /= ss.row;
            groundPartials[1][ii] /= ss.col;
        }

        for (size_t ii = 0; ii < 7; ++ii)
        {
            sensorPartials[0][ii] /= ss.row;
            sensorPartials[1][ii] /= ss.col;
        }

        math::linear::MatrixMxN<3, 3> B(0.0);
        B.addInPlace(groundPartials, 0, 0);
        B[2][0] = 2 * groundPt.x / square(a + height);
        B[2][1] = 2 * groundPt.y / square(a + height);
        B[2][2] = 2 * groundPt.z / square(b + height);

        math::linear::MatrixMxN<3, 10> A(0.0);
        A[2][2] = -2.0 * ((square(groundPt.x) + square(groundPt.y)) /
                          cube(a + height) +
                  square(groundPt.z) / cube(b + height));
        A.addInPlace(sensorPartials, 0, 3);
        A[0][0] = A[1][1] = 1.0;

        const math::linear::MatrixMxN<3, 3> Q = A * fullCovar * A.transpose();

        const math::linear::MatrixMxN<3, 3> Qinv = inverse(Q);

        const math::linear::MatrixMxN<3, 3> imageToGroundCovarInv =
                B.transpose() * Qinv * B;

        const math::linear::MatrixMxN<3, 3> errorCovar =
                inverse(imageToGroundCovarInv);

        csm::EcefCoordCovar csmErrorCovar;
        csmErrorCovar.x = groundPt.x;
        csmErrorCovar.y = groundPt.y;
        csmErrorCovar.z = groundPt.z;
        for (size_t ii = 0; ii < 3; ++ii)
        {
            for (size_t jj = 0; jj < 3; ++jj)
            {
                csmErrorCovar.covariance[ii * 3 + jj] = errorCovar[ii][jj];
            }
        }

        return csmErrorCovar;
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "ProjectionModelBasedSensorModelHelper::"
                           "imageToGround");
    }
}

double ProjectionModelBasedSensorModelHelper::getImageTime(
        const csm::ImageCoord& imagePt) const
{
    try
    {
        return mProjection->computeImageTime(fromPixel(imagePt));
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "ProjectionModelBasedSensorModelHelper::"
                           "getImageTime");
    }
}

csm::EcefCoord ProjectionModelBasedSensorModelHelper::getSensorPosition(
        const csm::ImageCoord& imagePt) const
{
    try
    {
        const double time = mProjection->computeImageTime(fromPixel(imagePt));
        return toEcefCoord(mProjection->computeARPPosition(time));
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "ProjectionModelBasedSensorModelHelper::"
                           "getSensorPosition");
    }
}

csm::EcefCoord
ProjectionModelBasedSensorModelHelper::getSensorPosition(double time) const
{
    try
    {
        return toEcefCoord(mProjection->computeARPPosition(time));
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "ProjectionModelBasedSensorModelHelper::"
                           "getSensorPosition");
    }
}

csm::EcefVector ProjectionModelBasedSensorModelHelper::getSensorVelocity(
        const csm::ImageCoord& imagePt) const
{
    try
    {
        const double time = mProjection->computeImageTime(fromPixel(imagePt));
        return toEcefVector(mProjection->computeARPVelocity(time));
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "ProjectionModelBasedSensorModelHelper::"
                           "getSensorVelocity");
    }
}

csm::EcefVector ProjectionModelBasedSensorModelHelper::getSensorVelocity(
        double time) const
{
    try
    {
        return toEcefVector(mProjection->computeARPVelocity(time));
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "ProjectionModelBasedSensorModelHelper::"
                           "getSensorVelocity");
    }
}

double ProjectionModelBasedSensorModelHelper::getCorrelationCoefficient(
        size_t cpGroupIndex,
        double deltaTime) const
{
    const scene::Errors errors = mProjection->getErrors();
    switch (cpGroupIndex)
    {
    case 0:
    {
        const double coeff = errors.mPositionCorrCoefZero -
                errors.mPositionDecorrRate * std::abs(deltaTime);
        return std::min(1.0, std::max(-1.0, coeff));
    }
    case 1:
    {
        const double coeff = errors.mRangeCorrCoefZero -
                errors.mRangeDecorrRate * std::abs(deltaTime);
        return std::min(1.0, std::max(-1.0, coeff));
    }
    default:
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                         "Invalid index in call in function call",
                         "ProjectionModelBasedSensorModelHelper::"
                         "getCorrelationCoefficient");
    }
}
}
}
