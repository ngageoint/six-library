/* =========================================================================
 * This file is part of the CSM SIX Plugin
 * =========================================================================
 *
 * (C) Copyright 2025, Arka Group, L.P.
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

#ifndef __SIX_CSM_SIX_SENSOR_MODEL_STATE_H__
#define __SIX_CSM_SIX_SENSOR_MODEL_STATE_H__

#include <six/Enums.h>
#include <six/Types.h>

#include "csm.h"

namespace six
{
namespace CSM
{
class SIXSensorModelState
{
public:
    /**
     * Construct an empty sensor state object
     */
    SIXSensorModelState();

    /**
     * Parse the given state string into a model state
     *
     * \param[in] stateString The sensor model state to update to.
     * \param[out] remainder All text in the stateString after the portion
     *     handled by this class.
     */
    SIXSensorModelState(const std::string& stateString, std::string& remainder);

    /**
     * Converts internal model state to a printable string for inclusion in a
     * complete sensor model state string
     *
     * \return Model state string as printable text
     */
    std::string toString() const;

    /* Adjustable parameter type */
    csm::param::Type getAdjustableType(int index) const
    {
        return mAdjustableTypes[index];
    }

    void setAdjustableType(int index, csm::param::Type type)
    {
        mAdjustableTypes[index] = type;
    }

    /* Adjustable type value */
    double getAdjustableValue(int index) const
    {
        return mAdjustableValues[index];
    }

    void setAdjustableValue(int index, double value)
    {
        mAdjustableValues[index] = value;
    }

    /* Dataset name (image identifier) */
    std::string getDatasetName() const
    {
        return mDatasetName;
    }

    void setDatasetName(const std::string& datasetName)
    {
        mDatasetName = datasetName;
    }

    /* Use overridden sensor covariance matrix */
    bool getOverrideSensorCovariance() const
    {
        return mOverrideSensorCovariance;
    }
    void setOverrideSensorCovariance(bool override)
    {
        mOverrideSensorCovariance = override;
    }

    /* Sensor covariance matrix */
    double getSensorCovariance(int idx1, int idx2) const
    {
        return mSensorCovariance(idx1, idx2);
    }

    void setSensorCovariance(int idx1, int idx2, double cov)
    {
        mSensorCovariance(idx1, idx2) = cov;
    }

protected:
    std::string initializeFromString(const std::string& stateString);

    csm::param::Type mAdjustableTypes[scene::AdjustableParams::NUM_PARAMS];

    double mAdjustableValues[scene::AdjustableParams::NUM_PARAMS];

    std::string mDatasetName;

    bool mOverrideSensorCovariance;

    math::linear::MatrixMxN<7, 7> mSensorCovariance;
};
}
}
#endif
