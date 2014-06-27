/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2013, General Dynamics - Advanced Information Systems
 *
 * six.sicd-c++ is free software; you can redistribute it and/or modify
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
 * License along with this program; If not,
 * see <http://www.gnu.org/licenses/>.
 *
 */
#include "six/sicd/Utilities.h"

using namespace six;

namespace
{
inline
double square(double val)
{
    return (val * val);
}

void assign(math::linear::MatrixMxN<7, 7>& sensorCovar,
            size_t row,
            size_t col,
            double val)
{
    sensorCovar(row, col) = sensorCovar(col, row) = val;
}

void getSensorCovariance(const six::PosVelError& error,
                         double rangeBias,
                         math::linear::MatrixMxN<7, 7>& sensorCovar)
{
    sensorCovar(0, 0) = square(error.p1);
    sensorCovar(1, 1) = square(error.p2);
    sensorCovar(2, 2) = square(error.p3);
    sensorCovar(3, 3) = square(error.v1);
    sensorCovar(4, 4) = square(error.v2);
    sensorCovar(5, 5) = square(error.v3);
    sensorCovar(6, 6) = square(rangeBias);

    if (error.corrCoefs.get())
    {
        const six::CorrCoefs& corrCoefs(*error.corrCoefs);

        // Position Error
        assign(sensorCovar, 0, 1, error.p1 * error.p2 * corrCoefs.p1p2);
        assign(sensorCovar, 0, 2, error.p1 * error.p3 * corrCoefs.p1p3);
        assign(sensorCovar, 1, 2, error.p2 * error.p3 * corrCoefs.p2p3);

        // Velocity Error
        assign(sensorCovar, 3, 4, error.v1 * error.v2 * corrCoefs.v1v2);
        assign(sensorCovar, 3, 5, error.v1 * error.v3 * corrCoefs.v1v3);
        assign(sensorCovar, 4, 5, error.v2 * error.v3 * corrCoefs.v2v3);

        // Position-Velocity Covariance
        assign(sensorCovar, 0, 3, error.p1 * error.v1 * corrCoefs.p1v1);
        assign(sensorCovar, 0, 4, error.p1 * error.v2 * corrCoefs.p1v2);
        assign(sensorCovar, 0, 5, error.p1 * error.v3 * corrCoefs.p1v3);
        assign(sensorCovar, 1, 3, error.p2 * error.v1 * corrCoefs.p2v1);
        assign(sensorCovar, 1, 4, error.p2 * error.v2 *corrCoefs.p2v2);
        assign(sensorCovar, 1, 5, error.p2 * error.v3 * corrCoefs.p2v3);
        assign(sensorCovar, 2, 3, error.p3 * error.v1 * corrCoefs.p3v1);
        assign(sensorCovar, 2, 4, error.p3 * error.v2 * corrCoefs.p3v2);
        assign(sensorCovar, 2, 5, error.p3 * error.v3 * corrCoefs.p3v3);
    }
}

void getErrors(const six::sicd::ComplexData& data,
               scene::Errors& errors)
{
    errors.clear();

    const six::ErrorStatistics* const errorStats(data.errorStatistics.get());
    if (errorStats)
    {
        const six::Components* const components(errorStats->components.get());

        if (components && components->posVelError.get())
        {
            errors.mFrameType = components->posVelError->frame;

            const double rangeBias = (components->radarSensor.get()) ?
                    components->radarSensor->rangeBias : 0.0;

            getSensorCovariance(*components->posVelError,
                                rangeBias,
                                errors.mSensorErrorCovar);
        }

        if (errorStats->compositeSCP.get() &&
            errorStats->compositeSCP->scpType == CompositeSCP::RG_AZ)
        {
            const types::RgAz<double> composite(
                    errorStats->compositeSCP->xErr,
                    errorStats->compositeSCP->yErr);
            const double corr = errorStats->compositeSCP->xyErr;
            const types::RgAz<double> sampleSpacing(
                    data.grid->row->sampleSpacing,
                    data.grid->col->sampleSpacing);

            errors.mUnmodeledErrorCovar(0, 0) =
                    square(composite.rg) / square(sampleSpacing.rg);
            errors.mUnmodeledErrorCovar(1, 1) =
                    square(composite.az) / square(sampleSpacing.az);
            errors.mUnmodeledErrorCovar(0, 1) =
                    errors.mUnmodeledErrorCovar(1, 0) =
                            corr * (composite.rg * composite.az) /
                            (sampleSpacing.rg * sampleSpacing.az);
        }

        if (components && components->ionoError.get())
        {
            const six::IonoError& ionoError(*components->ionoError);
            errors.mIonoErrorCovar(0, 0) =
                    square(ionoError.ionoRangeVertical);
            errors.mIonoErrorCovar(1, 1) =
                    square(ionoError.ionoRangeRateVertical);
            errors.mIonoErrorCovar(0, 1) =
                    errors.mIonoErrorCovar(1, 0) =
                            ionoError.ionoRangeVertical *
                            ionoError.ionoRangeRateVertical *
                            ionoError.ionoRgRgRateCC;
        }

        if (components && components->tropoError.get())
        {
            errors.mTropoErrorCovar(0, 0) =
                    square(components->tropoError->tropoRangeVertical);
        }
    }
}
}

scene::SceneGeometry*
six::sicd::Utilities::getSceneGeometry(const ComplexData* data)
{
    scene::SceneGeometry *geom =
            new scene::SceneGeometry(data->scpcoa->arpVel,
                                     data->scpcoa->arpPos,
                                     data->geoData->scp.ecf,
                                     data->grid->row->unitVector,
                                     data->grid->col->unitVector);

    return geom;
}

scene::ProjectionModel*
six::sicd::Utilities::getProjectionModel(const ComplexData* data, 
        const scene::SceneGeometry* geom)
{
    const six::ComplexImageGridType gridType = data->grid->type;
    const int lookDir = (data->scpcoa->sideOfTrack == 1) ? 1 : -1;

    // Parse error statistics so we can pass these to the projection model as
    // well
    scene::Errors errors;
    getErrors(*data, errors);

    switch ((int) gridType)
    {
    case six::ComplexImageGridType::RGAZIM:
        return new scene::RangeAzimProjectionModel(
                                                   data->pfa->polarAnglePoly,
                                                   data->pfa->spatialFrequencyScaleFactorPoly,
                                                   geom->getSlantPlaneZ(),
                                                   data->grid->row->unitVector,
                                                   data->grid->col->unitVector,
                                                   data->geoData->scp.ecf,
                                                   data->position->arpPoly,
                                                   data->grid->timeCOAPoly,
                                                   lookDir,
                                                   errors);

    case six::ComplexImageGridType::RGZERO:
        return new scene::RangeZeroProjectionModel(
                                                   data->rma->inca->timeCAPoly,
                                                   data->rma->inca->dopplerRateScaleFactorPoly,
                                                   data->rma->inca->rangeCA,
                                                   geom->getSlantPlaneZ(),
                                                   data->grid->row->unitVector,
                                                   data->grid->col->unitVector,
                                                   data->geoData->scp.ecf,
                                                   data->position->arpPoly,
                                                   data->grid->timeCOAPoly,
                                                   lookDir,
                                                   errors);
    case six::ComplexImageGridType::XRGYCR:
        // Note: This case has not been tested due to a lack of test data
        return new scene::XRGYCRProjectionModel(geom->getSlantPlaneZ(),
                                                data->grid->row->unitVector,
                                                data->grid->col->unitVector,
                                                data->geoData->scp.ecf,
                                                data->position->arpPoly,
                                                data->grid->timeCOAPoly,
                                                lookDir,
                                                errors);
    case six::ComplexImageGridType::XCTYAT:
        // Note: This case has not been tested due to a lack of test data
        return new scene::XCTYATProjectionModel(geom->getSlantPlaneZ(),
                                                data->grid->row->unitVector,
                                                data->grid->col->unitVector,
                                                data->geoData->scp.ecf,
                                                data->position->arpPoly,
                                                data->grid->timeCOAPoly,
                                                lookDir,
                                                errors);
    case six::ComplexImageGridType::PLANE:
        // Note: This case has not been tested due to a lack of test data
        return new scene::PlaneProjectionModel(geom->getSlantPlaneZ(),
                                               data->grid->row->unitVector,
                                               data->grid->col->unitVector,
                                               data->geoData->scp.ecf,
                                               data->position->arpPoly,
                                               data->grid->timeCOAPoly,
                                               lookDir,
                                               errors);
    default:
        throw except::Exception(Ctxt("Invalid grid type: " +
                gridType.toString()));
    }
}
