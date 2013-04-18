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
                                                   lookDir);

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
                                                   lookDir);
    case six::ComplexImageGridType::XRGYCR:
        // Note: This case has not been tested due to a lack of test data
        return new scene::XRGYCRProjectionModel(geom->getSlantPlaneZ(),
                                                data->grid->row->unitVector,
                                                data->grid->col->unitVector,
                                                data->geoData->scp.ecf,
                                                data->position->arpPoly,
                                                data->grid->timeCOAPoly,
                                                lookDir);
    case six::ComplexImageGridType::XCTYAT:
        // Note: This case has not been tested due to a lack of test data
        return new scene::XCTYATProjectionModel(geom->getSlantPlaneZ(),
                                                data->grid->row->unitVector,
                                                data->grid->col->unitVector,
                                                data->geoData->scp.ecf,
                                                data->position->arpPoly,
                                                data->grid->timeCOAPoly,
                                                lookDir);
    case six::ComplexImageGridType::PLANE:
        // Note: This case has not been tested due to a lack of test data
        return new scene::PlaneProjectionModel(geom->getSlantPlaneZ(),
                                               data->grid->row->unitVector,
                                               data->grid->col->unitVector,
                                               data->geoData->scp.ecf,
                                               data->position->arpPoly,
                                               data->grid->timeCOAPoly, lookDir);

    default:
        throw except::Exception(Ctxt(std::string("Invalid grid type: ")
                + gridType.toString()));

    }
}


