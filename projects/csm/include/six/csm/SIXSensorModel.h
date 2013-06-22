/* =========================================================================
 * This file is part of the CSM SIX Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2011, General Dynamics - Advanced Information Systems
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

#include <scene/SceneGeometry.h>
#include <scene/ProjectionModel.h>

namespace six
{
namespace csm
{
class SIXSensorModel : public ::csm::RasterGM
{
public:
    static const char FAMILY[];

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

public: // GeometricModel methods
    /**
     * Returns the number of adjustable parameters.  This is 0 which means
     * calling any method to get/set a parameter will throw an index out of
     * range error.
     *
     * \return Always returns 0
     */
    virtual int getNumParameters() const;

    /**
     * Always throws since there are no parameters
     */
    virtual std::string getParameterName(int index) const;

    /**
     * Always throws since there are no parameters
     */
    virtual std::string getParameterUnits(int index) const;

    /**
     * There are no parameters, so this always returns false
     */
    virtual bool hasShareableParameters() const;

    /**
     * Always throws since there are no parameters
     */
    virtual bool isParameterShareable(int index) const;

    /**
     * Always throws since there are no parameters
     */
    virtual ::csm::SharingCriteria getParameterSharingCriteria(int index) const;

    /**
     * Always throws since there are no parameters
     */
    virtual double getParameterValue(int index) const;

    /**
     * Always throws since there are no parameters
     */
    virtual void setParameterValue(int index, double value);

    /**
     * Always throws since there are no parameters
     */
    virtual ::csm::param::Type getParameterType(int index) const;

    /**
     * Always throws since there are no parameters
     */
    virtual void setParameterType(int index, ::csm::param::Type pType);

    /**
     * Always throws since there are no parameters
     */
    virtual double getParameterCovariance(int index1,
                                          int index2) const;

    /**
     * Always throws since there are no parameters
     */
    virtual void setParameterCovariance(int index1,
                                        int index2,
                                        double covariance);

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
                                              ::csm::param::Type pType);

    /**
     * Always throws since there are no geometric correction switches
     */
    virtual bool getGeometricCorrectionSwitch(int index) const;

public: // RasterGM methods
    /**
     * Returns the minimum and maximum image coordinates in full image space
     * pixels over which the current model is valid
     *
     * \return +/- 99,999 pixels (The CSM manual states to use this value if
     * there is no limit to the range in a direction.  This allows the model
     * to be used beyond the image boundaries.)
     */
    virtual
    std::pair< ::csm::ImageCoord, ::csm::ImageCoord> getValidImageRange() const;

    /*
     * Returns the minimum and maximum heights (in meters relative to WGS-84
     * ellipsoid) over which the model is valid.
     *
     * \return +/- 99,999 pixels (The CSM manual states to use this value if
     * there is no limit)
     */
    virtual std::pair<double,double> getValidHeightRange() const;

public:
    // All remaining public methods throw ::csm::Error's that they're not
    // implemented

    // These are pure virtual from GeometricModel
    virtual void setReferencePoint(const ::csm::EcefCoord& groundPt);

    virtual std::vector<double> getCrossCovarianceMatrix(
           const ::csm::GeometricModel& comparisonModel,
           ::csm::param::Set pSet,
           const ::csm::GeometricModel::GeometricModelList& otherModels) const;

    // These are pure virtual from RasterGM
    virtual ::csm::ImageCoordCovar groundToImage(
            const ::csm::EcefCoordCovar& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            ::csm::WarningList* warnings) const;

    virtual ::csm::EcefCoordCovar imageToGround(
            const ::csm::ImageCoordCovar& imagePt,
            double height,
            double heightVariance,
            double desiredPrecision,
            double* achievedPrecision,
            ::csm::WarningList* warnings) const;

    virtual ::csm::EcefLocus imageToProximateImagingLocus(
            const ::csm::ImageCoord& imagePt,
            const ::csm::EcefCoord& groundPt,
            double desiredPrecision,
            double* achievedPrecision,
            ::csm::WarningList* warnings) const;

    virtual ::csm::EcefLocus imageToRemoteImagingLocus(
            const ::csm::ImageCoord& imagePt,
            double desiredPrecision,
            double* achievedPrecision,
            ::csm::WarningList* warnings) const;

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

    virtual std::vector<double>
    computeGroundPartials(const ::csm::EcefCoord& groundPt) const;

    virtual const ::csm::CovarianceModel& getCovarianceModel() const;

    std::vector<double> getUnmodeledCrossCovariance(
            const ::csm::ImageCoord& pt1,
            const ::csm::ImageCoord& pt2) const;
};
}
}

#endif
