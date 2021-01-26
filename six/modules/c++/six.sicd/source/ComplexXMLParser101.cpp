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

#include <six/sicd/ComplexXMLParser101.h>

namespace
{
typedef xml::lite::Element* XMLElem;
}

using namespace six::sicd;

ComplexXMLParser101::ComplexXMLParser101(const std::string& version,
                                         logging::Logger* log,
                                         bool ownLog) :
    ComplexXMLParser10x(version, log, ownLog)
{
}

XMLElem ComplexXMLParser101::convertGeoInfoToXML(
    const six::GeoInfo *geoInfo,
    XMLElem parent) const
{
    //! 1.0.1 has ordering (1. Desc, 2. choice, 3. GeoInfo)
    XMLElem geoInfoXML = newElement("GeoInfo", parent);
    if (!geoInfo->name.empty())
        setAttribute(geoInfoXML, "name", geoInfo->name);

    common().addParameters("Desc", geoInfo->desc, geoInfoXML);

    const size_t numLatLons = geoInfo->geometryLatLon.size();
    if (numLatLons == 1)
    {
        common().createLatLon("Point", geoInfo->geometryLatLon[0], geoInfoXML);
    }
    else if (numLatLons >= 2)
    {
        XMLElem linePolyXML = newElement(numLatLons == 2 ? "Line" : "Polygon",
                                         geoInfoXML);
        setAttribute(linePolyXML, "size", numLatLons);

        for (size_t ii = 0; ii < numLatLons; ++ii)
        {
            XMLElem v = common().createLatLon(numLatLons == 2 ? "Endpoint" : "Vertex",
                         geoInfo->geometryLatLon[ii], linePolyXML);
            setAttribute(v, "index", ii + 1);
        }
    }

    for (size_t ii = 0; ii < geoInfo->geoInfos.size(); ++ii)
    {
        convertGeoInfoToXML(geoInfo->geoInfos[ii].get(), geoInfoXML);
    }

    return geoInfoXML;
}



