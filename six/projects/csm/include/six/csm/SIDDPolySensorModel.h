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

#ifndef __SIX_CSM_SIDD_POLY_SENSOR_MODEL_H__
#define __SIX_CSM_SIDD_POLY_SENSOR_MODEL_H__

#include <scene/ProjectionModel.h>
#include <scene/SceneGeometry.h>
#include <six/csm/SIDDSensorModel.h>
#include <six/sidd/DerivedData.h>

#include <memory>
#include <vector>

#include "NitfIsd.h"

namespace six
{
namespace CSM
{
/**
 * @class SIDDPolySensorModel
 *
 * @brief This class implements the CSM 3.0 SensorModel API for SIDD data with
 * only PolynomialProjection metadata
 *
 */
class SIDDPolySensorModel : public SIDDSensorModel
{
public:
    static const csm::Version VERSION;
    static const char NAME[];

    /**
     * Create sensor model given an ISD.  Only NITF2.1 and FILENAME formats are
     * supported.
     *
     * \param isd      The image support data stored in an ISD struct
     * \param dataDir  The plugin's data directory.  If this is an empty
     *     string, the SIX_SCHEMA_PATH environment variable must be set.
     */
    SIDDPolySensorModel(const csm::Isd& isd, const std::string& dataDir);

    /**
     * Create sensor model given sensor model state string representation.
     *
     * \param sensorModelState  The sensor model state represented as a string
     * \param dataDir           The plugin's data directory.  If this is an
     *     empty string, the SIX_SCHEMA_PATH environment variable must be set.
     */
    SIDDPolySensorModel(const std::string& sensorModelState,
                        const std::string& dataDir);

public:  // Model methods
    /*
     * Returns the version of the sensor model
     *
     * \return The version of the sensor model
     */
    csm::Version getVersion() const override;

    /**
     * Returns a string indicating the name of the sensor model.
     *
     * \return Name of the sensor model
     */
    std::string getModelName() const override;

    /**
     * Returns a character string that identifies the sensor, the model type,
     * its mode of acquisition and processing path
     *
     * \return String that contains the pedigree information
     */
    std::string getPedigree() const override;

public:  // RasterGM methods
    double getImageTime(const csm::ImageCoord& imagePt) const override;

    csm::EcefCoord imageToGround(const csm::ImageCoord& imagePt,
                                 double height,
                                 double desiredPrecision,
                                 double* achievedPrecision,
                                 csm::WarningList* warnings) const override;

    csm::EcefCoordCovar imageToGround(
            const csm::ImageCoordCovar& imagePt,
            double height,
            double heightVariance,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const override;

    csm::ImageCoord groundToImage(const csm::EcefCoord& groundPt,
                                  double desiredPrecision,
                                  double* achievedPrecision,
                                  csm::WarningList* warnings) const override;

    csm::ImageCoordCovar groundToImage(
            const csm::EcefCoordCovar& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const override;

    std::vector<double> computeGroundPartials(
            const csm::EcefCoord& groundPt) const override;

    std::vector<double> getUnmodeledCrossCovariance(
            const csm::ImageCoord& pt1,
            const csm::ImageCoord& pt2) const override;

    std::pair<csm::ImageCoord, csm::ImageCoord> getValidImageRange()
            const override;

    std::pair<double, double> getValidHeightRange() const override;

private:
    void reinitialize(SIXSensorModelState& modelState) override;

private:
    six::sidd::PolynomialProjection* mPolyProj;

    const scene::LLAToECEFTransform mLLAToECEF;
};
}
}

#endif
