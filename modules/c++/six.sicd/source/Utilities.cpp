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
	// Start by looking for error stats and filling in the matrices
	math::linear::MatrixMxN<7,7,double> sensorCovar((double) 0.0);
	math::linear::MatrixMxN<2,2,double> unmodeledCovar((double) 0.0);
	math::linear::MatrixMxN<2,2,double> ionoCovar((double) 0.0);
	math::linear::MatrixMxN<1,1,double> tropoCovar((double) 0.0);
	six::FrameType frameType = six::FrameType::RIC_ECF;
	if (data->errorStatistics.get())
	{
		if (data->errorStatistics->components->posVelError.get())
		{
			// This could probably be more elegant, but whatevs
			double p1 = data->errorStatistics->components->posVelError->p1;
			double p2 = data->errorStatistics->components->posVelError->p2;
			double p3 = data->errorStatistics->components->posVelError->p3;
			double v1 = data->errorStatistics->components->posVelError->v1;
			double v2 = data->errorStatistics->components->posVelError->v2;
			double v3 = data->errorStatistics->components->posVelError->v3;
			sensorCovar(0,0) = pow(p1,2);
			sensorCovar(1,1) = pow(p2,2);
			sensorCovar(2,2) = pow(p3,2);
			sensorCovar(3,3) = pow(v1,2);
			sensorCovar(4,4) = pow(v2,2);
			sensorCovar(5,5) = pow(v3,2);
			sensorCovar(6,6) = pow(data->errorStatistics->components->radarSensor->rangeBias,2);
			//Position Error
			sensorCovar(0,1) = p1*p2*data->errorStatistics->components->posVelError->corrCoefs->p1p2;
			sensorCovar(1,0) = sensorCovar(0,1);
			sensorCovar(0,2) = p1*p3*data->errorStatistics->components->posVelError->corrCoefs->p1p3;
			sensorCovar(2,0) = sensorCovar(0,2);
			sensorCovar(1,2) = p2*p3*data->errorStatistics->components->posVelError->corrCoefs->p2p3;
			sensorCovar(2,1) = sensorCovar(1,2);
			//Velocity Error
			sensorCovar(3,4) = v1*v2*data->errorStatistics->components->posVelError->corrCoefs->v1v2;
			sensorCovar(4,3) = sensorCovar(3,4);
			sensorCovar(3,5) = v1*v3*data->errorStatistics->components->posVelError->corrCoefs->v1v3;
			sensorCovar(5,3) = sensorCovar(3,5);
			sensorCovar(4,5) = v2*v3*data->errorStatistics->components->posVelError->corrCoefs->v2v3;
			sensorCovar(5,4) = sensorCovar(4,5);
			//Position-Velocity Covariance
			sensorCovar(0,3) = p1*v1*data->errorStatistics->components->posVelError->corrCoefs->p1v1;
			sensorCovar(3,0) = sensorCovar(0,3);
			sensorCovar(0,4) = p1*v2*data->errorStatistics->components->posVelError->corrCoefs->p1v2;
			sensorCovar(4,0) = sensorCovar(0,4);
			sensorCovar(0,5) = p1*v3*data->errorStatistics->components->posVelError->corrCoefs->p1v3;
			sensorCovar(5,0) = sensorCovar(0,5);
			sensorCovar(1,3) = p2*v1*data->errorStatistics->components->posVelError->corrCoefs->p2v1;
			sensorCovar(3,1) = sensorCovar(1,3);
			sensorCovar(1,4) = p2*v2*data->errorStatistics->components->posVelError->corrCoefs->p2v2;
			sensorCovar(4,1) = sensorCovar(1,4);
			sensorCovar(1,5) = p2*v3*data->errorStatistics->components->posVelError->corrCoefs->p2v3;
			sensorCovar(5,1) = sensorCovar(1,5);
			sensorCovar(2,3) = p3*v1*data->errorStatistics->components->posVelError->corrCoefs->p3v1;
			sensorCovar(3,2) = sensorCovar(2,3);
			sensorCovar(2,4) = p3*v2*data->errorStatistics->components->posVelError->corrCoefs->p3v2;
			sensorCovar(4,2) = sensorCovar(2,4);
			sensorCovar(2,5) = p3*v3*data->errorStatistics->components->posVelError->corrCoefs->p3v3;
			sensorCovar(5,2) = sensorCovar(2,5);
		} else {
			// Fill in default values, for now just leave everything zero
		}

		if(data->errorStatistics->compositeSCP.get())
		{
			double rg = data->errorStatistics->compositeSCP->xErr;
			double az = data->errorStatistics->compositeSCP->yErr;
			double corr = data->errorStatistics->compositeSCP->xyErr;
			double rg_ss = data->grid->row->sampleSpacing;
			double az_ss = data->grid->col->sampleSpacing;

			unmodeledCovar(0,0) = pow(rg,2) / pow(rg_ss,2);
			unmodeledCovar(1,1) = pow(az,2) / pow(az_ss,2);
			unmodeledCovar(0,1) = corr*(rg * az) / (rg_ss * az_ss);
			unmodeledCovar(1,0) = unmodeledCovar(0,1);
		} else {
			// Fill in default values, leave as zeros
		}

		if(data->errorStatistics->components->ionoError.get())
		{
			ionoCovar(0,0) = pow(data->errorStatistics->components->ionoError->ionoRangeVertical,2);
			ionoCovar(1,1) = pow(data->errorStatistics->components->ionoError->ionoRangeRateVertical,2);
			ionoCovar(0,1) = data->errorStatistics->components->ionoError->ionoRangeVertical
					* data->errorStatistics->components->ionoError->ionoRangeRateVertical
					* data->errorStatistics->components->ionoError->ionoRgRgRateCC;
			ionoCovar(1,0) = ionoCovar(0,1);
		} else {
			// Fill in defaults, leave zero for now
		}

		if(data->errorStatistics->components->tropoError.get())
		{
			tropoCovar(0,0) = pow(data->errorStatistics->components->tropoError->tropoRangeVertical,2);
		} else {
			// Fill in defaults, leave zero for now
		}
		if(data->errorStatistics->components->posVelError.get())
		{
			frameType = data->errorStatistics->components->posVelError->frame;
		}
	}

    six::ComplexImageGridType gridType = data->grid->type;
    int lookDir = (data->scpcoa->sideOfTrack == 1) ? 1 : -1;

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
                                                   sensorCovar,
                                                   unmodeledCovar,
                                                   ionoCovar,
                                                   tropoCovar,
                                                   frameType.toString());

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
                                                   sensorCovar,
                                                   unmodeledCovar,
                                                   ionoCovar,
                                                   tropoCovar,
                                                   frameType.toString());
    case six::ComplexImageGridType::XRGYCR:
        // Note: This case has not been tested due to a lack of test data
        return new scene::XRGYCRProjectionModel(geom->getSlantPlaneZ(),
                                                data->grid->row->unitVector,
                                                data->grid->col->unitVector,
                                                data->geoData->scp.ecf,
                                                data->position->arpPoly,
                                                data->grid->timeCOAPoly,
                                                lookDir,
                                                sensorCovar,
                								unmodeledCovar,
                								ionoCovar,
                								tropoCovar,
                								frameType.toString());
    case six::ComplexImageGridType::XCTYAT:
        // Note: This case has not been tested due to a lack of test data
        return new scene::XCTYATProjectionModel(geom->getSlantPlaneZ(),
                                                data->grid->row->unitVector,
                                                data->grid->col->unitVector,
                                                data->geoData->scp.ecf,
                                                data->position->arpPoly,
                                                data->grid->timeCOAPoly,
                                                lookDir,
                                                sensorCovar,
                                                unmodeledCovar,
                                                ionoCovar,
                                                tropoCovar,
                                                frameType.toString());
    case six::ComplexImageGridType::PLANE:
        // Note: This case has not been tested due to a lack of test data
        return new scene::PlaneProjectionModel(geom->getSlantPlaneZ(),
                                               data->grid->row->unitVector,
                                               data->grid->col->unitVector,
                                               data->geoData->scp.ecf,
                                               data->position->arpPoly,
                                               data->grid->timeCOAPoly, lookDir,
                                               sensorCovar,
                                               unmodeledCovar,
                                               ionoCovar,
                                               tropoCovar,
                                               frameType.toString());

    default:
        throw except::Exception(Ctxt(std::string("Invalid grid type: ")
                + gridType.toString()));

    }
}


