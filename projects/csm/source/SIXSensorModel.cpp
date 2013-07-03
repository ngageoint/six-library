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

namespace six
{
namespace csm
{
const char SIXSensorModel::FAMILY[] = CSM_GEOMETRIC_MODEL_FAMILY CSM_RASTER_FAMILY;

std::string SIXSensorModel::getTrajectoryIdentifier() const
{
    return "UNKNOWN";
}

std::string SIXSensorModel::getSensorType() const
{
    return CSM_SENSOR_TYPE_SAR;
}

int SIXSensorModel::getNumParameters() const
{
    return 0;
}

std::string SIXSensorModel::getParameterName(int ) const
{
    throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no parameters",
                       "SIXSensorModel::getParameterName");
}

std::string SIXSensorModel::getParameterUnits(int ) const
{
    throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no parameters",
                       "SIXSensorModel::getParameterUnits");
}

bool SIXSensorModel::hasShareableParameters() const
{
    return false;
}

bool SIXSensorModel::isParameterShareable(int i) const
{
    throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no parameters",
                       "SIXSensorModel::isParameterShareable");
}

::csm::SharingCriteria SIXSensorModel::getParameterSharingCriteria(int ) const
{
    throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no parameters",
                       "SIXSensorModel::getParameterSharingCriteria");
}

double SIXSensorModel::getParameterValue(int ) const
{
    throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no parameters",
                       "SIXSensorModel::getParameterValue");
}

void SIXSensorModel::setParameterValue(int , double )
{
    throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no parameters",
                       "SIXSensorModel::setParameterValue");
}

::csm::param::Type SIXSensorModel::getParameterType(int ) const
{
    throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no parameters",
                       "SIXSensorModel::getParameterType");
}

void SIXSensorModel::setParameterType(int , ::csm::param::Type )
{
    throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no parameters",
                       "SIXSensorModel::setParameterType");
}

double SIXSensorModel::getParameterCovariance(int , int ) const
{
    throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no parameters",
                       "SIXSensorModel::getParameterCovariance");
}

void SIXSensorModel::setParameterCovariance(int , int , double )
{
    throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no parameters",
                       "SIXSensorModel::setParameterCovariance");
}

int SIXSensorModel::getNumGeometricCorrectionSwitches() const
{
    return 0;
}

std::string SIXSensorModel::getGeometricCorrectionName(int ) const
{
    throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no geometric correction switches",
                       "SIXSensorModel::getGeometricCorrectionName");
}

void SIXSensorModel::setGeometricCorrectionSwitch(int ,
                                                  bool ,
                                                  ::csm::param::Type )
{
    throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no geometric correction switches",
                       "SIXSensorModel::setGeometricCorrectionName");
}

bool SIXSensorModel::getGeometricCorrectionSwitch(int ) const
{
    throw ::csm::Error(::csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no geometric correction switches",
                       "SIXSensorModel::getGeometricCorrectionSwitch");
}

std::pair< ::csm::ImageCoord, ::csm::ImageCoord>
SIXSensorModel::getValidImageRange() const
{
    return std::pair< ::csm::ImageCoord, ::csm::ImageCoord>(
            ::csm::ImageCoord(-99999.0, -99999.0),
            ::csm::ImageCoord( 99999.0,  99999.0));
}

std::pair<double,double> SIXSensorModel::getValidHeightRange() const
{
    return std::pair<double, double>(-99999.0, 99999);
}

void SIXSensorModel::setReferencePoint(const ::csm::EcefCoord& )
{
    throw ::csm::Error(::csm::Error::UNSUPPORTED_FUNCTION,
                       "Function not supported",
                       "SIXSensorModel::setReferencePoint");
}

std::vector<double> SIXSensorModel::getCrossCovarianceMatrix(
       const ::csm::GeometricModel& ,
       ::csm::param::Set ,
       const ::csm::GeometricModel::GeometricModelList& ) const
{
    throw ::csm::Error(::csm::Error::UNSUPPORTED_FUNCTION,
                       "Function not supported",
                       "SIXSensorModel::getCrossCovarianceMatrix");
}

::csm::ImageCoordCovar SIXSensorModel::groundToImage(const ::csm::EcefCoordCovar& ,
                                                    double ,
                                                    double* ,
                                                    ::csm::WarningList* ) const
{
    throw ::csm::Error(::csm::Error::UNSUPPORTED_FUNCTION,
                       "Function not supported",
                       "SIXSensorModel::groundToImage");
}

::csm::EcefCoordCovar SIXSensorModel::imageToGround(const ::csm::ImageCoordCovar& ,
                                                   double ,
                                                   double ,
                                                   double ,
                                                   double* ,
                                                   ::csm::WarningList* ) const
{
    throw ::csm::Error(::csm::Error::UNSUPPORTED_FUNCTION,
                       "Function not supported",
                       "SIXSensorModel::imageToGround");
}

::csm::EcefLocus SIXSensorModel::imageToProximateImagingLocus(
        const ::csm::ImageCoord& ,
        const ::csm::EcefCoord& ,
        double ,
        double* ,
        ::csm::WarningList* ) const
{
    throw ::csm::Error(::csm::Error::UNSUPPORTED_FUNCTION,
                       "Function not supported",
                       "SIXSensorModel::imageToProximateImagingLocus");
}

::csm::EcefLocus SIXSensorModel::imageToRemoteImagingLocus(
        const ::csm::ImageCoord& ,
        double ,
        double* ,
        ::csm::WarningList* ) const
{
    throw ::csm::Error(::csm::Error::UNSUPPORTED_FUNCTION,
                       "Function not supported",
                       "SIXSensorModel::imageToRemoteImagingLocus");
}

::csm::RasterGM::SensorPartials SIXSensorModel::computeSensorPartials(
        int ,
        const ::csm::EcefCoord& ,
        double ,
        double* ,
        ::csm::WarningList* ) const
{
    throw ::csm::Error(::csm::Error::UNSUPPORTED_FUNCTION,
                       "Function not supported",
                       "SIXSensorModel::computeSensorPartials");
}

::csm::RasterGM::SensorPartials SIXSensorModel::computeSensorPartials(
        int ,
        const ::csm::ImageCoord& ,
        const ::csm::EcefCoord& ,
        double ,
        double* ,
        ::csm::WarningList* ) const
{
    throw ::csm::Error(::csm::Error::UNSUPPORTED_FUNCTION,
                       "Function not supported",
                       "SIXSensorModel::computeSensorPartials");
}

std::vector<double>
SIXSensorModel::computeGroundPartials(const ::csm::EcefCoord& ) const
{
    throw ::csm::Error(::csm::Error::UNSUPPORTED_FUNCTION,
                       "Function not supported",
                       "SIXSensorModel::computeGroundPartials");
}

const ::csm::CorrelationModel& SIXSensorModel::getCorrelationModel() const
{
    throw ::csm::Error(::csm::Error::UNSUPPORTED_FUNCTION,
                       "Function not supported",
                       "SIXSensorModel::getCorrelationModel");
}

std::vector<double> SIXSensorModel::getUnmodeledCrossCovariance(
        const ::csm::ImageCoord& ,
        const ::csm::ImageCoord& ) const
{
    throw ::csm::Error(::csm::Error::UNSUPPORTED_FUNCTION,
                       "Function not supported",
                       "SIXSensorModel::getUnmodeledCrossCovariance");
}
}
}
