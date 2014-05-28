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
     * Returns the number of adjustable parameters
     *
     * \return The number of adjustable parameters
     */
    virtual int getNumParameters() const;

    /**
     * Returns the name for the specified adjustable parameter
     *
     * \param index The parameter index
     *
     * \return The associated parameter name
     */
    virtual std::string getParameterName(int index) const;

    /**
     * Returns the units for the specified adjustable parameter
     *
     * \param index The parameter index
     *
     * \return The units for the associated parameter
     */
    virtual std::string getParameterUnits(int index) const;

    /**
     * Parameters are not shareable, so returns false for indices in range and
     * throws otherwise
     *
     * \param index The parameter index
     *
     * \return Always false
     */
    virtual bool isParameterShareable(int index) const;

    /**
     * Parameters are not shareable, so returns a default-constructed
     * SharingCriteria for indices in range (per MSP's suggestion) and
     * throws otherwise
     *
     * \param index The parameter index
     *
     * \return Default-constructed SharingCriteria
     */
    virtual csm::SharingCriteria getParameterSharingCriteria(int index) const;

    /**
     * Returns the value for the specified adjustable parameter
     *
     * \param index The parameter index
     *
     * \return The associated parameter value
     */
    virtual double getParameterValue(int index) const;

    /**
     * Sets the specified adjustable parameter value
     *
     * \param index The parameter index
     * \param value The value to set the parameter to
     */
    virtual void setParameterValue(int index, double value);

    /**
     * Gets the specified adjustable parameter type
     *
     * \param index The parameter index
     *
     * \return The associated parameter type
     */
    virtual csm::param::Type getParameterType(int index) const;

    /**
     * Sets the specified adjustable parameter type
     *
     * \param index The parameter index
     * \param pType The type to set the parameter to
     */
    virtual void setParameterType(int index, csm::param::Type pType);

    /**
     * Provides the covariance between the specified parameters.  To get
     * variance of a single parameter, set index1 and index2 to the same
     * value.
     *
     * \param index1 The first parameter index
     * \param index2 The second parameter index
     *
     * \return The parameter covariance
     */
    virtual double getParameterCovariance(int index1,
                                          int index2) const;

    /**
     * Set the covariance between the specified parameters.  To set
     * variance of a single parameter, set index1 and index2 to the same
     * value.
     *
     * \param index1 The first parameter index
     * \param index2 The second parameter index
     * \param covariance The covariance to set
     */
    virtual void setParameterCovariance(int index1,
                                        int index2,
                                        double covariance);

    /*
     * Returns the partial derivatives of line and sample (in pixels per
     * meter) with respect to the given ground pt
     *
     * \param[in] groundPt Ground coordinate in ECEF meters
     *
     * \return A vector with six elements as follows:
     *   -  [0] = line wrt x
     *   -  [1] = line wrt y
     *   -  [2] = line wrt z
     *   -  [3] = sample wrt x
     *   -  [4] = sample wrt y
     *   -  [5] = sample wrt z
     */
   virtual std::vector<double>
   computeGroundPartials(const csm::EcefCoord& groundPt) const;

   /**
    * This method returns the 2x2 line and sample covariance (in pixels
    * squared) at the given imagePt for any model error not accounted for
    * by the model parameters.
    *
    * \param imagePt Currently ignored
    *
    * \return A vector of four elements:
    * [0] = line variance
    * [1] = line/sample covariance
    * [2] = sample/line covariance
    * [3] = sample variance
    */
   virtual std::vector<double>
   getUnmodeledError(const csm::ImageCoord& imagePt) const;

   /**
    * This method returns the partial derivatives of line and sample
    * in pixels per the applicable model parameter units), respectively,
    * with respect to the model parameter given by index at the given
    * groundPt (x,y,z in ECEF meters).
    *
    * \param index The desired index
    * \param groundPt Ground point
    * \param desiredPrecision The desired precision.  Unused.
    * \param achievedPrecision The achieved precision.  Always set to
    * desired precision if supplied.
    * \param warnings Warnings.
    */
   virtual csm::RasterGM::SensorPartials computeSensorPartials(
           int index,
           const csm::EcefCoord& groundPt,
           double desiredPrecision = 0.001,
           double* achievedPrecision = NULL,
           csm::WarningList* warnings = NULL) const;

   /**
    * This method returns the partial derivatives of line and sample
    * (in pixels per the applicable model parameter units), respectively,
    * with respect to the model parameter given by index at the given
    * groundPt (x,y,z in ECEF meters).
    *
    * \param index The desired index
    * \param imagePt The image point corresponding to groundPt.  Results are
    * unpredictable if it does not correspond to calling groundToImage() with
    * groundPt
    * \param groundPt Ground point
    * \param desiredPrecision The desired precision.  Unused.
    * \param achievedPrecision The achieved precision.  Always set to
    * desired precision if supplied.
    * \param warnings Warnings.
    */
   virtual csm::RasterGM::SensorPartials computeSensorPartials(
           int index,
           const csm::ImageCoord& imagePt,
           const csm::EcefCoord& groundPt,
           double desiredPrecision = 0.001,
           double* achievedPrecision = NULL,
           csm::WarningList* warnings = NULL) const;

   /**
    * This method returns the partial derivatives of line and sample
    * (in pixels per the applicable model parameter units), respectively,
    * with respect to to each of the desired model parameters at the given
    * groundPt (x,y,z in ECEF meters).  Desired model parameters are
    * indicated by the given pSet.
    *
    * \param groundPt Ground point
    * \param pSet Desired model parameters
    * \param desiredPrecision The desired precision.  Unused.
    * \param achievedPrecision The achieved precision.  Always set to
    * desired precision if supplied.
    * \param warnings Warnings.
    *
    * \return Sensor partials
    */
   virtual std::vector<SensorPartials> computeAllSensorPartials(
                   const csm::EcefCoord& groundPt,
                   csm::param::Set pSet = csm::param::VALID,
                   double desiredPrecision = 0.001,
                   double* achievedPrecision = NULL,
                   csm::WarningList* warnings = NULL) const;

   /**
    * This method returns the partial derivatives of line and sample
    * (in pixels per the applicable model parameter units), respectively,
    * with respect to to each of the desired model parameters at the given
    * groundPt (x,y,z in ECEF meters).  Desired model parameters are
    * indicated by the given pSet.
    *
    * \param imagePt The image point corresponding to groundPt.  Results are
    * unpredictable if it does not correspond to calling groundToImage() with
    * groundPt
    * \param groundPt Ground point
    * \param pSet Desired model parameters
    * \param desiredPrecision The desired precision.  Unused.
    * \param achievedPrecision The achieved precision.  Always set to
    * desired precision if supplied.
    * \param warnings Warnings.
    *
    * \return Sensor partials
    */
   virtual std::vector<SensorPartials> computeAllSensorPartials(
                   const csm::ImageCoord& imagePt,
                   const csm::EcefCoord& groundPt,
                   csm::param::Set pSet = csm::param::VALID,
                   double desiredPrecision = 0.001,
                   double* achievedPrecision = NULL,
                   csm::WarningList* warnings = NULL) const;

    /**
     * Returns coordinates in meters to indicate the general location of the
     * image.
     *
     * \return Ground coordinate in meters
     */
    virtual csm::EcefCoord getReferencePoint() const;

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
    virtual csm::ImageCoord groundToImage(const csm::EcefCoord& groundPt,
                                            double desiredPrecision,
                                            double* achievedPrecision,
                                            csm::WarningList* warnings) const;

    /**
     * Converts groundPt in ground space (ECEF) and corresponding covariance
     * to returned ImageCoordCovar with covariance in image space.
     *
     * \param[in] groundPt Ground coordinate in ECEF meters and corresponding
     *     3x3 covariance in ECEF meters squared
     * \param[in] desiredPrecision Requested precision in pixels of the
     *     calculation. Currently this parameter is ignored.
     * \param[out] achievedPrecision  Precision in pixels to which the
     *     calculation is achieved (currently this is just set to
     *     desiredPrecision if non-NULL).
     * \param[out] warnings Unused
     *
     * \return Image coordinate in pixels and corresponding 2x2 covariance in
     * pixels squared
     */
    virtual csm::ImageCoordCovar groundToImage(
            const csm::EcefCoordCovar& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

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
    virtual csm::EcefCoord imageToGround(const csm::ImageCoord& imagePt,
                                           double height,
                                           double desiredPrecision,
                                           double* achievedPrecision,
                                           csm::WarningList* warnings) const;

    /**
     * Converts imagePt (pixels in image space and corresponding 2x2
     * covariance in pixels squared) to a ground coordinate with covariance.
     *
     * \param[in] imagePt Image line and sample in pixels and covariance in
     *     pixels squared
     * \param[in] height Height in meters measured with respect to the WGS-84
     *     ellipsoid.
     * \param[in] heightVariance Height variance in meters.  This is currently
     *     ignored - any error in the height should be translated to
     *     ephemeris error
     * \param[in] desiredPrecision Requested precision in pixels of the
     *     calculation. Currently this parameter is ignored.
     * \param[out] achievedPrecision  Precision in pixels to which the
     *     calculation is achieved (currently this is just set to
     *     desiredPrecision if non-NULL).
     * \param[out] warnings Unused
     *
     * \return Ground coordinate with covariance (x, y, z in ECEF meters
     * and corresponding 3x3 covariance in ECEF meters squared)
     */
    virtual csm::EcefCoordCovar imageToGround(
            const csm::ImageCoordCovar& imagePt,
            double height,
            double heightVariance,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    /**
     * Returns the starting coordinate for the imaging operation.
     *
     * \return Always returns (0, 0)
     */
    virtual csm::ImageCoord getImageStart() const;

    /**
     * Returns the number of lines and samples in full image space pixels for
     * the imaging operation.
     *
     * \return The size of the entire SICD
     */
    virtual csm::ImageVector getImageSize() const;

    /**
     * Calculates the direction of illumination at the given ground position
     * groundPt.  The returned values define a direction vector that points
     * from the illumination source to the given ground point.
     *
     * \param[in] groundPt  Ground coordinate in meters
     *
     * \return Illumination direction vector
     */
    virtual csm::EcefVector
    getIlluminationDirection(const csm::EcefCoord& groundPt) const;

    /**
     * Computes the time in seconds at which the pixel specified by imagePt was
     * imaged. The time provided is relative to the reference date and time
     * given by getReferenceDateAndTime().
     *
     * \param[in] imagePt Image position in pixels
     *
     * \return Time in seconds from the reference date and time
     */
    virtual double getImageTime(const csm::ImageCoord& imagePt) const;

    /**
     * Returns the position of the physical sensor at the given position in
     * the image
     *
     * \param[in] imagePt Image position in pixels
     *
     * \return Sensor ECEF coordinate in meters
     */
    virtual
    csm::EcefCoord getSensorPosition(const csm::ImageCoord& imagePt) const;

    /**
     * Returns the position of the physical sensor at the given time
     *
     * \param[in] time Time pixel is imaged in seconds
     *
     * \return Sensor ECEF coordinate in meters
     */
    virtual csm::EcefCoord getSensorPosition(double time) const;

    /**
     * Returns the velocity of the physical sensor at the given position in
     * the image
     *
     * \param[in] imagePt Image position in pixels
     *
     * \return Sensor velocity in meters per second
     */
    virtual
    csm::EcefVector getSensorVelocity(const csm::ImageCoord& imagePt) const;

    /**
     * Returns the velocity of the physical sensor at the given time
     *
     * \param[in] time Time pixel is imaged in seconds
     *
     * \return Sensor velocity in meters per second
     */
    virtual csm::EcefVector getSensorVelocity(double time) const;

private:
    /**
     * Transforms the given l, s values from units of meters with the origin
     * at the center of the image to pixels from upper left.
     *
     * \param[in] l     Line position in terms of meters from the image center
     * \param[in] s     Sample position in terms of meters from the image center
     * \return A types::RowCol<double> containing the distance in pixels from the upper left of the image
     */
    types::RowCol<double> toPixel(const types::RowCol<double>& pos) const;

    /**
     * Transforms the given l, s values from units of pixels from upper left
     * to meters with the origin at the center of the image.
     *
     * \param[in] l     Line position in terms of pixels from upper left
     * \param[in] s     Sample position in terms of pixels from upper left
     * \return A types::RowCol<double> containing the distance in meters from the center of the image
     */
    types::RowCol<double> fromPixel(const csm::ImageCoord& pos) const;

    void replaceModelStateImpl(const std::string& sensorModelState);

    void initializeFromFile(const std::string& pathname);

    void initializeFromISD(const csm::Nitf21Isd& isd);

    void setSchemaDir(const std::string& schemaDir);

private:
    std::vector<std::string> mSchemaDirs;
    std::string mSensorModelState;
    std::auto_ptr<six::sicd::ComplexData> mData;
    std::auto_ptr<const scene::SceneGeometry> mGeometry;
    std::auto_ptr<scene::ProjectionModel> mProjection;
    csm::param::Type mAdjustableTypes[scene::AdjustableParams::NUM_PARAMS];
};
}
}

#endif
