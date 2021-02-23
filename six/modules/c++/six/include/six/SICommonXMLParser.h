/* =========================================================================
 * This file is part of six-c++
 * =========================================================================
 *
 * (C) Copyright 2004 - 2014, MDA Information Systems LLC
 *
 * six-c++ is free software; you can redistribute it and/or modify
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

#ifndef __SIX_SI_COMMON_XML_PARSER_H__
#define __SIX_SI_COMMON_XML_PARSER_H__

#include <six/CollectionInformation.h>
#include <six/MatchInformation.h>
#include <six/XMLParser.h>
#include <six/Parameter.h>
#include <six/ParameterCollection.h>
#include <six/ErrorStatistics.h>
#include <six/Radiometric.h>
#include <six/GeoInfo.h>
#include <six/MatchInformation.h>

namespace six
{
class SICommonXMLParser : public XMLParser
{
public:
    SICommonXMLParser(const std::string& defaultURI,
                      bool addClassAttributes,
                      const std::string& siCommonURI,
                      logging::Logger* log = nullptr,
                      bool ownLog = false);

    std::string getSICommonURI() const
    {
        return mSICommonURI;
    }

    XMLElem createComplex(const std::string& name, std::complex<double> c,
            XMLElem parent = nullptr) const;
    XMLElem createVector2D(const std::string& name, const std::string& uri,
            Vector2 p = 0.0, XMLElem parent = nullptr) const;
    XMLElem createVector2D(const std::string& name, Vector2 p = 0.0,
            XMLElem parent = nullptr) const;
    XMLElem createVector3D(const std::string& name, const std::string& uri,
            Vector3 p = 0.0, XMLElem parent = nullptr) const;
    XMLElem createVector3D(const std::string& name, Vector3 p = 0.0,
            XMLElem parent = nullptr) const;
    XMLElem createRowCol(const std::string& name, const std::string& uri,
            const std::string& rowName, const std::string& colName,
            const RowColInt& value, XMLElem parent = nullptr) const;
    XMLElem createRowCol(const std::string& name, const std::string& rowName,
            const std::string& colName, const RowColInt& value,
            XMLElem parent = nullptr) const;
    XMLElem createRowCol(const std::string& name, const std::string& uri,
            const std::string& rowName, const std::string& colName,
            const RowColDouble& value, XMLElem parent = nullptr) const;
    XMLElem createRowCol(const std::string& name, const std::string& rowName,
            const std::string& colName, const RowColDouble& value,
            XMLElem parent = nullptr) const;
    XMLElem createRowCol(const std::string& name, const std::string& uri,
            const RowColInt& value, XMLElem parent = nullptr) const;
    XMLElem createRowCol(const std::string& name, const RowColInt& value,
            XMLElem parent = nullptr) const;
    XMLElem createRowCol(const std::string& name, const std::string& uri,
            const RowColDouble& value, XMLElem parent = nullptr) const;
    XMLElem createRowCol(const std::string& name, const RowColDouble& value,
            XMLElem parent = nullptr) const;
    XMLElem createRowCol(const std::string&, const RowColLatLon& value,
            XMLElem parent = nullptr) const;
    XMLElem createRangeAzimuth(const std::string& name,
            const types::RgAz<double>& value, XMLElem parent = nullptr) const;
    XMLElem createLatLon(const std::string& name, const LatLon& value,
            XMLElem parent = nullptr) const;
    XMLElem createLatLon(const std::string& name, const std::string& uri,
            const LatLon& value, XMLElem parent = nullptr) const;
    XMLElem createLatLonAlt(const std::string& name, const LatLonAlt& value,
            XMLElem parent = nullptr) const;

    XMLElem createPoly1D(const std::string& name, const std::string& uri,
            const Poly1D& poly1D, XMLElem parent = nullptr) const;
    XMLElem createPoly2D(const std::string& name, const std::string& uri,
            const Poly2D& poly2D, XMLElem parent = nullptr) const;
    XMLElem createPoly1D(const std::string& name, const Poly1D& poly1D,
            XMLElem parent = nullptr) const;
    XMLElem createPoly2D(const std::string& name, const Poly2D& poly2D,
            XMLElem parent = nullptr) const;
    XMLElem createPolyXYZ(const std::string& name, const PolyXYZ& polyXYZ,
            XMLElem parent = nullptr) const;
    XMLElem createParameter(const std::string& name, const std::string& uri,
            const Parameter& value, XMLElem parent = nullptr) const;
    void addParameters(const std::string& name, const std::string& uri,
            const ParameterCollection& props, XMLElem parent = nullptr) const;
    XMLElem createParameter(const std::string& name, const Parameter& value,
            XMLElem parent = nullptr) const;
    void addParameters(const std::string& name,
            const ParameterCollection& props, XMLElem parent = nullptr) const;
    void addDecorrType(const std::string& name, const std::string& uri,
            DecorrType dt, XMLElem p) const;

    // TODO: Can make this virtual if we ever need it
    //       This is the implementation for SICD 1.x / SIDD 2.0+
    XMLElem convertGeoInfoToXML(const GeoInfo& geoInfo,
            bool hasSIPrefix, XMLElem parent = nullptr) const;
    void parseGeoInfoFromXML(const XMLElem geoInfoXML, GeoInfo* geoInfo) const;

    void parseEarthModelType(XMLElem element, EarthModelType& value) const;

    XMLElem createEarthModelType(const std::string& name,
            const EarthModelType& value,
            XMLElem parent = nullptr) const;

    XMLElem createLatLonFootprint(const std::string& name,
                                  const std::string& cornerName,
                                  const LatLonCorners& corners,
                                  XMLElem parent) const;

    void parsePoly1D(XMLElem polyXML, Poly1D& poly1D) const;
    void parsePoly2D(XMLElem polyXML, Poly2D& poly2D) const;
    void parsePolyXYZ(XMLElem polyXML, PolyXYZ& polyXYZ) const;

    void parseVector2D(XMLElem vecXML, Vector2& vec) const;
    void parseVector3D(XMLElem vecXML, Vector3& vec) const;
    void parseLatLonAlt(XMLElem llaXML, LatLonAlt& lla) const;
    void parseLatLon(XMLElem parent, LatLon& ll) const;
    void parseLatLons(XMLElem pointsXML, const std::string& pointName,
            std::vector<LatLon>& llVec) const;
    void parseRangeAzimuth(XMLElem parent, types::RgAz<double>& value) const;

    void parseRowColDouble(XMLElem parent, const std::string& rowName,
            const std::string& colName, RowColDouble& rc) const;
    void parseRowColDouble(XMLElem parent, RowColDouble& rc) const;

    void parseRowColInt(XMLElem parent, const std::string& rowName,
            const std::string& colName, RowColInt& rc) const;
    void parseRowColInt(XMLElem parent, RowColInt& rc) const;
    void parseRowColInts(XMLElem pointsXML, const std::string& pointName,
            std::vector<RowColInt>& rcVec) const;
    void parseParameter(XMLElem element, Parameter& param) const;

    void parseRowColLatLon(XMLElem parent, RowColLatLon& rc) const;

    void parseParameters(XMLElem paramXML, const std::string& paramName,
            ParameterCollection& props) const;

    void parseDecorrType(XMLElem decorrXML, DecorrType& decorrType) const;
    void parseOptionalDecorrType(XMLElem parent, const std::string& tag, DecorrType& decorrType) const;

    void parseFootprint(XMLElem footprint,
            const std::string& cornerName, LatLonCorners& corners) const;

    void parseFootprint(XMLElem footprint,
            const std::string& cornerName, LatLonAltCorners& corners) const;

    XMLElem convertErrorStatisticsToXML(
        const ErrorStatistics* errorStatistics,
        XMLElem parent = nullptr) const;

    void parseErrorStatisticsFromXML(
        const XMLElem errorStatsXML,
        ErrorStatistics* errorStatistics) const;

    XMLElem convertCollectionInformationToXML(
        const CollectionInformation *obj,
        XMLElem parent = nullptr) const;

    void parseCollectionInformationFromXML(
        const XMLElem collectionInfoXML,
        CollectionInformation *obj) const;

    virtual XMLElem convertRadiometryToXML(
        const Radiometric *obj,
        XMLElem parent = nullptr) const = 0;

    virtual void parseRadiometryFromXML(
        const XMLElem radiometricXML,
        Radiometric *obj) const = 0;

    virtual XMLElem convertMatchInformationToXML(
        const MatchInformation& matchInfo,
        XMLElem parent) const = 0;

    virtual void parseMatchInformationFromXML(
        const XMLElem matchInfoXML,
        MatchInformation* info) const = 0;

protected:
    virtual XMLElem convertCompositeSCPToXML(
        const ErrorStatistics* errorStatistics,
        XMLElem parent = nullptr) const = 0;

    virtual void parseCompositeSCPFromXML(
        const XMLElem errorStatsXML,
        ErrorStatistics* errorStatistics) const = 0;

private:
    // TODO: Can we combine this with parsePoly1D()?
    void parsePoly(XMLElem polyXML, size_t xyzIdx, PolyXYZ& polyXYZ) const;

private:
    const std::string mSICommonURI;
};
}

#endif
