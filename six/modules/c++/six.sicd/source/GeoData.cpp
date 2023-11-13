/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2020, MDA Information Systems LLC
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
#include <memory>

#include "six/sicd/GeoData.h"
#include "six/sicd/ImageData.h"

namespace six
{
namespace sicd
{
const double GeoData::ECF_THRESHOLD = 1e-2;

bool GeoData::operator==(const GeoData& rhs) const
{
    return (earthModel == rhs.earthModel && scp == rhs.scp &&
            imageCorners == rhs.imageCorners && validData == rhs.validData
            && geoInfos == rhs.geoInfos);
}

void GeoData::fillDerivedFields(const ImageData& imageData,
        const scene::ProjectionModel& model)
{
    if (!Init::isUndefined(scp.ecf) &&
        Init::isUndefined(scp.llh))
    {
        scene::ECEFToLLATransform transformer;
        scp.llh = transformer.transform(scp.ecf);
    }
    if (!Init::isUndefined(scp.llh) &&
        Init::isUndefined(scp.ecf))
    {
        scene::LLAToECEFTransform transformer;
        scp.ecf = transformer.transform(scp.llh);
    }

    if (Init::isUndefined(imageCorners.getCorner(0)) &&
        !Init::isUndefined(imageData.numRows) &&
        !Init::isUndefined(imageData.numCols))
    {
        std::vector<RowColDouble> cornerLineSample;
        cornerLineSample.resize(4);
        cornerLineSample[0] = RowColDouble(1, 1);
        cornerLineSample[1] =
                RowColDouble(1, static_cast<double>(imageData.numCols));
        cornerLineSample[2] =
                RowColDouble(static_cast<double>(imageData.numRows),
                    static_cast<double>(imageData.numCols));
        cornerLineSample[3] =
                RowColDouble(static_cast<double>(imageData.numRows), 1);

        scene::ECEFToLLATransform transformer;
        for (size_t ii = 0; ii < cornerLineSample.size(); ++ii)
        {
            const LatLonAlt lla = transformer.transform(model.imageGridToECEF(
                    cornerLineSample[ii]));
            imageCorners.getCorner(ii).setLatLon(lla);
        }
    }

    // Derived: Add ValidData geocoords
    if (!imageData.validData.empty() &&
        validData.empty())
    {
        validData.resize(imageData.validData.size());
        scene::ECEFToLLATransform transformer;
        for (size_t ii = 0; ii < imageData.validData.size(); ++ii)
        {
            const LatLonAlt lla = transformer.transform(model.imageGridToECEF(
                    imageData.validData[ii]));
            validData[ii] = LatLon(lla.getLat(), lla.getLon());
        }
    }
}

bool GeoData::validate(logging::Logger& log) const
{
    if (Init::isUndefined(scp.llh) || Init::isUndefined(scp.ecf))
    {
        log.error("GeoData.SCP is undefined\n");
        return false;
    }

    // 2.10
    const scene::LLAToECEFTransform transformer;
    const Vector3 derivedEcf = transformer.transform(scp.llh);
    const auto ecfDiff = (scp.ecf - derivedEcf).norm();
    if (ecfDiff > ECF_THRESHOLD)
    {
        std::ostringstream messageBuilder;
        messageBuilder.str("");
        messageBuilder << "GeoData.SCP.ECF and GeoData.SCP.LLH not consistent.\n"
            << "SICD.GeoData.SCP.ECF - SICD.GeoData.SCP.LLH: " << ecfDiff << " (m)\n";
        log.error(messageBuilder);
        return false;
    }

    return true;
}
}
}

