/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2017, MDA Information Systems LLC
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
#pragma once
#ifndef __SIX_SICD_AREA_PLANE_UTILITY_H__
#define __SIX_SICD_AREA_PLANE_UTILITY_H__

#include <vector>

#include <six/Types.h>
#include <six/sicd/ComplexData.h>
#include <six/sicd/RadarCollection.h>
#include <six/sicd/Exports.h>

#include <types/RgAz.h>
#include <types/RowCol.h>

namespace six
{
namespace sicd
{
/*!
 * \class AreaPlaneUtility
 * \brief Provides functions to populate
 * ComplexData->RadarCollection->Area->Plane if unset
 */
class SIX_SICD_API AreaPlaneUtility
{
public:
    /*!
     * If data has no areaPlane, derive it and set it
     * \param data ComplexData to populate with areaPlane
     * \param includeSegmentList Should the areaPlane's segmentlist be filled?
     * \param sampleDensity Value to use to calculate SampleSpacing
     */
    static void setAreaPlane(ComplexData& data,
            bool includeSegmentList=true,
            double sampleDensity=DEFAULT_SAMPLE_DENSITY);

    /*!
     * Derives an output plane definition from existing SICD metadata
     * \param data ComplexData from which to derive AreaPlane
     * \param areaPlane AreaPlane to populate
     * \param includeSegmentList Should the areaPlane's segmentlist be filled?
     * \param sampleDensity Value to use to calculate SampleSpacing
     */
    static void deriveAreaPlane(const ComplexData& data,
            AreaPlane& areaPlane,
            bool includeSegmentList=true,
            double sampleDensity=DEFAULT_SAMPLE_DENSITY);

    /*!
     * Returns whether ComplexData has an AreaPlane
     * \param data ComplexData to be queried
     * \return true if ComplexData has AreaPlane
     */
    static bool hasAreaPlane(const ComplexData& data);

    static const double DEFAULT_SAMPLE_DENSITY;

private:
    static RowColDouble deriveReferencePoint(
            const types::RgAz<std::vector<double> >& sortedMetersFromCenter,
            const RowColDouble& spacing);
    static std::vector<RowColDouble > computeCornersPix(
            const ComplexData& data);
    static std::vector<Vector3> computeInPlaneCorners(
            const ComplexData& data,
            const types::RowCol<Vector3>& unitVectors);
    static types::RgAz<std::vector<double> > computeMetersFromCenter(
            const ComplexData& data,
            const types::RowCol<Vector3>& unitVectors);
    static types::RowCol<Vector3> deriveUnitVectors(
            const ComplexData& data);
    static RowColDouble deriveSpacing(
            const ComplexData& data,
            const types::RowCol<Vector3>& unitVectors,
            double sampleDensity);
    static types::RowCol<size_t> derivePlaneDimensions(
            const types::RgAz<std::vector<double> >& sortedMetersFromCenter,
            const RowColDouble& spacing);
};
}
}

#endif

