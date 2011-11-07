/* =========================================================================
 * This file is part of the CSM SIDD Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2011, General Dynamics - Advanced Information Systems
 *
 * The CSM SIDD Plugin is free software; you can redistribute it and/or modify
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

#ifndef __SIDDSENSORMODEL_H
#define __SIDDSENSORMODEL_H

/**
 * @class SIDDSensorModel
 *
 * @brief This class implements the TSMSensorModel API for SIDD data
 *
 * @author Martina Schultz
 */

#include "TSMSensorModel.h"
#include "TSMISDNITF21.h"
#include <import/six/sidd.h>
#include <import/scene.h>

namespace sidd_sensor
{

class SIDDSensorModel : public TSMSensorModel
{

public:
    /**
     * Create sensor model given a filename.
     *
     * \param filename      the filename
     * \param sensorModel   the name/id of this sensor model
     */
    SIDDSensorModel(const std::string& filename,
                    const std::string& sensorModel);

    /**
     * Create sensor model given NITF 2.1 support data.
     *
     * \param imageSupportData  the image support data stored in a TSM struct
     * \param sensorModel   the name/id of this sensor model
     */
    SIDDSensorModel(NITF_2_1_ISD* imageSupportData,
                    const std::string& sensorModel);

    /**
     * Create sensor model given sensor model state string representation.
     *
     * \param sensorModelState the sensor model state represented as a string
     */
    SIDDSensorModel(const std::string& sensorModelState);

    virtual ~SIDDSensorModel();

    /**
     * Converts x, y and z (meters) in ground space (ECEF) to
     * line and sample (pixels) in image space.
     *
     * \param[in] x        Ground X coordinate in meters
     * \param[in] y        Ground Y coordinate in meters
     * \param[in] z        Ground Z coordinate in meters
     * \param[out] line    Image line in pixels
     * \param[out] sample  Image sample in pixels
     * \param[out] achieved_precision  Precision in pixels to which the calculation is achieved (currently this is just set to desired_precision)
     * \param[in] desired_precision    Requested precision in pixels of the calculation
     */
    virtual TSMWarning
    *groundToImage(const double &x, const double &y, const double &z,
                   double& line, double& sample, double& achieved_precision,
                   const double &desired_precision = 0.001) throw (TSMError);

    /**
     * Converts x, y and z (meters) in ground space (ECEF) to
     * line and sample (pixels) in image space.
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] x        Ground X coordinate in meters
     * \param[in] y        Ground Y coordinate in meters
     * \param[in] z        Ground Z coordinate in meters
     * \param[in] groundCovariance     Array of doubles consisting of the 3x3 covariance of the passed in ground point
     * \param[out] line    Image line in pixels
     * \param[out] sample  Image sample in pixels
     * \param[out] imageCovariance     Array of doubles consisting of the 2x2 covariance of the resultant image point
     * \param[out] achieved_precision  Precision in pixels to which the calculation is achieved (currently this is just set to desired_precision)
     * \param[in] desired_precision    Requested precision in pixels of the calculation
     */
    virtual TSMWarning
    * groundToImage(const double& x, const double& y, const double& z,
                    const double groundCovariance[9], double& line,
                    double& sample, double imageCovariance[4],
                    double& achieved_precision,
                    const double& desired_precision = 0.001) throw (TSMError);

    /**
     * Converts line and sample (pixels) in image space
     * to x, y and z (meters) in ground space (ECEF).
     *
     * \param[in] line     Image line in pixels
     * \param[in] sample   Image sample in pixels
     * \param[in] height   Height in meters measured with respect to the WGS-84 ellipsoid
     * \param[out] x       Ground X coordinate in meters
     * \param[out] y       Ground Y coordinate in meters
     * \param[out] z       Ground Z coordinate in meters
     * \param[out] achieved_precision  Precision in pixels to which the calculation is achieved (currently this is just set to desired_precision)
     * \param[in] desired_precision    Requested precision in pixels of the calculation
     */
    virtual TSMWarning
    * imageToGround(const double& line, const double& sample,
                    const double& height, double& x, double& y, double& z,
                    double& achieved_precision,
                    const double& desired_precision = 0.001) throw (TSMError);

    /**
     * Converts line and sample (pixels) in image space
     * to x, y and z (meters) in ground space (ECEF).
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] line     Image line in pixels
     * \param[in] sample   Image sample in pixels
     * \param[in] imageCovariance      Array of doubles consisting of the 2x2 covariance of the passed in image point
     * \param[in] height   Height in meters measured with respect to the WGS-84 ellipsoid
     * \param[in] heightVariance       Double representing the variance of the passed in height measurement
     * \param[out] x       Ground X coordinate in meters
     * \param[out] y       Ground Y coordinate in meters
     * \param[out] z       Ground Z coordinate in meters
     * \param[out] groundCovariance    Array of doubles consisting of the 3x3 covariance of the resultant ground point
     * \param[out] achieved_precision  Precision in pixels to which the calculation is achieved (currently this is just set to desired_precision)
     * \param[in] desired_precision    Requested precision in pixels of the calculation
     */
    virtual TSMWarning
    * imageToGround(const double& line, const double& sample,
                    const double imageCovariance[4], const double& height,
                    const double& heightVariance, double& x, double& y,
                    double& z, double groundCovariance[9],
                    double& achieved_precision,
                    const double& desired_precision = 0.001) throw (TSMError);

    /**
     * Computes a proximate imaging locus, a vector approximation of the
     * imaging locus for the given line and sample nearest the given x, y and z.     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] line     Image line in pixels
     * \param[in] sample   Image sample in pixels
     * \param[in] x        Ground X coordinate in meters
     * \param[in] y        Ground Y coordinate in meters
     * \param[in] z        Ground Z coordinate in meters
     * \param[out] locus   Array of six doubles: a position and a direction vector
     * \param[out] achieved_precision  Precision in pixels to which the calculation is achieved (currently this is just set to desired_precision)
     * \param[in] desired_precision    Requested precision in pixels of the calculation
     */
    virtual TSMWarning
            * imageToProximateImagingLocus(const double& line,
                                           const double& sample,
                                           const double& x, const double& y,
                                           const double& z, double locus[6],
                                           double& achieved_precision,
                                           const double& desired_precision =
                                                   0.001) throw (TSMError);

    /**
     * Computes locus, a vector approximation of the
     * imaging locus for the given line and sample.
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] line     Image line in pixels
     * \param[in] sample   Image sample in pixels
     * \param[out] locus   Array of six doubles: a position and a direction vector
     * \param[out] achieved_precision  Precision in pixels to which the calculation is achieved (currently this is just set to desired_precision)
     * \param[in] desired_precision    Requested precision in pixels of the calculation
     */
    virtual TSMWarning
            * imageToRemoteImagingLocus(const double& line,
                                        const double& sample, double locus[6],
                                        double& achieved_precision,
                                        const double& desired_precision = 0.001)
                                                                                 throw (TSMError);

    /**
     * Calculates the partial derivatives of image position (both line and
     * sample) with respect to ground coordinates at the given ground
     * position x, y, z
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] x        Ground X coordinate in meters
     * \param[in] y        Ground Y coordinate in meters
     * \param[in] z        Ground Z coordinate in meters
     * \param[out] partials   Array of six doubles containing partial derivatives: line wrt x, y, and z followed by sample wrt x, y, and z
     */
    virtual TSMWarning
    * computeGroundPartials(const double& x, const double& y, const double& z,
                            double partials[6]) throw (TSMError);

    /**
     * Calculates the partial derivatives of image position (both line and
     * sample) with respect to the given sensor parameter (index) at the
     * given ground position
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index    Selects the sensor parameter
     * \param[in] x        Ground X coordinate in meters
     * \param[in] y        Ground Y coordinate in meters
     * \param[in] z        Ground Z coordinate in meters
     * \param[out] line_partial        Partial of line with respect to the indexed sensor parameter
     * \param[out] sample_partial      Partial of sample with respect to the indexed sensor parameter
     * \param[out] achieved_precision  Precision in pixels to which the calculation is achieved (currently this is just set to desired_precision)
     * \param[in] desired_precision    Requested precision in pixels of the calculation
     */
    virtual TSMWarning
            * computeSensorPartials(const int& index, const double& x,
                                    const double& y, const double& z,
                                    double& line_partial,
                                    double& sample_partial,
                                    double& achieved_precision,
                                    const double& desired_precision = 0.001)
                                                                             throw (TSMError);

    /**
     * Calculates the partial derivatives of image position (both line and
     * sample) with respect to the given sensor parameter (index) at the
     * given ground position
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index    Selects the sensor parameter
     * \param[in] line     Image line in pixels
     * \param[in] sample   Image sample in pixels
     * \param[in] x        Ground X coordinate in meters
     * \param[in] y        Ground Y coordinate in meters
     * \param[in] z        Ground Z coordinate in meters
     * \param[out] line_partial        Partial of line with respect to the indexed sensor parameter
     * \param[out] sample_partial      Partial of sample with respect to the indexed sensor parameter
     * \param[out] achieved_precision  Precision in pixels to which the calculation is achieved (currently this is just set to desired_precision)
     * \param[in] desired_precision    Requested precision in pixels of the calculation
     */
    virtual TSMWarning * computeSensorPartials(const int& index,
                                               const double& line,
                                               const double& sample,
                                               const double& x,
                                               const double& y,
                                               const double& z,
                                               double& line_partial,
                                               double& sample_partial,
                                               double& achieved_precision,
                                               const double& desired_precision =
                                                       0.001) throw (TSMError);

    /**
     * Returns the covariance of the specified parameter pair (index1, index2).
     * The variance of the given parameter can be obtained by using the same
     * value for index1 and index2.
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index1   Selects the first of the parameter pair
     * \param[in] index2   Selects the second of the parameter pair
     * \param[out] return_val          Double returning the covariance value of the specified parameter pair
     */
    virtual TSMWarning
    *getCurrentParameterCovariance(const int& index1, const int& index2,
                                   double &return_val) throw (TSMError);

    /**
     * Sets the covariance of the specified parameter pair (index1, index2).
     * The variance of the given parameter can be set by using the same
     * value for index1 and index2.
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index1   Selects the first of the parameter pair
     * \param[in] index2   Selects the second of the parameter pair
     * \param[in] covariance           Double containing the covariance value of the specified parameter pair
     */
    virtual TSMWarning
    * setCurrentParameterCovariance(const int& index1, const int& index2,
                                    const double& covariance) throw (TSMError);

    /**
     * Sets the covariance of the specified parameter pair (index1, index2).
     * The variance of the given parameter can be set by using the same
     * value for index1 and index2.
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index1   Selects the first of the parameter pair
     * \param[in] index2   Selects the second of the parameter pair
     * \param[in] covariance           Double containing the covariance value of the specified parameter pair
     */
    virtual TSMWarning
    * setOriginalParameterCovariance(const int& index1, const int& index2,
                                     const double& covariance) throw (TSMError);

    /**
     * Returns the covariance of the specified parameter pair (index1, index2).
     * The variance of the given parameter can be obtained by using the same
     * value for index1 and index2.
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index1   Selects the first of the parameter pair
     * \param[in] index2   Selects the second of the parameter pair
     * \param[out] return_val          Double returning the covariance value of the specified parameter pair
     */
    virtual TSMWarning
    *getOriginalParameterCovariance(const int& index1, const int& index2,
                                    double &return_val) throw (TSMError);

    /**
     * Returns trajectoryId to indicate which trajectory was used to acquire
     * the image. This trajectoryId is unique for each sensor type on an
     * individual path.
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[out] trajectoryId        String containing the trajectory name
     */
    virtual TSMWarning
    * getTrajectoryIdentifier(std::string& trajectoryId) throw (TSMError);

    /**
     * Returns a UTC (Universal Time Coordinated) date and time near the time of the trajectory for the associated image.
     *
     * \param[out] date_and_time       String containing a representation of the date and time
     */
    virtual TSMWarning
    * getReferenceDateAndTime(std::string& date_and_time) throw (TSMError);

    /**
     * Computes the time in seconds at which the pixel specified by line and
     * sample was imaged. The time provided is relative to the reference date
     * and time given by getReferenceDateAndTime.
     *
     * \param[in] line     Image line in pixels
     * \param[in] sample   Image sample in pixels
     * \param[out] time    Time in seconds from the reference date and time
     */
    virtual TSMWarning * getImageTime(const double& line, const double& sample,
                                      double& time) throw (TSMError);

    /**
     * Returns the position of the physical sensor at the given position in
     * the image
     *
     * \param[in] line     Image line in pixels
     * \param[in] sample   Image sample in pixels
     * \param[out] x       Sensor X coordinate in meters
     * \param[out] y       Sensor Y coordinate in meters
     * \param[out] z       Sensor Z coordinate in meters
     */
    virtual TSMWarning
    * getSensorPosition(const double& line, const double& sample, double& x,
                        double& y, double& z) throw (TSMError);

    /**
     * Returns the position of the physical sensor at the given position in
     * the image
     *
     * \param[in] time     Time pixel is imaged in seconds
     * \param[out] x       Sensor X coordinate in meters
     * \param[out] y       Sensor Y coordinate in meters
     * \param[out] z       Sensor Z coordinate in meters
     */
    virtual TSMWarning
            * getSensorPosition(const double& time, double& x, double& y,
                                double& z) throw (TSMError);

    /**
     * Returns the position of the physical sensor at the given position in
     * the image
     *
     * \param[in] line     Image line in pixels
     * \param[in] sample   Image sample in pixels
     * \param[out] vx      Sensor X velocity coordinate in meters per second
     * \param[out] vy      Sensor Y velocity coordinate in meters per second
     * \param[out] vz      Sensor Z velocity coordinate in meters per second
     */
    virtual TSMWarning
    * getSensorVelocity(const double& line, const double& sample, double& vx,
                        double& vy, double& vz) throw (TSMError);

    /**
     * Returns the position of the physical sensor at the given position in
     * the image
     *
     * \param[in] time     Time pixel is imaged in seconds
     * \param[out] vx      Sensor X velocity coordinate in meters per second
     * \param[out] vy      Sensor Y velocity coordinate in meters per second
     * \param[out] vz      Sensor Z velocity coordinate in meters per second
     */
    virtual TSMWarning
            * getSensorVelocity(const double& time, double& vx, double& vy,
                                double& vz) throw (TSMError);

    /**
     * Sets the value of the adjustable parameter indicated by index
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index    Selects the sensor parameter
     * \param[in] value    Contains the value to set the parameter
     */
    virtual TSMWarning
            * setCurrentParameterValue(const int& index, const double& value)
                                                                              throw (TSMError);

    /**
     * Gets the value of the adjustable parameter indicated by index
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index    Selects the sensor parameter
     * \param[out] ret     Returns the parameter value
     */
    virtual TSMWarning
    * getCurrentParameterValue(const int& index, double &ret) throw (TSMError);

    /**
     * Gets the name of the parameter indicated by index
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index    Selects the sensor parameter
     * \param[out] name    Returns the parameter name
     */
    virtual TSMWarning
    * getParameterName(const int& index, std::string& name) throw (TSMError);

    /**
     * Gets the number of parameters for the sensor model
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[out] return_val    Returns the number of parameters
     */
    virtual TSMWarning *getNumParameters(int &return_val) throw (TSMError);

    /**
     * Sets the original value of the parameter indicated by index
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index    Selects the sensor parameter
     * \param[in] value    Contains the value to set the parameter
     */
    virtual TSMWarning
            * setOriginalParameterValue(const int& index, const double& value)
                                                                               throw (TSMError);

    /**
     * Gets the value of the parameter indicated by index
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index    Selects the sensor parameter
     * \param[out] return_val     Returns the parameter value
     */
    virtual TSMWarning
            *getOriginalParameterValue(const int& index, double &return_val)
                                                                             throw (TSMError);

    /**
     * Gets the type of the parameter indicated by index.  The parameter types
     * are as follows:
     *
     * 0 = None        the parameter value has not yet been initialized,
     * 1 = Fictitious  the parameter value has been calculated by resection or other means,
     * 2 = Real        the parameter value has been measured or read from support data,
     * 3 = Exact        the parameter value has been specified and is assumed to have no uncertainty.
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index    Selects the sensor parameter
     * \param[out] return_val    Returns the parameter type
     */
    virtual TSMWarning
            *getParameterType(const int& index,
                              TSMMisc::Param_CharType &return_val)
                                                                   throw (TSMError);

    /**
     * Sets the type of the parameter indicated by index.  The parameter types
     * are as follows:
     *
     * 0 = None        the parameter value has not yet been initialized,
     * 1 = Fictitious  the parameter value has been calculated by resection or other means,
     * 2 = Real        the parameter value has been measured or read from support data,
     * 3 = Exact        the parameter value has been specified and is assumed to have no uncertainty.
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index    Selects the sensor parameter
     * \param[in] parameterType    The parameter type
     */
    virtual TSMWarning
            *setParameterType(const int& index,
                              const TSMMisc::Param_CharType &parameterType)
                                                                            throw (TSMError);

    /**
     * Returns a character string that identifies the sensor, the model type,
     * its mode of acquisition and processing path
     *
     * \param[out] pedigree        String that contains the pedigree information     */
    virtual TSMWarning *getPedigree(std::string& pedigree) throw (TSMError);

    /**
     * Returns imageId, a unique identifier associated with the given
     * sensor model
     *
     * \param[out] imageId        String that contains the image identifier
     */
    virtual TSMWarning
    *getImageIdentifier(std::string& imageId) throw (TSMError);

    /**
     * Sets a unique identifier for the image to which the sensor model pertains     *
     * \param[in] imageId        String that contains the image identifier
     */
    virtual TSMWarning
    *setImageIdentifier(const std::string& imageId) throw (TSMError);

    /**
     * Returns sensorId to indicate which sensor was used to acquire the image.
     * This sensorId is meant to uniquely identify the sensor used to make the
     * image.
     *
     * \param[out] sensorId       String that contains the sensor identifier
     */
    virtual TSMWarning
    *getSensorIdentifier(std::string& sensorId) throw (TSMError);

    /**
     * Returns platformId to indicate which sensor was used to acquire the
     * image.  This sensorId is meant to uniquely identify the platform used to
     * collect the image.
     *
     * \param[out] platformId     String that contains the platform identifier
     */
    virtual TSMWarning
    *getPlatformIdentifier(std::string& platformId) throw (TSMError);

    /**
     * Returns x, y and z in meters to indicate the general location of the
     * image.
     *      * \param[out] x       Ground X coordinate in meters
     * \param[out] y       Ground Y coordinate in meters
     * \param[out] z       Ground Z coordinate in meters
     */
    virtual TSMWarning
    *getReferencePoint(double& x, double& y, double& z) throw (TSMError);

    /**
     * Sets the reference point to the input x, y and z position in meters to
     * indicate the general location of the image.
     *
     * \param[in] x        Ground X coordinate in meters
     * \param[in] y        Ground Y coordinate in meters
     * \param[in] z        Ground Z coordinate in meters
     */
    virtual TSMWarning *setReferencePoint(const double& x, const double& y,
                                          const double& z) throw (TSMError);

    /**
     * Returns a string indicating the name of the sensor model.
     *
     * \param[out] name    Name of the sensor model
     */
    virtual TSMWarning *getSensorModelName(std::string& name) throw (TSMError);

    /**
     * Gets the number of lines and samples in the image.
     *
     * \param[out] num_lines     Number of image lines
     * \param[out] num_samples   Number of image samples
     */
    virtual TSMWarning
    *getImageSize(int& num_lines, int& num_samples) throw (TSMError);

    /**
     * Returns a string representing the state of the sensor model.  The state
     * string is made up of the sensor model name, followed by a space, then
     * the SICD XML as a string.
     *
     * \param[out] return_val     State of the sensor model
     */
    virtual TSMWarning
    *getSensorModelState(std::string &return_val) throw (TSMError);

    /**
     * Returns the minimum and maximum altitudes that describe the range of
     * validity of the model
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[out] minHeight      Minimum valid altitude in meters above the WGS-84 ellipsoid
     * \param[out] maxHeight      Maximum valid altitude in meters above the WGS-84 ellipsoid
     */
    virtual TSMWarning
            *getValidAltitudeRange(double& minHeight, double& maxHeight)
                                                                         throw (TSMError);

    /**
     * Calculates the direction of illumination at the given ground position
     * x, y, z.  The returned values define a direction vector that points
     * from the illumination source to the given ground point.
     *
     * \param[in] x        Ground X coordinate in meters
     * \param[in] y        Ground Y coordinate in meters
     * \param[in] z        Ground Z coordinate in meters
     * \param[out] direction_x    X component of illumination direction vector
     * \param[out] direction_y    Y component of illumination direction vector
     * \param[out] direction_z    Z component of illumination direction vector
     */
    virtual TSMWarning
            *getIlluminationDirection(const double& x, const double& y,
                                      const double& z, double& direction_x,
                                      double& direction_y, double& direction_z)
                                                                                throw (TSMError);

    /**
     * Gets the number of systematic error corrections for the associated
     * sensor model.  The returned value is a one subscripted reference (values
     * start at 1).
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[out] return_val     Number of systematic error corrections defined for the sensor model
     */
    virtual TSMWarning
    *getNumSystematicErrorCorrections(int &return_val) throw (TSMError);

    /**
     * Returns name to indicate the name of the sensor model systematic error
     * correction for the specified index.
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index    Selects the systematic error correction switch
     * \param[out] name    The systematic error correction name
     */
    virtual TSMWarning
            *getSystematicErrorCorrectionName(const int& index,
                                              std::string &name)
                                                                 throw (TSMError);

    /**
     * Sets the switch of the systematic error correction indicated by index.
     * A systematic error correction switch of "False" turns off the associated
     * (by index) systematic error correction.  A value of "True" turns on the
     * associated (by index) systematic error correction.
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index    Selects the systematic error correction switch
     * \param[in] value    A boolean switch
     * \param[in] parameterType    The value to set the parameter type
     */
    virtual TSMWarning
            *setCurrentSystematicErrorCorrectionSwitch(
                                                       const int& index,
                                                       const bool &value,
                                                       const TSMMisc::Param_CharType& parameterType)
                                                                                                     throw (TSMError);

    /**
     * Returns the value of the systematic error correction switch given by
     * index
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] index    Selects the systematic error correction switch
     * \param[out] ret     Boolean corresponding to the state of the associated systematic error correction
     */
    virtual TSMWarning
            * getCurrentSystematicErrorCorrectionSwitch(const int& index,
                                                        bool &ret)
                                                                   throw (TSMError);

    /**
     * Returns covariance model information. This data supplies the data to
     * compute cross covariance between images. Images may be correlated
     * because they are taken by the same sensor or from sensors on the same
     * platform. Images may also be correlated due to post processing of the
     * sensor models.  A NULL pointer may be returned indicating that there
     * are no correlations between images from the sensor.
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[out] covModel     a pointer to a class containing data necessary to perform error analysis and propagation 
     */
    virtual TSMWarning
    * getCovarianceModel(tsm_CovarianceModel*& covModel) throw (TSMError);

    /**
     * Gives a sensor specific error for the given input image point. The error
     * is reported as the four terms of a 2x2 covariance mensuration error
     * matrix. This error term is meant to map error terms that are not modeled
     * in the sensor model to image space for inclusion in error propagation.
     * The extra error is added to the mensuration error that may already be in
     * the matrix.
     *
     * This function is not currently implemented for this sensor model.
     *
     * \param[in] line     Image line in pixels
     * \param[in] sample   Image sample in pixels
     * \param[out] covariance      2x2 matrix containing line variance, line-sample covariance, line-sample covariance, and sample variance
     */
    virtual TSMWarning
    * getUnmodeledError(const double line, const double sample,
                        double covariance[4]) throw (TSMError);

    /**
     * Gives the cross covariance for unmodeled error between two image points
     * on the same image. The error is reported as the four terms of a 2x2
     * matrix. The unmodeled cross covariance is added to any values that may
     * already be in the cross covariance matrix.
     *
     * This function is not currently implemented for this sensor model.
     *      * \param[in] pt1Line    Image line in pixels
     * \param[in] pt1Sample  Image sample in pixels
     * \param[in] pt2Line    Image line in pixels
     * \param[in] pt2Sample  Image sample in pixels
     * \param[out] crossCovariance      2x2 matrix containing line variance, line-sample covariance, line-sample covariance, and sample variance
     */
    virtual TSMWarning
    * getUnmodeledCrossCovariance(const double pt1Line, const double pt1Sample,
                                  const double pt2Line, const double pt2Sample,
                                  double crossCovariance[4]) throw (TSMError);

    /**
     * Returns collectionId to indicate an identifier that uniquely identifies
     * a collection activity by a sensor platform.
     *
     * \param[out] collectionId     String that contains the collection identifier
     */
    virtual TSMWarning
    *getCollectionIdentifier(std::string& collectionId) throw (TSMError);

    /**
     * Returns a shareable flag to indicate whether or not the sensor model
     * parameter adjustments are shareable across images for the sensor model
     * adjustable parameter referenced by index.
     *
     * \param[in] index    Selects the sensor model adjustable parameter
     * \param[out] shareable    Boolean flag identifying whether or not the adjustments are shareable across images
     */
    virtual TSMWarning
    *isParameterShareable(const int& index, bool& shareable) throw (TSMError);

    /**
     * returns characteristics to indicate how the sensor model adjustable parameter referenced by index may be shareable across images.
     *
     * \param[in] index    Selects the sensor model adjustable parameter
     * \param[out] requireModelNameMatch      Boolean flag identifying whether or not the sensor model names must match for the parameter to be shareable
     * \param[out] requireSensorIDMatch       Boolean flag identifying whether or not the sensor IDs must match for the parameter to be shareable
     * \param[out] requirePlatformIDMatch     Boolean flag identifying whether or not the platform IDs must match for the parameter to be shareable
     * \param[out] requireCollectionIDMatch   Boolean flag identifying whether or not the collection IDs must match for the parameter to be shareable
     * \param[out] requireTrajectoryIDMatch   Boolean flag identifying whether or not the trajectory IDs must match for the parameter to be shareable
     * \param[out] requireDateTimeMatch       Boolean flag identifying whether or not the image date/times must be similar for the parameter to be shareable
     * \param[out] allowableTimeDelta         Double indicating the time gap (in seconds) that may exist between images for the parameter to be shareable
     */
    virtual TSMWarning
    * getParameterSharingCriteria(const int& index,
                                  bool& requireModelNameMatch,
                                  bool& requireSensorIDMatch,
                                  bool& requirePlatformIDMatch,
                                  bool& requireCollectionIDMatch,
                                  bool& requireTrajectoryIDMatch,
                                  bool& requireDateTimeMatch,
                                  double& allowableTimeDelta) throw (TSMError);

protected:
    std::string mSensorModelState;
    std::string mSensorModelName;
    std::auto_ptr<six::sidd::DerivedData> mData;
    std::auto_ptr<scene::GridGeometry> mGrid;

    /**
     * Transforms the given l, s values from units of pixels from upper left
     * to meters with the origin at the center of the image.
     *
     * \param[in] l     Line position in terms of pixels from upper left
     * \param[in] s     Sample position in terms of pixels from upper left
     * \return A scene::RowCol<double> containing the distance in meters from the center of the image
     */
    scene::RowCol<double> fromPixel(double l, double s);
};

}

#endif
