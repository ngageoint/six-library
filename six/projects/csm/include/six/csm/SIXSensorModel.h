/* =========================================================================
 * This file is part of the CSM SIX Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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

#ifndef __SIX_CSM_SIX_SENSOR_MODEL_H__
#define __SIX_CSM_SIX_SENSOR_MODEL_H__

#include <memory>

#include "RasterGM.h"
#include "CorrelationModel.h"
#include "NitfIsd.h"

#include <scene/SceneGeometry.h>
#include <scene/ProjectionModel.h>
#include <scene/ECEFToLLATransform.h>
#include <six/Enums.h>
#include <six/Types.h>

namespace six
{
namespace CSM
{
class SIXSensorModel : public csm::RasterGM
{
public:
    static const char FAMILY[];

    SIXSensorModel();

public: // Model methods
    /*
     * Returns a description of the trajectory identifier
     *
     * \return UNKNOWN
     */
    virtual std::string getTrajectoryIdentifier() const;

    /*
     * Returns a description of the sensor type
     *
     * \return CSM_SENSOR_TYPE_SAR
     */
    virtual std::string getSensorType() const;

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

    /**
     * Returns a UTC (Universal Time Coordinated) date and time near the time
     * of the trajectory for the associated image.
     *
     * \return String containing a representation of the date and time in ISO
     * 8601 format
     */
    virtual std::string getReferenceDateAndTime() const;

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
     * Parameters are not shareable, so returns false
     *
     * \return Always false
     */
    virtual bool hasShareableParameters() const;

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
     /**
     * Returns the number of geometric correction switches
     *
     * \return Always returns 0
     */
    virtual int getNumGeometricCorrectionSwitches() const;

    /**
     * Always throws since there are no geometric correction switches
     */
    virtual std::string getGeometricCorrectionName(int index) const;

    /**
     * Always throws since there are no geometric correction switches
     */
    virtual void setGeometricCorrectionSwitch(int index,
            bool value,
            csm::param::Type pType);

    /**
     * Always throws since there are no geometric correction switches
     */
    virtual bool getGeometricCorrectionSwitch(int index) const;

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

    /**
     * Returns the minimum and maximum image coordinates in full image space
     * pixels over which the current model is valid
     *
     * \return +/- 99,999 pixels (The CSM manual states to use this value if
     * there is no limit to the range in a direction.  This allows the model
     * to be used beyond the image boundaries.)
     */
    virtual
    std::pair< csm::ImageCoord, csm::ImageCoord> getValidImageRange() const;

    /*
     * Returns the minimum and maximum heights (in meters relative to WGS-84
     * ellipsoid) over which the model is valid.
     *
     * \return +/- 99,999 pixels (The CSM manual states to use this value if
     * there is no limit)
     */
    virtual std::pair<double,double> getValidHeightRange() const;

    /*
     * Returns the position and direction of the imaging locus nearest the
     * given ground point
     *
     * \param[in] imagePt Image position in pixels
     * \param[in] groundPt Ground coordinate in ECEF meters
     * \param[in] desiredPrecision Requested precision in pixels of the
     *     calculation. Currently this parameter is ignored.
     * \param[out] achievedPrecision  Precision in pixels to which the
     *     calculation is achieved (currently this is just set to
     *     desiredPrecision if non-NULL).
     * \param[out] warnings Unused
     */
    virtual csm::EcefLocus imageToProximateImagingLocus(
            const csm::ImageCoord& imagePt,
            const csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            csm::WarningList* warnings) const;

    /*
     * Returns a reference to a NoCorrelationModel
     */
    virtual const csm::CorrelationModel& getCorrelationModel() const;

    /*
     * Returns the 2x2 line and sample cross covariance (in pixels squared)
     * between the given points for any model error not accounted for by the
     * model parameters
     *
     * \return A four element vector of 0's
     */
    virtual std::vector<double> getUnmodeledCrossCovariance(
            const csm::ImageCoord& pt1,
            const csm::ImageCoord& pt2) const;

    virtual std::vector<double> getCrossCovarianceMatrix(
            const GeometricModel& comparisonModel,
            csm::param::Set pSet = csm::param::VALID,
            const GeometricModelList& otherModels = GeometricModelList()) const;

    static
    size_t getNumSensorModelParameters()
    {
        return static_cast<size_t>(scene::AdjustableParams::NUM_PARAMS);
    }

    static
    size_t getNumCorrelationParameterGroups()
    {
        return static_cast<size_t>(scene::AdjustableParams::NUM_CORR_GROUPS);
    }

    static
    size_t getCorrelationParameterGroup(size_t smParamIndex);

    double getCorrelationCoefficient(size_t cpGroupIndex,
                                     double deltaTime) const;

public:
    // All remaining public methods throw csm::Error's that they're not
    // implemented

    // These are pure virtual from GeometricModel
    virtual void setReferencePoint(const csm::EcefCoord& groundPt);

    // These are pure virtual from RasterGM
    csm::EcefLocus imageToRemoteImagingLocus(
            const csm::ImageCoord& ,
            double ,
            double* ,
            csm::WarningList* ) const;

protected:
    virtual six::DateTime getReferenceDateAndTimeImpl() const = 0;

    /**
     * Transforms the given l, s values from units of meters with the origin
     * at the center of the image to pixels from upper left.
     *
     * \param[in] pos Line position in terms of meters from the image center
     * \return A types::RowCol<double> containing the distance in pixels from
     * the upper left of the full image
     */
    virtual
    types::RowCol<double> toPixel(const types::RowCol<double>& pos) const = 0;

    /**
     * Transforms the given l, s values from units of pixels from upper left
     * to meters with the origin at the center of the image.
     *
     * \param[in] pos Line and sample positions in terms of pixels from upper
     *                left of the full image
     * \return A types::RowCol<double> containing the distance in meters from
     * the center of the image
     */
    virtual
    types::RowCol<double> fromPixel(const csm::ImageCoord& pos) const = 0;

    virtual
    void replaceModelStateImpl(const std::string& sensorModelState) = 0;

    virtual types::RowCol<double> getSampleSpacing() const = 0;

    virtual void setSchemaDir(const std::string& schemaDir);

    virtual csm::ImageCoord groundToImageImpl(const csm::EcefCoord& groundPt,
                                      double desiredPrecision,
                                      double* achievedPrecision) const;

    static
    scene::Vector3 toVector3(const csm::EcefCoord& pt)
    {
        scene::Vector3 vec;
        vec[0] = pt.x;
        vec[1] = pt.y;
        vec[2] = pt.z;
        return vec;
    }

    static
    csm::EcefVector toEcefVector(const scene::Vector3& vec)
    {
        return csm::EcefVector(vec[0], vec[1], vec[2]);
    }

    static
    csm::EcefCoord toEcefCoord(const scene::Vector3& vec)
    {
        return csm::EcefCoord(vec[0], vec[1], vec[2]);
    }

    static
    csm::ImageCoord toImageCoord(const types::RowCol<double>& pt)
    {
        return csm::ImageCoord(pt.row, pt.col);
    }

    static
    DataType getDataType(const csm::Des& des);

protected:
    const scene::ECEFToLLATransform mECEFToLLA;
    const csm::NoCorrelationModel mCorrelationModel;
    std::vector<std::string> mSchemaDirs;
    std::string mSensorModelState;
    std::auto_ptr<const scene::SceneGeometry> mGeometry;
    std::auto_ptr<scene::ProjectionModel> mProjection;
    csm::param::Type mAdjustableTypes[scene::AdjustableParams::NUM_PARAMS];

    // NOTE: This is computed just at the SCP once each time a new SICD is
    //       loaded rather than being recomputed for each ground point (which
    //       the scene module does support).  This is due to avoiding a
    //       mismatch between what the imageToGround() / groundToImage()
    //       overloadings that compute covariance use (where you do have a
    //       point)  and getParameterCovariance() / setParameterCovariance()
    //       where you do not.  Given the current CSM API, the only other
    //       solution would be to increase the number of adjustable parameters
    //       and the covariance matrix size to 9 and not fold in the
    //       tropo/iono error covariance into the ephemeric error covariance.
    math::linear::MatrixMxN<7, 7> mSensorCovariance;
};
}
}

#endif
