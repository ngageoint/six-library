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
};
}
}

#endif
