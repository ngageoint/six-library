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

#ifndef __SIX_CSM_SIDD_PROJECTION_MODEL_BASED_HELPER_H__
#define __SIX_CSM_SIDD_PROJECTION_MODEL_BASED_HELPER_H__

#include <six/sidd/DerivedData.h>
#include <six/csm/ProjectionModelBasedSensorModelHelper.h>

namespace six
{
namespace CSM
{
class SIDDProjectionModelBasedHelper :
        public ProjectionModelBasedSensorModelHelper
{
public:
    SIDDProjectionModelBasedHelper(
        mem::SharedPtr<six::sidd::DerivedData> data,
        const scene::SceneGeometry& geometry);

protected:
    virtual types::RowCol<double> getSampleSpacing() const;

    virtual types::RowCol<double> fromPixel(const csm::ImageCoord& pos) const;

    virtual
    types::RowCol<double> toPixel(const types::RowCol<double>& pos) const;

private:
    const mem::SharedPtr<six::sidd::DerivedData> mData;
    types::RowCol<double> mSampleSpacing;
    types::RowCol<double> mRefPt;
};
}
}

#endif
