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
namespace csm
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
    static const ::csm::Version VERSION;
    static const char NAME[];

    /**
     * Create sensor model given an ISD.  Only NITF2.1 and FILENAME formats are
     * supported.
     *
     * \param isd        The image support data stored in an ISD struct
     * \param dataDir  The plugin's data directory.  If this is an empty
     *     string, the SIX_SCHEMA_PATH environment variable must be set.
     */
    SICDSensorModel(const ::csm::Isd& isd, const std::string& dataDir);

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
    bool containsComplexDES(const ::csm::Nitf21Isd& isd);

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

    virtual ::csm::ImageCoordCovar groundToImage(const ::csm::EcefCoordCovar& groundPt,
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

    virtual ::csm::EcefCoordCovar imageToGround(const ::csm::ImageCoordCovar& imagePt,
    		double height,
    		double heightVariance,
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

    // Error prop additions
    int getNumParameters() const;

    // Return name of AdjustableParameter[index]
    virtual std::string getParameterName(int index) const;

    // Return units of AdjustableParameter[index]
    virtual std::string getParameterUnits(int index) const;

    // Not sure, leave out for now
    //virtual bool hasShareableParameters() const;

    // Not sure
    //virtual bool isParameterShareable(int index) const;

    // Not sure
    //virtual ::csm::SharingCriteria getParameterSharingCriteria(int index) const;

    virtual double getParameterValue(int index) const;

    virtual void setParameterValue(int index, double value);

    virtual ::csm::param::Type getParameterType(int index) const;

    virtual void setParameterType(int index, ::csm::param::Type pType);

    virtual double getParameterCovariance(int index1,
                                           int index2) const;

    virtual void setParameterCovariance(int index1,
                                         int index2,
                                         double covariance);

    // return zero
    virtual int getNumGeometricCorrectionSwitches() const;

    //virtual std::string getGeometricCorrectionName(int index) const;

    //virtual void setGeometricCorrectionSwitch(int index,
    //                                           bool value,
    //                                           ::csm::param::Type pType);

    //virtual bool getGeometricCorrectionSwitch(int index) const;

     /**
      * Returns the minimum and maximum image coordinates in full image space
      * pixels over which the current model is valid
      *
      * \return +/- 99,999 pixels (The CSM manual states to use this value if
      * there is no limit to the range in a direction.  This allows the model
      * to be used beyond the image boundaries.)
      */
     //virtual
     //std::pair< ::csm::ImageCoord, ::csm::ImageCoord> getValidImageRange() const;

     /*
      * Returns the minimum and maximum heights (in meters relative to WGS-84
      * ellipsoid) over which the model is valid.
      *
      * \return +/- 99,999 pixels (The CSM manual states to use this value if
      * there is no limit)
      */
    //virtual std::pair<double,double> getValidHeightRange() const;

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
    computeGroundPartials(const ::csm::EcefCoord& groundPt) const;

    virtual std::vector<double> getUnmodeledError(const ::csm::ImageCoord& imagePt) const;

     // These are pure virtual from GeometricModel
     //virtual void setReferencePoint(const ::csm::EcefCoord& groundPt);

     //virtual std::vector<double> getCrossCovarianceMatrix(
     //       const ::csm::GeometricModel& comparisonModel,
     //       ::csm::param::Set pSet,
     //       const ::csm::GeometricModel::GeometricModelList& otherModels) const;

     virtual ::csm::RasterGM::SensorPartials computeSensorPartials(
             int index,
             const ::csm::EcefCoord& groundPt,
             double desiredPrecision,
             double* achievedPrecision,
             ::csm::WarningList* warnings) const;

     virtual ::csm::RasterGM::SensorPartials computeSensorPartials(
             int index,
             const ::csm::ImageCoord& imagePt,
             const ::csm::EcefCoord& groundPt,
             double desiredPrecision,
             double* achievedPrecision,
             ::csm::WarningList* warnings) const;

     virtual std::vector<SensorPartials> computeAllSensorPartials(
                     const ::csm::EcefCoord& groundPt,
                     ::csm::param::Set pSet           = ::csm::param::VALID,
                     double desiredPrecision   = 0.001,
                     double* achievedPrecision = NULL,
                     ::csm::WarningList* warnings     = NULL) const;

     virtual std::vector<SensorPartials> computeAllSensorPartials(
                     const ::csm::ImageCoord& imagePt,
                     const ::csm::EcefCoord& groundPt,
                     ::csm::param::Set pSet           = ::csm::param::VALID,
                     double desiredPrecision   = 0.001,
                     double* achievedPrecision = NULL,
                     ::csm::WarningList* warnings     = NULL) const;


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
    types::RowCol<double> fromPixel(const ::csm::ImageCoord& pos) const;

    void replaceModelStateImpl(const std::string& sensorModelState);

    void initializeFromFile(const std::string& pathname);

    void initializeFromISD(const ::csm::Nitf21Isd& isd);

    void setSchemaDir(const std::string& schemaDir);

private:
    std::vector<std::string> mSchemaDirs;
    std::string mSensorModelState;
    std::auto_ptr<six::sicd::ComplexData> mData;
    std::auto_ptr<const scene::SceneGeometry> mGeometry;
    std::auto_ptr<scene::ProjectionModel> mProjection;
    ::csm::param::Type AdjustableTypes[scene::AdjustableParam::size];
};
}
}

#endif
