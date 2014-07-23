/* =========================================================================
 * This file is part of the CSM SIX Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2011, General Dynamics - Advanced Information Systems
 *
 * The CSM SIX Plugin is free software; you can redistribute it and/or modify
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
#include <six/csm/SIXSensorModel.h>

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
}

namespace six
{
namespace CSM
{
const char SIXSensorModel::FAMILY[] = CSM_GEOMETRIC_MODEL_FAMILY CSM_RASTER_FAMILY;



SIXSensorModel::SIXSensorModel()
{
}

std::string SIXSensorModel::getModelState() const
{
    return mSensorModelState;
}

void SIXSensorModel::replaceModelState(const std::string& argState)
{
    if (!argState.empty())
    {
        replaceModelStateImpl(argState);
    }
}

std::string SIXSensorModel::getTrajectoryIdentifier() const
{
    return "UNKNOWN";
}

std::string SIXSensorModel::getSensorType() const
{
    return CSM_SENSOR_TYPE_SAR;
}


csm::ImageCoord SIXSensorModel::getImageStart() const
{
    return csm::ImageCoord(0.0, 0.0);
}

int SIXSensorModel::getNumParameters() const
{
    return scene::AdjustableParams::NUM_PARAMS;
}

std::string SIXSensorModel::getParameterName(int index) const
{
    if (index < 0 || index >= getNumParameters())
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SIXSensorModel::getParameterName");
    }
    return scene::AdjustableParams::name(
            static_cast<scene::AdjustableParams::ParamsEnum>(index));
}

std::string SIXSensorModel::getParameterUnits(int index) const
{
    if (index < 0 || index >= getNumParameters())
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SIXSensorModel::getParameterUnits");
    }
    return scene::AdjustableParams::units(
            static_cast<scene::AdjustableParams::ParamsEnum>(index));
}

bool SIXSensorModel::hasShareableParameters() const
{
	return false;
}

bool SIXSensorModel::isParameterShareable(int index) const
{
    if (index < 0 || index >= getNumParameters())
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SIXSensorModel::isParameterShareable");
    }

    return false;
}

csm::SharingCriteria
SIXSensorModel::getParameterSharingCriteria(int index) const
{
    if (index < 0 || index >= getNumParameters())
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SIXSensorModel::getParameterSharingCriteria");
    }

    return csm::SharingCriteria();
}

double SIXSensorModel::getParameterValue(int index) const
{
    if (index < 0 || index >= getNumParameters())
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SIXSensorModel::getParameterValue");
    }
    return mProjection->getAdjustableParams()[index];
}

void SIXSensorModel::setParameterValue(int index, double value)
{
    if (index < 0 || index >= getNumParameters())
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SIXSensorModel::setParameterValue");
    }
    mProjection->getAdjustableParams().mParams[index] = value;
}

csm::param::Type SIXSensorModel::getParameterType(int index) const
{
    if (index < 0 || index >= getNumParameters())
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SIXSensorModel::getParameterType");
    }
    return mAdjustableTypes[index];
}

void SIXSensorModel::setParameterType(int index, csm::param::Type pType)
{
    if (index < 0 || index >= getNumParameters())
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SIXSensorModel::setParameterType");
    }
    mAdjustableTypes[index] = pType;
}

double SIXSensorModel::getParameterCovariance(int index1, int index2) const
{
    if (index1 < 0 || index1 >= getNumParameters() ||
        index2 < 0 || index2 >= getNumParameters())
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SIXSensorModel::getParameterCovariance");
    }

    return mSensorCovariance(index1, index2);
}

void SIXSensorModel::setParameterCovariance(int index1,
                                             int index2,
                                             double covariance)
{
    if (index1 < 0 || index1 >= getNumParameters() ||
        index2 < 0 || index2 >= getNumParameters() )
    {
        throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                "Invalid index in call in function call",
                " SIXSensorModel::setParameterCovariance");
    }

    mSensorCovariance(index1, index2) = covariance;
}

int SIXSensorModel::getNumGeometricCorrectionSwitches() const
{
    return 0;
}

std::string SIXSensorModel::getGeometricCorrectionName(int ) const
{
    throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no geometric correction switches",
                       "SIXSensorModel::getGeometricCorrectionName");
}

void SIXSensorModel::setGeometricCorrectionSwitch(int ,
                                                  bool ,
                                                  csm::param::Type )
{
    throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no geometric correction switches",
                       "SIXSensorModel::setGeometricCorrectionName");
}

bool SIXSensorModel::getGeometricCorrectionSwitch(int ) const
{
    throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no geometric correction switches",
                       "SIXSensorModel::getGeometricCorrectionSwitch");
}

std::pair< csm::ImageCoord, csm::ImageCoord>
SIXSensorModel::getValidImageRange() const
{
    return std::pair< csm::ImageCoord, csm::ImageCoord>(
            csm::ImageCoord(-99999.0, -99999.0),
            csm::ImageCoord( 99999.0,  99999.0));
}

std::pair<double,double> SIXSensorModel::getValidHeightRange() const
{
    return std::pair<double, double>(-99999.0, 99999);
}

std::vector<double>
SIXSensorModel::computeGroundPartials(const csm::EcefCoord& groundPt) const
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
                           "SIXSensorModel::computeGroundPartials");
    }
}

std::vector<double> SIXSensorModel::getUnmodeledError(
        const csm::ImageCoord& imagePt ) const
{
    try
    {
    	types::RowCol<double> pixelPt = fromPixel(imagePt);
        const math::linear::MatrixMxN<2, 2, double> unmodeledError =
                mProjection->getUnmodeledErrorCovariance( pixelPt );

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
                           "SIXSensorModel::getUnmodeledError");
    }
}

csm::RasterGM::SensorPartials SIXSensorModel::computeSensorPartials(
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
                "SIXSensorModel::computeSensorPartials");
    }

}

csm::RasterGM::SensorPartials SIXSensorModel::computeSensorPartials(
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
                        " SIXSensorModel::computeSensorPartials");
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
                "SIXSensorModel::computeSensorPartials");
    }
}

std::vector< csm::RasterGM::SensorPartials>
SIXSensorModel::computeAllSensorPartials(
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
                "SIXSensorModel::computeSensorPartials");
    }
}

std::vector< csm::RasterGM::SensorPartials>
SIXSensorModel::computeAllSensorPartials(
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
        std::vector<SensorPartials> sensorPartialsVec(7, SensorPartials(0,0));
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
                "SIXSensorModel::computeAllSensorPartials");
    }
}

csm::EcefCoord SIXSensorModel::getReferencePoint() const
{
    return toEcefCoord(mGeometry->getReferencePosition());
}

csm::EcefLocus SIXSensorModel::imageToProximateImagingLocus(
        const csm::ImageCoord& imagePt,
        const csm::EcefCoord& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    scene::Vector3 sceneGroundPt;
    sceneGroundPt[0] = groundPt.x;
    sceneGroundPt[1] = groundPt.y;
    sceneGroundPt[2] = groundPt.z;
    const double height = mECEFToLLA.transform(sceneGroundPt).getAlt();

    const double DELTA = 1.0;

    const csm::EcefCoord gp0 =
            imageToGround(imagePt, height, desiredPrecision, achievedPrecision, warnings);

    const csm::EcefCoord gp1 =
            imageToGround(imagePt, height + DELTA, desiredPrecision, achievedPrecision, warnings);

    // TODO: Not sure how to calculate achievedPrecision
    if (achievedPrecision)
    {
        *achievedPrecision = desiredPrecision;
    }

    csm::EcefLocus locus;
    locus.point = gp0;
    locus.direction.x = gp0.x - gp1.x;
    locus.direction.y = gp0.y - gp1.y;
    locus.direction.z = gp0.z - gp1.z;

    return locus;
}

const csm::CorrelationModel& SIXSensorModel::getCorrelationModel() const
{
    return mCorrelationModel;
}

std::vector<double> SIXSensorModel::getUnmodeledCrossCovariance(
        const csm::ImageCoord& ,
        const csm::ImageCoord& ) const
{
    return std::vector<double>(4, 0.0);
}

void SIXSensorModel::setReferencePoint(const csm::EcefCoord& )
{
    throw csm::Error(csm::Error::UNSUPPORTED_FUNCTION,
                       "Function not supported",
                       "SIXSensorModel::setReferencePoint");
}

csm::ImageCoord SIXSensorModel::groundToImageImpl(
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

csm::ImageCoord SIXSensorModel::groundToImage(
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
                           "SIXSensorModel::groundToImage");
    }
}

csm::ImageCoordCovar SIXSensorModel::groundToImage(
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
                           "SIXSensorModel::groundToImage");
    }
}

csm::EcefCoord SIXSensorModel::imageToGround(
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
                           "SIXSensorModel::imageToGround");
    }
}

math::linear::MatrixMxN<3,3> pseudoInverse(const math::linear::MatrixMxN<3,3>& m)
{
	math::linear::Matrix2D<double> input(3,3,(double *) m.mRaw);
	math::linear::Eigenvalue<double> Veig(input.transpose() * input);
	math::linear::Eigenvalue<double> Ueig(input * input.transpose());
	math::linear::Vector<double> eigs = Veig.getRealEigenvalues();
	double eigMax = std::max(std::max(std::sqrt(eigs[0]),std::sqrt(eigs[1])), std::sqrt(eigs[2]));
	math::linear::Matrix2D<double> D(3,3,(double) 0.0);
	if (eigMax <= std::numeric_limits<double>::epsilon() * 3.0)
	{
		return math::linear::MatrixMxN<3,3>((double) 0.0);
	}

	for (size_t ii = 0; ii < 3; ii++)
	{
		if  (std::sqrt(eigs[ii]) >= std::numeric_limits<double>::epsilon() * 3.0 * eigMax)
		{
			D[ii][ii] = 1/std::sqrt(eigs[ii]);
		}
	}
	math::linear::Matrix2D<double> V = Veig.getV();
	math::linear::Matrix2D<double> U = Ueig.getV();
	math::linear::Matrix2D<double> returnVal = U*D*V.transpose();

	// Feels dirty to use &returnVal[0][0], but mRaw is private and there is no get()
	return math::linear::MatrixMxN<3,3>((double *) &returnVal[0][0]);
}

csm::EcefCoordCovar SIXSensorModel::imageToGround(
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

        math::linear::MatrixMxN<3,3> Qinv;
        try
        {
        	Qinv = math::linear::inverse(Q);
        } catch (const except::Exception& ex)
        {
        	Qinv = pseudoInverse(Q);
        }

        const math::linear::MatrixMxN<3, 3> imageToGroundCovarInv =
                B.transpose() * Qinv * B;

        math::linear::MatrixMxN<3, 3> errorCovar;

        try
        {
        	errorCovar = math::linear::inverse(imageToGroundCovarInv);
        } catch (const except::Exception& ex)
        {
            errorCovar = pseudoInverse(imageToGroundCovarInv);
        }

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
                           "SIXSensorModel::imageToGround");
    }
}

csm::EcefLocus SIXSensorModel::imageToRemoteImagingLocus(
        const csm::ImageCoord& ,
        double ,
        double* ,
        csm::WarningList* ) const
{
    throw csm::Error(csm::Error::UNSUPPORTED_FUNCTION,
                       "Function not supported",
                       "SIXSensorModel::imageToRemoteImagingLocus");
}

csm::EcefVector SIXSensorModel::getIlluminationDirection(
        const csm::EcefCoord& groundPt) const
{
    const scene::Vector3 groundPos(toVector3(groundPt));

    scene::Vector3 illumVec = groundPos - mGeometry->getARPPosition();
    illumVec.normalize();

    return toEcefVector(illumVec);
}

double SIXSensorModel::getImageTime(const csm::ImageCoord& imagePt) const
{
    try
    {
        return mProjection->computeImageTime(fromPixel(imagePt));
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "SIXSensorModel::getImageTime");
    }
}

csm::EcefCoord
SIXSensorModel::getSensorPosition(const csm::ImageCoord& imagePt) const
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
                           "SIXSensorModel::getSensorPosition");
    }
}

csm::EcefCoord SIXSensorModel::getSensorPosition(double time) const
{
    try
    {
        return toEcefCoord(mProjection->computeARPPosition(time));
    }
    catch (const except::Exception& ex)
    {
        throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "SIXSensorModel::getSensorPosition");
    }
}

csm::EcefVector
SIXSensorModel::getSensorVelocity(const csm::ImageCoord& imagePt) const
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
                           "SIXSensorModel::getSensorVelocity");
    }
}

csm::EcefVector SIXSensorModel::getSensorVelocity(double time) const
{
    try
    {
        return toEcefVector(mProjection->computeARPVelocity(time));
    }
    catch (const except::Exception& ex)
    {
    	throw csm::Error(csm::Error::UNKNOWN_ERROR,
                           ex.getMessage(),
                           "SIXSensorModel::getSensorVelocity");
    }
}

void SIXSensorModel::setSchemaDir(const std::string& dataDir)
{
    sys::OS os;
    if (dataDir.empty())
    {
        mSchemaDirs.clear();

        // OK, but you better have your schema path set then
        std::string schemaPath;
        try
        {
            schemaPath = os.getEnv(six::SCHEMA_PATH);
        }
        catch(const except::Exception& )
        {
            throw csm::Error(csm::Error::SENSOR_MODEL_NOT_CONSTRUCTIBLE,
                    "Must specify SICD schema path via "
                    "Plugin::getDataDirectory() or " +
                    std::string(six::SCHEMA_PATH) + " environment variable",
                    "SIXSensorModel::setSchemaDir");
        }

        if (schemaPath.empty())
        {
            throw csm::Error(csm::Error::SENSOR_MODEL_NOT_CONSTRUCTIBLE,
                    std::string(six::SCHEMA_PATH) +
                    " environment variable is set but is empty",
                    "SIXSensorModel::setSchemaDir");
        }
    }
    else
    {
        const std::string schemaDir =
                sys::Path(dataDir).join("schema").join("six");
        if (!os.exists(schemaDir))
        {
            throw csm::Error(csm::Error::SENSOR_MODEL_NOT_CONSTRUCTIBLE,
                    "Schema directory '" + schemaDir + "' does not exist",
                    "SIXSensorModel::setSchemaDir");
        }

        mSchemaDirs.resize(1);
        mSchemaDirs[0] = schemaDir;
    }
}

template<typename _T> math::linear::Matrix2D<_T> matrixSqrt(const math::linear::Matrix2D<_T>& m)
{
	math::linear::Eigenvalue<_T> Eig(m);
	math::linear::Matrix2D<_T> D;
	Eig.getD(D);
	math::linear::Matrix2D<_T> V = Eig.getV();
	for (size_t ii = 0; ii < m.rows(); ii++)
	{
		if (D[ii][ii] < 0)
		{
			throw except::Exception(Ctxt("Matrix is not Positive-Definite"));
		}
		D[ii][ii] = std::sqrt(D[ii][ii]);
	}
	math::linear::Matrix2D<_T> returnVal = V*D*V.transpose();
	return returnVal;
}

template<typename _T> void printVector(const std::vector<_T> m)
{
	std::cout << "(";
	for (size_t ii = 0; ii < m.size(); ii++)
	{
		std::cout << std::setprecision(15) << m[ii] << ", ";
	}
	std::cout << ")"<<std::endl;

}
template<typename _T> void printMatrix(const math::linear::Matrix2D<_T> m)
{
	std::cout << "[ ";
	for (size_t ii = 0; ii < m.rows(); ii++)
	{
		for (size_t jj = 0; jj < m.cols(); jj++)
		{
			std::cout << m[ii][jj] << ", ";
		}
		std::cout << ";" << std::endl;
	}
}


std::vector<double> SIXSensorModel::getCrossCovarianceMatrix(
		const GeometricModel& comparisonModel,
		csm::param::Set pSet,
		const GeometricModelList& otherModels) const
{
	std::vector<int> paramSetP1 =  this->getParameterSetIndices(pSet);
	std::vector<int> paramSetP2 = comparisonModel.getParameterSetIndices(pSet);
	if (comparisonModel.getPlatformIdentifier() == this->getPlatformIdentifier())
	{
		//if (comparisonModel.getImageIdentifier() == this->getImageIdentifier())
		//{
		//	return this->getCovarianceMatrix( pSet);
		//}
		const SIXSensorModel* comparisonSIXModel = reinterpret_cast<const SIXSensorModel*>(&comparisonModel);
		size_t numGroups = this->getNumCorrelationParameterGroups();
		//size_t numParams = this->getNumSensorModelParameters();
		six::DateTime timeP1 = getSIXDateTime();
		six::DateTime timeP2 = comparisonSIXModel->getSIXDateTime();
		double time = (double) 1000 * (timeP1.getTimeInMillis() - timeP2.getTimeInMillis());
		std::vector<size_t> m(numGroups);
		//math::linear::MatrixMxN<paramSetP1.size(),paramSetP1.size()> returnVal((double) 0.0);
		std::vector<double> returnVal(paramSetP1.size() * paramSetP2.size(), (double) 0.0);
		for (size_t ii = 0; ii < numGroups; ii++)
		{
			m[ii] = 0;
			double corrCoeff = this->getCorrelationCoefficient(ii, time);
			for (size_t jj = 0; jj < paramSetP1.size(); jj++)
			{
				if (this->getCorrelationParameterGroup(paramSetP1[jj]) == ii)
				{
					m[ii]++;
				}
			}
			math::linear::Matrix2D<double> covarP1(m[ii],m[ii], 0.0);
			math::linear::Matrix2D<double> covarP2(m[ii],m[ii], 0.0);
			size_t jjP = 0;
			size_t kkP = 0;
			for (size_t jj = 0; jj < paramSetP1.size(); jj++)
			{
				kkP = 0;
				if (this->getCorrelationParameterGroup(paramSetP1[jj]) == ii)
				{
					for (size_t kk = 0; kk < paramSetP1.size(); kk++)
					{
						if (this->getCorrelationParameterGroup(paramSetP1[kk]) == ii)
						{
							covarP1[jjP][kkP] = this->getParameterCovariance(jj,kk);
							covarP2[jjP][kkP] = comparisonModel.getParameterCovariance(jj,kk);
							kkP++;
						}
					}
					jjP++;
				}
			}
			math::linear::Matrix2D<double> crossCovar = matrixSqrt(covarP1) * matrixSqrt(covarP2);
			crossCovar.scale(corrCoeff);

			jjP = 0;
			kkP = 0;
			for (size_t jj = 0; jj < paramSetP1.size(); jj++)
			{
				kkP =0;
				if (this->getCorrelationParameterGroup(paramSetP1[jj]) == ii)
				{
					for (size_t kk = 0; kk < paramSetP1.size(); kk++)
					{
						if (this->getCorrelationParameterGroup(paramSetP1[kk]) == ii)
						{
							returnVal[paramSetP2.size()*jj+kk] = crossCovar(jjP,kkP);
							kkP++;
						}
					}
					jjP++;
				}
			}
		}
		return returnVal;
	} else
	{
		return std::vector<double>(paramSetP1.size() * paramSetP2.size(), (double) 0.0);
	}
}

size_t SIXSensorModel::getNumSensorModelParameters() const
{
	return static_cast<size_t>(scene::AdjustableParams::NUM_PARAMS);
}

size_t SIXSensorModel::getNumCorrelationParameterGroups() const
{
	return static_cast<size_t>(scene::AdjustableParams::NUM_CORR_GROUPS);
}

int SIXSensorModel::getCorrelationParameterGroup(size_t smParamIndex)
const
{
	return scene::AdjustableParams::group(
			static_cast<scene::AdjustableParams::ParamsEnum>(smParamIndex));
}

double SIXSensorModel::getCorrelationCoefficient(size_t cpGroupIndex,
                                   double deltaTime) const
{
	scene::Errors errors = mProjection->getErrors();
	switch (cpGroupIndex)
	{
	case (0):
			return std::min(1.0, std::max(-1.0,
					errors.mPositionCorrCoefZero - errors.mPositionDecorrRate * std::abs(deltaTime)));
	case (1):
			return std::min(1.0, std::max(-1.0,
					errors.mRangeCorrCoefZero - errors.mRangeDecorrRate * std::abs(deltaTime)));
	default:
		throw(csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                    " Invalid index in call in function call ",
                    "SIXSensorModel::getCorrelationCoefficient"));
	}
}

}
}
