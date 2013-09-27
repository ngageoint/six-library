/* =========================================================================
 * This file is part of the CSM SICD Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2011, General Dynamics - Advanced Information Systems
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

#ifndef __SIX_CSM_SIDD_SENSOR_MODEL_H__
#define __SIX_CSM_SIDD_SENSOR_MODEL_H__

#include <memory>
#include <vector>

#include "NitfIsd.h"

#include <six/csm/SIXSensorModel.h>
#include <six/sidd/DerivedData.h>
#include <scene/GridGeometry.h>
#include <scene/GridECEFTransform.h>

namespace six
{
namespace csm
{
/**
 * @class SIDDSensorModel
 *
 * @brief This class implements the CSM 3.0 SensorModel API for SIDD data
 *
 */
class SIDDSensorModel : public SIXSensorModel
{
public:
    static const ::csm::Version VERSION;
    static const char NAME[];

    /**
     * Create sensor model given an ISD.  Only NITF2.1 and FILENAME formats are
     * supported.
     *
     * \param isd      The image support data stored in an ISD struct
     * \param dataDir  The plugin's data directory.  If this is an empty
     *     string, the SIX_SCHEMA_PATH environment variable must be set.
     */
    SIDDSensorModel(const ::csm::Isd& isd, const std::string& dataDir);

    /**
     * Create sensor model given sensor model state string representation.
     *
     * \param sensorModelState  The sensor model state represented as a string
     * \param dataDir           The plugin's data directory.  If this is an
     *     empty string, the SIX_SCHEMA_PATH environment variable must be set.
     */
    SIDDSensorModel(const std::string& sensorModelState,
                    const std::string& dataDir);

    static
    bool containsDerivedDES(const ::csm::Nitf21Isd& isd);

public: // Model methods
    /*
     * Returns the version of the sensor model
     *
     * \return The version of the sensor model
     */
    virtual ::csm::Version getVersion() const;

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
                                    ::csm::WarningList* warnings);

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

    /**
     * Returns a UTC (Universal Time Coordinated) date and time near the time of
     * the trajectory for the associated image.
     *
     * \return String containing a representation of the date and time in ISO
     * 8601 format
     */
    virtual std::string getReferenceDateAndTime() const;

    /**
     * Returns a string representing the state of the sensor model.  The state
     * string is made up of the sensor model name, followed by a space, then
     * the SICD XML as a string.
     *
     * \return State of the sensor model
     */
    virtual std::string getModelState() const;

    /**
     * Initialize the current model with argState
     *
     * \param[in] argState The sensor model state to update to.  If the string
     *     is empty, the model is unchanged.
     */
    virtual void replaceModelState(const std::string& argState);

public: // GeometricModel methods
    /**
     * Returns coordinates in meters to indicate the general location of the
     * image.
     *
     * \return Ground coordinate in meters
     */
    virtual ::csm::EcefCoord getReferencePoint() const;

public: // RasterGM methods
    /**
     * Converts groundPt in ground space (ECEF) to returned ImageCoord in image
     * space.
     *
     * \param[in] groundPt Ground coordinate in meters
     * \param[in] desiredPrecision Requested precision in pixels of the
     *     calculation. Currently this parameter is ignored.
     * \param[out] achievedPrecision  Precision in pixels to which the
     *     calculation is achieved (currently this is just set to
     *     desiredPrecision if non-NULL).
     * \param[out] warnings Unused
     *
     * \return Image coordinate in pixels
     */
    virtual ::csm::ImageCoord groundToImage(const ::csm::EcefCoord& groundPt,
                                            double desiredPrecision,
                                            double* achievedPrecision,
                                            ::csm::WarningList* warnings) const;

    /**
     * Converts imagePt (pixels) in image space returned EcefCoord (meters)
     * in ground space (ECEF).
     *
     * \param[in] imagePt Image line and sample in pixels
     * \param[in] height Height in meters measured with respect to the WGS-84
     *     ellipsoid.
     * \param[in] desiredPrecision Requested precision in pixels of the
     *     calculation. Currently this parameter is ignored.
     * \param[out] achievedPrecision  Precision in pixels to which the
     *     calculation is achieved (currently this is just set to
     *     desiredPrecision if non-NULL).
     * \param[out] warnings Unused
     *
     * \return Ground coordinate in meters
     */
    virtual ::csm::EcefCoord imageToGround(const ::csm::ImageCoord& imagePt,
                                           double height,
                                           double desiredPrecision,
                                           double* achievedPrecision,
                                           ::csm::WarningList* warnings) const;

    /**
     * Returns the starting coordinate for the imaging operation.
     *
     * \return Always returns (0, 0)
     */
    virtual ::csm::ImageCoord getImageStart() const;

    /**
     * Returns the number of lines and samples in full image space pixels for
     * the imaging operation.
     *
     * \return The size of the entire SICD
     */
    virtual ::csm::ImageVector getImageSize() const;

    /**
     * Calculates the direction of illumination at the given ground position
     * groundPt.  The returned values define a direction vector that points
     * from the illumination source to the given ground point.
     *
     * \param[in] groundPt  Ground coordinate in meters
     *
     * \return Illumination direction vector
     */
    virtual ::csm::EcefVector
    getIlluminationDirection(const ::csm::EcefCoord& groundPt) const;

    /**
     * Computes the time in seconds at which the pixel specified by imagePt was
     * imaged. The time provided is relative to the reference date and time
     * given by getReferenceDateAndTime().
     *
     * \param[in] imagePt Image position in pixels
     *
     * \return Time in seconds from the reference date and time
     */
    virtual double getImageTime(const ::csm::ImageCoord& imagePt) const;

    /**
     * Returns the position of the physical sensor at the given position in
     * the image
     *
     * \param[in] imagePt Image position in pixels
     *
     * \return Sensor ECEF coordinate in meters
     */
    virtual
    ::csm::EcefCoord getSensorPosition(const ::csm::ImageCoord& imagePt) const;

    /**
     * Returns the position of the physical sensor at the given time
     *
     * \param[in] time Time pixel is imaged in seconds
     *
     * \return Sensor ECEF coordinate in meters
     */
    virtual ::csm::EcefCoord getSensorPosition(double time) const;

    /**
     * Returns the velocity of the physical sensor at the given position in
     * the image
     *
     * \param[in] imagePt Image position in pixels
     *
     * \return Sensor velocity in meters per second
     */
    virtual
    ::csm::EcefVector getSensorVelocity(const ::csm::ImageCoord& imagePt) const;

    /**
     * Returns the velocity of the physical sensor at the given time
     *
     * \param[in] time Time pixel is imaged in seconds
     *
     * \return Sensor velocity in meters per second
     */
    virtual ::csm::EcefVector getSensorVelocity(double time) const;

private:
    /**
     * Transforms the given l, s values from units of pixels from upper left
     * to meters with the origin at the center of the image.
     *
     * \param[in] l     Line position in terms of pixels from upper left
     * \param[in] s     Sample position in terms of pixels from upper left
     * \return A types::RowCol<double> containing the distance in meters from the center of the image
     */
    types::RowCol<double> fromPixel(const ::csm::ImageCoord& pos) const;

    const six::sidd::MeasurableProjection* getProjection() const;

    void replaceModelStateImpl(const std::string& sensorModelState);

    void initializeFromFile(const std::string& pathname, size_t imageIndex);

    void initializeFromISD(const ::csm::Nitf21Isd& isd, size_t imageIndex);

    void setSchemaDir(const std::string& schemaDir);

    types::RowCol<double> ecefToRowCol(const scene::Vector3& ecef) const;

    scene::Vector3 rowColToECEF(const types::RowCol<double>& imagePt) const;

private:
    std::vector<std::string> mSchemaDirs;
    std::string mSensorModelState;
    std::auto_ptr<six::sidd::DerivedData> mData;
    std::auto_ptr<const scene::GridGeometry> mGridGeometry;
    std::auto_ptr<const scene::GridECEFTransform> mGridTransform;
};
}
}

#endif
