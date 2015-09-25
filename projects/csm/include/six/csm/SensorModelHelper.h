#ifndef __SIX_CSM_SENSOR_MODEL_HELPER_H__
#define __SIX_CSM_SENSOR_MODEL_HELPER_H__

#include <vector>

#include "csm.h"
#include "RasterGM.h"

#include <types/RowCol.h>

namespace six
{
namespace CSM
{
class SensorModelHelper
{
public:
    virtual ~SensorModelHelper();

    // By default these basically all throw or return 0's as appropriate
    virtual
    std::vector<double>
    computeGroundPartials(const csm::EcefCoord& groundPt) const;

    virtual int getNumParameters() const;

    virtual double getParameterValue(int index) const;

    virtual void setParameterValue(int index, double value);

    virtual double getParameterCovariance(int index1, int index2) const;

    virtual void setParameterCovariance(int index1,
                                        int index2,
                                        double covariance);

    virtual std::vector<double> getUnmodeledError(
            const csm::ImageCoord& imagePt) const;

    virtual csm::RasterGM::SensorPartials computeSensorPartials(
            int index,
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual csm::RasterGM::SensorPartials computeSensorPartials(
            int index,
            const csm::ImageCoord& imagePt,
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual
    std::vector<csm::RasterGM::SensorPartials> computeAllSensorPartials(
            const csm::EcefCoord& groundPt,
            csm::param::Set pSet,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual
    std::vector<csm::RasterGM::SensorPartials> computeAllSensorPartials(
            const csm::ImageCoord& imagePt,
            const csm::EcefCoord& groundPt,
            csm::param::Set pSet,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    virtual double getCorrelationCoefficient(size_t cpGroupIndex,
                                             double deltaTime) const;

    // Just call the other groundToImage() overloading without sending in a
    // precision because it's not applicable anyway
    virtual csm::ImageCoord groundToImage(
            const csm::EcefCoord& groundPt) const;

public:
    virtual csm::ImageCoord groundToImage(
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const = 0;

    virtual csm::ImageCoordCovar groundToImage(
            const csm::EcefCoordCovar& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* ) const = 0;

    virtual csm::EcefCoord imageToGround(
            const csm::ImageCoord& imagePt,
            double height,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const = 0;

    virtual csm::EcefCoordCovar imageToGround(
            const csm::ImageCoordCovar& imagePt,
            double height,
            double heightVariance,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const = 0;

    virtual double getImageTime(const csm::ImageCoord& imagePt) const = 0;

    virtual
    csm::EcefCoord getSensorPosition(const csm::ImageCoord& imagePt) const = 0;

    virtual csm::EcefCoord getSensorPosition(double time) const = 0;

    virtual csm::EcefVector
    getSensorVelocity(const csm::ImageCoord& imagePt) const = 0;

    virtual csm::EcefVector getSensorVelocity(double time) const = 0;

protected:
    virtual types::RowCol<double> getSampleSpacing() const = 0;

    /**
     * Transforms the given l, s values from units of pixels from upper left
     * to meters with the origin at the center of the image.
     *
     * \param[in] l     Line position in terms of pixels from upper left
     * \param[in] s     Sample position in terms of pixels from upper left
     * \return A types::RowCol<double> containing the distance in meters from
     *     the center of the image
     */
    virtual
    types::RowCol<double> fromPixel(const csm::ImageCoord& pos) const = 0;

    /**
     * Transforms the given l, s values from units of meters with the origin
     * at the center of the image to pixels from upper left.
     *
     * \param[in] l     Line position in terms of meters from the image center
     * \param[in] s     Sample position in terms of meters from the image center
     * \return A types::RowCol<double> containing the distance in pixels from
     *     the upper left of the image
     */
    virtual
    types::RowCol<double> toPixel(const types::RowCol<double>& pos) const = 0;
};
}
}

#endif
