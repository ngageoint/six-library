/* =========================================================================
 * This file is part of the CSM SIX Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#include <cmath>
#include <limits>

#include "Error.h"
#include <six/csm/Utilities.h>
#include <six/csm/SIXSensorModel.h>

namespace
{
// TODO: Does this belong in math.linear?
template<typename T>
math::linear::Matrix2D<T> matrixSqrt(const math::linear::Matrix2D<T>& m)
{
    const math::linear::Eigenvalue<T> eig(m);
    math::linear::Matrix2D<T> diag;
    eig.getD(diag);

    for (size_t ii = 0; ii < m.rows(); ii++)
    {
        if (diag[ii][ii] < 0)
        {
            throw except::Exception(Ctxt("Matrix is not Positive-Definite"));
        }

        diag[ii][ii] = std::sqrt(diag[ii][ii]);
    }

    const math::linear::Matrix2D<T> eigenVec = eig.getV();
    return (eigenVec * diag * eigenVec.transpose());
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

std::string SIXSensorModel::getReferenceDateAndTime() const
{
    return getReferenceDateAndTimeImpl().format("%Y%m%dT%H%M%.2SZ");
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
    return mHelper->getNumParameters();
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
    return mHelper->getParameterValue(index);
}

void SIXSensorModel::setParameterValue(int index, double value)
{
    mHelper->setParameterValue(index, value);
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
    return mHelper->getParameterCovariance(index1, index2);
}

void SIXSensorModel::setParameterCovariance(int index1,
                                            int index2,
                                            double covariance)
{
    mHelper->setParameterCovariance(index1, index2, covariance);
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
    return mHelper->computeGroundPartials(groundPt);
}

std::vector<double> SIXSensorModel::getUnmodeledError(
        const csm::ImageCoord& imagePt) const
{
    return mHelper->getUnmodeledError(imagePt);
}

csm::RasterGM::SensorPartials SIXSensorModel::computeSensorPartials(
        int index,
        const csm::EcefCoord& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    return mHelper->computeSensorPartials(index,
                                          groundPt,
                                          desiredPrecision,
                                          achievedPrecision,
                                          warnings);
}

csm::RasterGM::SensorPartials SIXSensorModel::computeSensorPartials(
        int index,
        const csm::ImageCoord& imagePt,
        const csm::EcefCoord& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    return mHelper->computeSensorPartials(index,
                                          imagePt,
                                          groundPt,
                                          desiredPrecision,
                                          achievedPrecision,
                                          warnings);
}

std::vector< csm::RasterGM::SensorPartials>
SIXSensorModel::computeAllSensorPartials(
        const csm::EcefCoord& groundPt,
        csm::param::Set pSet,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    return computeAllSensorPartials(groundPt,
                                    pSet,
                                    desiredPrecision,
                                    achievedPrecision,
                                    warnings);
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
    return computeAllSensorPartials(imagePt,
                                    groundPt,
                                    pSet,
                                    desiredPrecision,
                                    achievedPrecision,
                                    warnings);
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

    const csm::EcefCoord gp0 = imageToGround(imagePt,
                                             height,
                                             desiredPrecision,
                                             achievedPrecision,
                                             warnings);

    const csm::EcefCoord gp1 = imageToGround(imagePt,
                                             height + DELTA,
                                             desiredPrecision,
                                             achievedPrecision,
                                             warnings);

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

csm::ImageCoord SIXSensorModel::groundToImage(
        const csm::EcefCoord& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    return mHelper->groundToImage(groundPt,
                                  desiredPrecision,
                                  achievedPrecision,
                                  warnings);
}

csm::ImageCoordCovar SIXSensorModel::groundToImage(
        const csm::EcefCoordCovar& groundPt,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    return mHelper->groundToImage(groundPt,
                                  desiredPrecision,
                                  achievedPrecision,
                                  warnings);
}

csm::EcefCoord SIXSensorModel::imageToGround(
        const csm::ImageCoord& imagePt,
        double height,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    return mHelper->imageToGround(imagePt,
                                  height,
                                  desiredPrecision,
                                  achievedPrecision,
                                  warnings);
}

csm::EcefCoordCovar SIXSensorModel::imageToGround(
        const csm::ImageCoordCovar& imagePt,
        double height,
        double heightVariance,
        double desiredPrecision,
        double* achievedPrecision,
        csm::WarningList* warnings) const
{
    return mHelper->imageToGround(imagePt,
                                  height,
                                  heightVariance,
                                  desiredPrecision,
                                  achievedPrecision,
                                  warnings);
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
    return mHelper->getImageTime(imagePt);
}

csm::EcefCoord
SIXSensorModel::getSensorPosition(const csm::ImageCoord& imagePt) const
{
    return mHelper->getSensorPosition(imagePt);
}

csm::EcefCoord SIXSensorModel::getSensorPosition(double time) const
{
    return mHelper->getSensorPosition(time);
}

csm::EcefVector
SIXSensorModel::getSensorVelocity(const csm::ImageCoord& imagePt) const
{
    return mHelper->getSensorVelocity(imagePt);
}

csm::EcefVector SIXSensorModel::getSensorVelocity(double time) const
{
    return mHelper->getSensorVelocity(time);
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

std::vector<double> SIXSensorModel::getCrossCovarianceMatrix(
        const GeometricModel& comparisonModel,
        csm::param::Set pSet,
        const GeometricModelList& otherModels) const
{
    const std::vector<int> paramSetP1 = getParameterSetIndices(pSet);
    const std::vector<int> paramSetP2 =
            comparisonModel.getParameterSetIndices(pSet);
    std::vector<double> returnVal(paramSetP1.size() * paramSetP2.size(),
                                  0.0);

    if (comparisonModel.getPlatformIdentifier() == getPlatformIdentifier())
    {
        // NOTE: Used dynamic_cast here previously but using it with IAI's
        //       version of vts, it threw an exception.  Did older versions of
        //       vts not use the /GR flag (they've since reported it worked
        //       fine when our plugin used dynamic_cast)?
        const SIXSensorModel& comparisonSIXModel =
                (const SIXSensorModel&)comparisonModel;
        const size_t numGroups = getNumCorrelationParameterGroups();
        const six::DateTime timeP1 = getReferenceDateAndTimeImpl();
        const six::DateTime timeP2 =
                comparisonSIXModel.getReferenceDateAndTimeImpl();
        const double time =
                1000.0 * (timeP1.getTimeInMillis() - timeP2.getTimeInMillis());
        std::vector<size_t> m(numGroups);

        for (size_t ii = 0; ii < numGroups; ++ii)
        {
            m[ii] = 0;
            const double corrCoeff = getCorrelationCoefficient(ii, time);
            for (size_t jj = 0; jj < paramSetP1.size(); ++jj)
            {
                if (getCorrelationParameterGroup(paramSetP1[jj]) == ii)
                {
                    ++m[ii];
                }
            }
            math::linear::Matrix2D<double> covarP1(m[ii], m[ii], 0.0);
            math::linear::Matrix2D<double> covarP2(m[ii], m[ii], 0.0);
            size_t jjP = 0;
            size_t kkP = 0;
            for (size_t jj = 0; jj < paramSetP1.size(); ++jj)
            {
                kkP = 0;
                if (getCorrelationParameterGroup(paramSetP1[jj]) == ii)
                {
                    for (size_t kk = 0; kk < paramSetP1.size(); ++kk)
                    {
                        if (getCorrelationParameterGroup(paramSetP1[kk]) == ii)
                        {
                            covarP1[jjP][kkP] = getParameterCovariance(jj, kk);
                            covarP2[jjP][kkP] =
                                    comparisonModel.getParameterCovariance(jj, kk);
                            ++kkP;
                        }
                    }
                    ++jjP;
                }
            }
            math::linear::Matrix2D<double> crossCovar;
            if (getImageIdentifier() == comparisonModel.getImageIdentifier())
            {
                crossCovar = covarP1;
            }
            else
            {
                crossCovar =
                        matrixSqrt(covarP1) * matrixSqrt(covarP2);
                crossCovar.scale(corrCoeff);
            }

            jjP = 0;
            kkP = 0;
            for (size_t jj = 0; jj < paramSetP1.size(); ++jj)
            {
                kkP = 0;
                if (getCorrelationParameterGroup(paramSetP1[jj]) == ii)
                {
                    for (size_t kk = 0; kk < paramSetP1.size(); ++kk)
                    {
                        if (getCorrelationParameterGroup(paramSetP1[kk]) == ii)
                        {
                            returnVal[paramSetP2.size() * jj + kk] =
                                    crossCovar(jjP, kkP);
                            ++kkP;
                        }
                    }
                    ++jjP;
                }
            }
        }
    }

    return returnVal;
}

size_t SIXSensorModel::getCorrelationParameterGroup(size_t smParamIndex)
{
    return scene::AdjustableParams::group(
            static_cast<scene::AdjustableParams::ParamsEnum>(smParamIndex));
}

double SIXSensorModel::getCorrelationCoefficient(size_t cpGroupIndex,
                                                 double deltaTime) const
{
    return mHelper->getCorrelationCoefficient(cpGroupIndex, deltaTime);
}
}
}
