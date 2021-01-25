/* =========================================================================
 * This file is part of the CSM SICD Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __SIX_CSM_SICD_SENSOR_MODEL_H__
#define __SIX_CSM_SICD_SENSOR_MODEL_H__

#include <memory>
#include <vector>

#include "NitfIsd.h"

#include <six/csm/SIXSensorModel.h>
#include <scene/SceneGeometry.h>
#include <scene/ProjectionModel.h>
#include <six/sicd/ComplexData.h>

namespace six
{
namespace CSM
{
/**
 * @class SICDSensorModel
 *
 * @brief This class implements the CSM 3.0 SensorModel API for SICD data
 *
 */
class SICDSensorModel : public SIXSensorModel
{
public:
    static const csm::Version VERSION;
    static const char NAME[];

    /**
     * Create sensor model given an ISD.  Only NITF2.1 and FILENAME formats are
     * supported.
     *
     * \param isd        The image support data stored in an ISD struct
     * \param dataDir  The plugin's data directory.  If this is an empty
     *     string, the SIX_SCHEMA_PATH environment variable must be set.
     */
    SICDSensorModel(const csm::Isd& isd, const std::string& dataDir);

    /**
     * Create sensor model given sensor model state string representation.
     *
     * \param sensorModelState  The sensor model state represented as a string
     * \param dataDir           The plugin's data directory.  If this is an
     *     empty string, the SIX_SCHEMA_PATH environment variable must be set.
     */
    SICDSensorModel(const std::string& sensorModelState,
                    const std::string& dataDir);

    static
    bool containsComplexDES(const csm::Nitf21Isd& isd);

public: // Model methods
    /*
     * Returns the version of the sensor model
     *
     * \return The version of the sensor model
     */
    virtual csm::Version getVersion() const;

    /**
     * Returns a string indicating the name of the sensor model.
     *
     * \return Name of the sensor model
     */
    virtual std::string getModelName() const;

    /**
     * Returns a character string that identifies the sensor, the model type,
     * its mode of acquisition and processing path
     *
     * \return String that contains the pedigree information
     */
    virtual std::string getPedigree() const;

    /**
     * Returns imageId, a unique identifier associated with the given
     * sensor model
     *
     * \return String that contains the image identifier
     */
    virtual std::string getImageIdentifier() const;

    /**
     * Sets a unique identifier for the image to which the sensor model pertains
     *
     * \param[in]  imageId   String that contains the image identifier
     * \param[out] warnings  Unused
     */
    virtual void setImageIdentifier(const std::string& imageId,
                                    csm::WarningList* warnings);

    /**
     * Returns which sensor was used to acquire the image.  This is meant to
     * uniquely identify the sensor used to make the image.
     *
     * \return String that contains the sensor identifier
     */
    virtual std::string getSensorIdentifier() const;

    /**
     * Returns which platform was used to acquire the image.  This is meant to
     * uniquely identify the platform used to collect the image.
     *
     * \return String that contains the platform identifier
     */
    virtual std::string getPlatformIdentifier() const;

    /**
     * Returns an identifier that uniquely identifies a collection activity by a
     * sensor platform.
     *
     * \return String that contains the collection identifier
     */
    virtual std::string getCollectionIdentifier() const;

    /*
     * Returns a description of the sensor mode
     *
     * \return A description of the sensor mode
     */
    virtual std::string getSensorMode() const;

public: // GeometricModel methods


public: // RasterGM methods
    /**
    * Returns the starting coordinate for the imaging operation.
    * In case you have a chipped image, this will return the pixel
    * offset to the chip.
    * \return starting coordinate for the imaging operation
    */
   virtual csm::ImageCoord getImageStart() const;

    /**
     * Returns the number of lines and samples in full image space pixels for
     * the imaging operation.
     *
     * \return The size of the entire SICD
     */
    virtual csm::ImageVector getImageSize() const;

protected:
    virtual six::DateTime getReferenceDateAndTimeImpl() const;

private:
    virtual types::RowCol<double> toPixel(const types::RowCol<double>& pos) const;

    virtual types::RowCol<double> fromPixel(const csm::ImageCoord& pos) const;

    void replaceModelStateImpl(const std::string& sensorModelState);

    void initializeFromFile(const std::string& pathname);

    void initializeFromISD(const csm::Nitf21Isd& isd);

    virtual types::RowCol<double> getSampleSpacing() const;

    void reinitialize();

private:
    std::unique_ptr<six::sicd::ComplexData> mData;
};
}
}

#endif
