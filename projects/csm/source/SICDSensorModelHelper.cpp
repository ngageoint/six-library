/* =========================================================================
 * This file is part of the CSM SICD Plugin
 * =========================================================================
 *
 * (C) Copyright 2004 - 2015, MDA Information Systems LLC
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

#include <six/sicd/Utilities.h>
#include <six/csm/SICDSensorModelHelper.h>

namespace six
{
namespace CSM
{
SICDSensorModelHelper::SICDSensorModelHelper(
        mem::SharedPtr<six::sicd::ComplexData> data,
        const scene::SceneGeometry& geometry) :
    ProjectionModelBasedSensorModelHelper(
            geometry,
            mem::SharedPtr<scene::ProjectionModel>(
                    six::sicd::Utilities::getProjectionModel(data.get(),
                                                             &geometry))),
    mData(data)
{
}

types::RowCol<double> SICDSensorModelHelper::getSampleSpacing() const
{
    return types::RowCol<double>(mData->grid->row->sampleSpacing,
                                 mData->grid->col->sampleSpacing);
}

types::RowCol<double>
SICDSensorModelHelper::fromPixel(const csm::ImageCoord& pos) const
{
    const six::sicd::ImageData& imageData(*mData->imageData);
    const types::RowCol<double> aoiOffset(imageData.firstRow,
                                          imageData.firstCol);

    const types::RowCol<double> adjustedPos(
            pos.line - (imageData.scpPixel.row - aoiOffset.row),
            pos.samp - (imageData.scpPixel.col - aoiOffset.col));

    return types::RowCol<double>(
            adjustedPos.row * mData->grid->row->sampleSpacing,
            adjustedPos.col * mData->grid->col->sampleSpacing);
}

types::RowCol<double>
SICDSensorModelHelper::toPixel(const types::RowCol<double>& pos) const
{
    const six::sicd::ImageData& imageData(*mData->imageData);
    const types::RowCol<double> aoiOffset(imageData.firstRow,
                                          imageData.firstCol);

    const types::RowCol<double> offset(
            imageData.scpPixel.row - aoiOffset.row,
            imageData.scpPixel.col - aoiOffset.col);

    return types::RowCol<double>(
            (pos.row / mData->grid->row->sampleSpacing) + offset.row,
            (pos.col / mData->grid->col->sampleSpacing) + offset.col);
}
}
}
