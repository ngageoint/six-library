#include "Error.h"

#include <six/csm/SensorModelHelper.h>

namespace six
{
namespace CSM
{
SensorModelHelper::~SensorModelHelper()
{
}

csm::ImageCoord SensorModelHelper::groundToImage(
        const csm::EcefCoord& groundPt) const
{
    double desiredPrecision = 0.0001;
    return groundToImage(groundPt, desiredPrecision, NULL, NULL);
}

std::vector<double>
SensorModelHelper::computeGroundPartials(const csm::EcefCoord& groundPt) const
{
    const double DELTA = 1.0;

    csm::EcefCoord gp = groundPt;
    const csm::ImageCoord imagePt0 = groundToImage(gp);

    gp.x += DELTA;
    const csm::ImageCoord imagePtDeltaX = groundToImage(gp);

    gp = groundPt;
    gp.y += DELTA;
    const csm::ImageCoord imagePtDeltaY = groundToImage(gp);

    gp = groundPt;
    gp.z += DELTA;
    const csm::ImageCoord imagePtDeltaZ = groundToImage(gp);

    std::vector<double> partials;
    partials.push_back((imagePtDeltaX.line - imagePt0.line) / DELTA);
    partials.push_back((imagePtDeltaY.line - imagePt0.line) / DELTA);
    partials.push_back((imagePtDeltaZ.line - imagePt0.line) / DELTA);
    partials.push_back((imagePtDeltaX.samp - imagePt0.samp) / DELTA);
    partials.push_back((imagePtDeltaY.samp - imagePt0.samp) / DELTA);
    partials.push_back((imagePtDeltaZ.samp - imagePt0.samp) / DELTA);
    return partials;
}

int SensorModelHelper::getNumParameters() const
{
    return 0;
}

double SensorModelHelper::getParameterValue(int ) const
{
    throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                     "Sensor model has no parameters",
                     "SensorModelHelper::getParameterValue");
}

void SensorModelHelper::setParameterValue(int , double )
{
    throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                     "Sensor model has no parameters",
                     "SensorModelHelper::setParameterValue");
}

double SensorModelHelper::getParameterCovariance(int , int ) const
{
    throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no parameters",
                       "SensorModelHelper::getParameterCovariance");
}

void SensorModelHelper::setParameterCovariance(int , int , double )
{
    throw csm::Error(csm::Error::INDEX_OUT_OF_RANGE,
                       "Sensor model has no parameters",
                       "SensorModelHelper::setParameterCovariance");
}

std::vector<double> SensorModelHelper::getUnmodeledError(
            const csm::ImageCoord& ) const
{
    return std::vector<double>(4, 0.0);
}

csm::RasterGM::SensorPartials SensorModelHelper::computeSensorPartials(
        int ,
        const csm::EcefCoord& ,
        double ,
        double* ,
        csm::WarningList* ) const
{
    throw csm::Error(csm::Error::UNSUPPORTED_FUNCTION,
                     "Function not supported",
                     "SensorModelHelper::computeSensorPartials");
}

csm::RasterGM::SensorPartials SensorModelHelper::computeSensorPartials(
        int ,
        const csm::ImageCoord& ,
        const csm::EcefCoord& ,
        double ,
        double* ,
        csm::WarningList* ) const
{
    throw csm::Error(csm::Error::UNSUPPORTED_FUNCTION,
                     "Function not supported",
                     "SensorModelHelper::computeSensorPartials");
}

std::vector<csm::RasterGM::SensorPartials>
SensorModelHelper::computeAllSensorPartials(
        const csm::EcefCoord& ,
        csm::param::Set ,
        double ,
        double* ,
        csm::WarningList* ) const
{
    throw csm::Error(csm::Error::UNSUPPORTED_FUNCTION,
                     "Function not supported",
                     "SensorModelHelper::computeAllSensorPartials");
}

std::vector<csm::RasterGM::SensorPartials>
SensorModelHelper::computeAllSensorPartials(
        const csm::ImageCoord& ,
        const csm::EcefCoord& ,
        csm::param::Set ,
        double ,
        double* ,
        csm::WarningList* ) const
{
    throw csm::Error(csm::Error::UNSUPPORTED_FUNCTION,
                     "Function not supported",
                     "SensorModelHelper::computeAllSensorPartials");
}

double SensorModelHelper::getCorrelationCoefficient(size_t ,
                                                    double ) const
{
    throw csm::Error(csm::Error::UNSUPPORTED_FUNCTION,
                     "Function not supported",
                     "SensorModelHelper::getCorrelationCoefficient");
}
}
}
