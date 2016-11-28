/* =========================================================================
 * This file is part of six.sicd-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
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
GeoInfo* GeoInfo::clone() const
{
    return new GeoInfo(*this);
}

bool GeoInfo::operator==(const GeoInfo& rhs) const
{
    return (name == rhs.name && geoInfos == rhs.geoInfos &&
            desc == rhs.desc && geometryLatLon == rhs.geometryLatLon);
}

const double GeoData::ECF_THRESHOLD = 1e-2;

GeoData* GeoData::clone()
{
    return new GeoData(*this);
}

bool GeoData::operator==(const GeoData& rhs) const
{
    return (earthModel == rhs.earthModel && scp == rhs.scp &&
            imageCorners == rhs.imageCorners && validData == rhs.validData
            && geoInfos == rhs.geoInfos);
}

void GeoData::fillDerivedFields(const ImageData& imageData,
        const scene::ProjectionModel& model)
{
    if (!Init::isUndefined<Vector3>(scp.ecf) &&
        Init::isUndefined<LatLonAlt>(scp.llh))
    {
        scene::ECEFToLLATransform transformer;
        scp.llh = transformer.transform(scp.ecf);
    }
    if (!Init::isUndefined<LatLonAlt>(scp.llh) &&
        Init::isUndefined<Vector3>(scp.ecf))
    {
        scene::LLAToECEFTransform transformer;
        scp.ecf = transformer.transform(scp.llh);
    }

    if (Init::isUndefined<LatLon>(imageCorners.getCorner(0)) &&
        !Init::isUndefined<size_t>(imageData.numRows) &&
        !Init::isUndefined<size_t>(imageData.numCols))
    {
        std::vector<RowColDouble> cornerLineSample;
        cornerLineSample.resize(4);
        cornerLineSample[0].row = 1;
        cornerLineSample[0].col = 1;
        cornerLineSample[1].row = 1;
        cornerLineSample[1].col = imageData.numCols;
        cornerLineSample[2].row = imageData.numRows;
        cornerLineSample[2].col = imageData.numCols;
        cornerLineSample[3].row = imageData.numRows;
        cornerLineSample[4].col = 1;


        // line 746; requires point_slant_to_ground
    }

    // Derived: Add ValidData geocoords
    if (!imageData.validData.empty() &&
        validData.empty())
    {
        validData.resize(imageData.validData.size());
        scene::ECEFToLLATransform transformer;
        for (size_t ii = 0; ii < imageData.validData.size(); ++ii)
        {
            // TODO: test
            LatLonAlt lla = transformer.transform(model.imageGridToECEF(
                    imageData.validData[ii]));
            validData[ii] = LatLon(lla.getLat(), lla.getLon());
        }
        // line 757; requires point slant to ground
    }
}

bool GeoData::validate(logging::Logger& log) const
{
    bool valid = true;
    std::ostringstream messageBuilder;

    // 2.10
    scene::LLAToECEFTransform transformer;
    Vector3 derivedEcf = transformer.transform(scp.llh);
    double ecfDiff = (scp.ecf - derivedEcf).norm();

    if (ecfDiff > ECF_THRESHOLD)
    {
        messageBuilder.str("");
        messageBuilder << "GeoData.SCP.ECF and GeoData.SCP.LLH not consistent."
            << std::endl << "SICD.GeoData.SCP.ECF - SICD.GeoData.SCP.LLH: "
            << ecfDiff << " (m)" << std::endl;
        log.error(messageBuilder.str());
        valid = false;
    }
    return valid;
}
}
}

