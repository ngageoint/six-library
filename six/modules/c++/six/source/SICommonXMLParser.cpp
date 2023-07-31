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
#include <assert.h>

#include <set>
#include <string>

#include <str/Convert.h>
#include <gsl/gsl.h>

#include <six/Utilities.h>
#include <six/CollectionInformation.h>
#include <six/SICommonXMLParser.h>
#include <six/ParameterCollection.h>
#include <six/XmlLite.h>

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
    SICommonXMLParser::SICommonXMLParser(const std::string& defaultURI, bool addClassAttributes, const std::string& siCommonURI,
        logging::Logger* log, bool ownLog) : XMLParser(defaultURI, addClassAttributes, log, ownLog),
        mSICommonURI(siCommonURI) { }
    SICommonXMLParser::SICommonXMLParser(const std::string& defaultURI, bool addClassAttributes, const std::string& siCommonURI,
        std::unique_ptr<logging::Logger>&& log) : XMLParser(defaultURI, addClassAttributes, std::move(log)),
        mSICommonURI(siCommonURI) { }
    SICommonXMLParser::SICommonXMLParser(const std::string& defaultURI, bool addClassAttributes, const std::string& siCommonURI,
        logging::Logger& log) : XMLParser(defaultURI, addClassAttributes, log),
        mSICommonURI(siCommonURI) { }

void SICommonXMLParser::parseVector2D(const xml::lite::Element* vecXML, Vector2& vec) const
{
    parseDouble(getFirstAndOnly(vecXML, "X"), vec[0]);
    parseDouble(getFirstAndOnly(vecXML, "Y"), vec[1]);
}

void SICommonXMLParser::parseVector3D(const xml::lite::Element* vecXML, Vector3& vec) const
{
    parseDouble(getFirstAndOnly(vecXML, "X"), vec[0]);
    parseDouble(getFirstAndOnly(vecXML, "Y"), vec[1]);
    parseDouble(getFirstAndOnly(vecXML, "Z"), vec[2]);
}

void SICommonXMLParser::parseLatLonAlt(const xml::lite::Element* llaXML, LatLonAlt& lla) const
{
    double lat, lon, alt;

    parseDouble(getFirstAndOnly(llaXML, "Lat"), lat);
    parseDouble(getFirstAndOnly(llaXML, "Lon"), lon);
    parseDouble(getFirstAndOnly(llaXML, "HAE"), alt);

    lla.setLat(lat);
    lla.setLon(lon);
    lla.setAlt(alt);
}

XMLElem SICommonXMLParser::createVector2D(
        const std::string& name,
        const std::string& uri,
        Vector2 p,
        XMLElem parent) const
{
    XMLElem e = newElement(name, (uri.empty()) ? getDefaultURI() : uri, parent);
    createDouble("X", getSICommonURI(), p[0], e);
    createDouble("Y", getSICommonURI(), p[1], e);
    return e;
}

XMLElem SICommonXMLParser::createVector2D(
        const std::string& name,
        Vector2 p,
        XMLElem parent) const
{
    return createVector2D(name, "", p, parent);
}

XMLElem SICommonXMLParser::createVector3D(
        const std::string& name,
        const std::string& uri,
        Vector3 p,
        XMLElem parent) const
{
    XMLElem e = newElement(name, (uri.empty()) ? getDefaultURI() : uri, parent);
    createDouble("X", getSICommonURI(), p[0], e);
    createDouble("Y", getSICommonURI(), p[1], e);
    createDouble("Z", getSICommonURI(), p[2], e);
    return e;
}

XMLElem SICommonXMLParser::createVector3D(
        const std::string& name,
        Vector3 p,
        XMLElem parent) const
{
    return createVector3D(name, "", p, parent);
}

XMLElem SICommonXMLParser::createPoly1D(const std::string& name,
        const std::string& uri, const Poly1D& poly1D, XMLElem parent) const
{
    const size_t order = poly1D.order();
    XMLElem poly1DXML = newElement(name, uri, parent);
    setAttribute(poly1DXML, "order1", order);

    for (size_t ii = 0; ii <= order; ++ii)
    {
        XMLElem coefXML = createDouble("Coef", getSICommonURI(), poly1D[ii],
                                       poly1DXML);
        setAttribute(coefXML, "exponent1", ii);
    }
    return poly1DXML;
}

XMLElem SICommonXMLParser::createPoly1D(const std::string& name,
        const Poly1D& poly1D, XMLElem parent) const
{
    return createPoly1D(name, getDefaultURI(), poly1D, parent);
}

void SICommonXMLParser::parsePolyXYZ(const xml::lite::Element* polyXML, PolyXYZ& polyXYZ) const
{
    XMLElem xXML = getFirstAndOnly(polyXML, "X");
    XMLElem yXML = getFirstAndOnly(polyXML, "Y");
    XMLElem zXML = getFirstAndOnly(polyXML, "Z");

    // Usually these polynomials will always be the same order, but there's
    // no guarantee this will be the case.  So, we'll make a polynomial of
    // the max order of the three polynomials - the constructor will
    // initialize all the coefficients to 0, so we'll get the right behavior
    // if one of these polynomials is lower-order.
    const size_t xOrder =
            str::toType<size_t>(xXML->getAttributes().getValue("order1"));
    const size_t yOrder =
            str::toType<size_t>(yXML->getAttributes().getValue("order1"));
    const size_t zOrder =
            str::toType<size_t>(zXML->getAttributes().getValue("order1"));
    const size_t order =
            std::max<size_t>(std::max<size_t>(xOrder, yOrder), zOrder);

    polyXYZ = PolyXYZ(order);

    parsePoly(xXML, 0, polyXYZ);
    parsePoly(yXML, 1, polyXYZ);
    parsePoly(zXML, 2, polyXYZ);
}

void SICommonXMLParser::parsePoly(const xml::lite::Element* polyXML,
                                  size_t xyzIdx,
                                  PolyXYZ& polyXYZ) const
{
    assert(polyXML != nullptr);

    std::vector<XMLElem> coeffsXML;
    polyXML->getElementsByTagName("Coef", coeffsXML);

    for (size_t ii = 0; ii < coeffsXML.size(); ++ii)
    {
        // Check the order attr, and use that index
        const size_t orderIdx = str::toType<size_t>(
            coeffsXML[ii]->getAttributes().getValue("exponent1"));
        if (orderIdx > polyXYZ.order())
        {
            throw except::Exception(Ctxt(
                    "Order " + std::to_string(orderIdx) + " is out of bounds"));
        }
        parseDouble(coeffsXML[ii], polyXYZ[orderIdx][xyzIdx]);
    }
}

XMLElem SICommonXMLParser::createPolyXYZ(const std::string& name,
        const PolyXYZ& polyXYZ, XMLElem parent) const
{
    const size_t order = polyXYZ.order();
    XMLElem polyXML = newElement(name, getDefaultURI(), parent);

    XMLElem xXML = newElement("X", getSICommonURI(), polyXML);
    XMLElem yXML = newElement("Y", getSICommonURI(), polyXML);
    XMLElem zXML = newElement("Z", getSICommonURI(), polyXML);

    setAttribute(xXML, "order1", order);
    setAttribute(yXML, "order1", order);
    setAttribute(zXML, "order1", order);

    for (size_t ii = 0; ii <= order; ++ii)
    {
        Vector3 v3 = polyXYZ[ii];
        XMLElem xCoefXML = createDouble("Coef", getSICommonURI(), v3[0], xXML);
        XMLElem yCoefXML = createDouble("Coef", getSICommonURI(), v3[1], yXML);
        XMLElem zCoefXML = createDouble("Coef", getSICommonURI(), v3[2], zXML);

        setAttribute(xCoefXML, "exponent1", ii);
        setAttribute(yCoefXML, "exponent1", ii);
        setAttribute(zCoefXML, "exponent1", ii);
    }
    return polyXML;
}

XMLElem SICommonXMLParser::convertGeoInfoToXML(const GeoInfo& geoInfo,
                                               bool hasSIPrefix,
                                               XMLElem parent) const
{
    const std::string uri = hasSIPrefix ? getSICommonURI() : "";

    //! 1.0.x has ordering (1. Desc, 2. choice, 3. GeoInfo)
    XMLElem geoInfoXML = newElement("GeoInfo", uri, parent);

    addParameters("Desc", uri, geoInfo.desc, geoInfoXML);

    const size_t numLatLons = geoInfo.geometryLatLon.size();
    if (numLatLons == 1)
    {
        createLatLon("Point", uri, geoInfo.geometryLatLon[0], geoInfoXML);
    }
    else if (numLatLons >= 2)
    {
        XMLElem linePolyXML = newElement(numLatLons == 2 ? "Line" : "Polygon",
                                         uri, geoInfoXML);

        setAttribute(linePolyXML, "size", numLatLons);

        for (size_t ii = 0; ii < numLatLons; ++ii)
        {
            XMLElem v = createLatLon(
                    numLatLons == 2 ? "Endpoint" : "Vertex",
                    uri,
                    geoInfo.geometryLatLon[ii],
                    linePolyXML);

            setAttribute(v, "index", ii + 1);
        }
    }

    if (!geoInfo.name.empty())
    {
        setAttribute(geoInfoXML, "name", geoInfo.name);
    }

    for (size_t ii = 0; ii < geoInfo.geoInfos.size(); ++ii)
    {
        convertGeoInfoToXML(*geoInfo.geoInfos[ii], hasSIPrefix, geoInfoXML);
    }

    return geoInfoXML;
}

void SICommonXMLParser::parseGeoInfoFromXML(const xml::lite::Element* geoInfoXML, GeoInfo* geoInfo) const
{
    assert(geoInfoXML != nullptr);

    std::vector < XMLElem > geoInfosXML;
    geoInfoXML->getElementsByTagName("GeoInfo", geoInfosXML);
    geoInfo->name = geoInfoXML->getAttributes().getValue("name");

    //optional
    size_t idx(geoInfo->geoInfos.size());
    geoInfo->geoInfos.resize(idx + geoInfosXML.size());

    for (std::vector<XMLElem>::const_iterator it = geoInfosXML.begin(); it
        != geoInfosXML.end(); ++it, ++idx)
    {
        geoInfo->geoInfos[idx].reset(new GeoInfo());
        parseGeoInfoFromXML(*it, geoInfo->geoInfos[idx].get());
    }

    //optional
    parseParameters(geoInfoXML, "Desc", geoInfo->desc);

    XMLElem tmpElem = getOptional(geoInfoXML, "Point");
    if (tmpElem)
    {
        LatLon ll;
        parseLatLon(tmpElem, ll);
        geoInfo->geometryLatLon.push_back(ll);
    }
    else
    {
        std::string pointName = "Endpoint";
        tmpElem = getOptional(geoInfoXML, "Line");
        if (!tmpElem)
        {
            pointName = "Vertex";
            tmpElem = getOptional(geoInfoXML, "Polygon");
        }
        if (tmpElem)
        {
            parseLatLons(tmpElem, pointName, geoInfo->geometryLatLon);
        }
    }
}

XMLElem SICommonXMLParser::createEarthModelType(const std::string& name,
    const EarthModelType& value,
    XMLElem parent) const
{
    return createSixString(name, value, parent);
}
void SICommonXMLParser::parseEarthModelType(const xml::lite::Element& element,
    EarthModelType& value) const
{
    value = six::toType<EarthModelType>(element.getCharacterData());
}
void SICommonXMLParser::parseEarthModelType(const xml::lite::Element* element,
    EarthModelType& value) const
{
    parseEarthModelType(*element, value);
}

XMLElem SICommonXMLParser::createLatLonFootprint(const std::string& name,
                                                 const std::string& cornerName,
                                                 const LatLonCorners& corners,
                                                 XMLElem parent) const
{
    XMLElem footprint = newElement(name, parent);

    // Write the corners in CW order
    XMLElem vertex = createLatLon(cornerName, corners.upperLeft, footprint);
    setAttribute(vertex, "index", "1:FRFC");

    vertex = createLatLon(cornerName, corners.upperRight, footprint);
    setAttribute(vertex, "index", "2:FRLC");

    vertex = createLatLon(cornerName, corners.lowerRight, footprint);
    setAttribute(vertex, "index", "3:LRLC");

    vertex = createLatLon(cornerName, corners.lowerLeft, footprint);
    setAttribute(vertex, "index", "4:LRFC");

    return footprint;
}

void SICommonXMLParser::parsePoly1D(const xml::lite::Element* polyXML, Poly1D& poly1D) const
{
    const auto order1 = str::toType<int>(polyXML->getAttributes().getValue("order1"));
    Poly1D p1D(gsl::narrow<size_t>(order1));

    std::vector < XMLElem > coeffsXML;
    polyXML->getElementsByTagName("Coef", coeffsXML);

    for (auto element : coeffsXML)
    {
        const auto exp1 = str::toType<int>(element->getAttributes().getValue("exponent1"));
        parseDouble(element, p1D[gsl::narrow<size_t>(exp1)]);
    }
    poly1D = p1D;
}

void SICommonXMLParser::parsePoly2D(const xml::lite::Element* polyXML, Poly2D& poly2D) const
{
    const auto order1 = str::toType<int>(polyXML->getAttributes().getValue("order1"));
    const auto order2 = str::toType<int>(polyXML->getAttributes().getValue("order2"));
    Poly2D p2D(gsl::narrow<size_t>(order1), gsl::narrow<size_t>(order2));

    std::vector < XMLElem > coeffsXML;
    polyXML->getElementsByTagName("Coef", coeffsXML);

    for (auto element : coeffsXML)
    {
        const auto exp1 = str::toType<int>(element->getAttributes().getValue("exponent1"));
        const auto exp2 = str::toType<int>(element->getAttributes().getValue("exponent2"));
        parseDouble(element, p2D[gsl::narrow<size_t>(exp1)][exp2]);
    }
    poly2D = p2D;
}
bool SICommonXMLParser::parseOptionalPoly2D(const xml::lite::Element* parent, const std::string& tag, Poly2D& value) const
{
    if (const xml::lite::Element* const element = getOptional(parent, tag))
    {
        parsePoly2D(element, value);
        return true;
    }
    return false;
}


XMLElem SICommonXMLParser::createPoly2D(const std::string& name,
        const std::string& uri, const Poly2D& poly2D, XMLElem parent) const
{
    xml::lite::AttributeNode node;
    XMLElem poly2DXML = newElement(name, uri, parent);
    setAttribute(poly2DXML, "order1", poly2D.orderX());
    setAttribute(poly2DXML, "order2", poly2D.orderY());

    for (size_t ii = 0; ii <= poly2D.orderX(); ii++)
    {
        for (size_t jj = 0; jj <= poly2D.orderY(); jj++)
        {
            XMLElem coefXML = createDouble("Coef", getSICommonURI(),
                                           poly2D[ii][jj], poly2DXML);
            setAttribute(coefXML, "exponent1", ii);
            setAttribute(coefXML, "exponent2", jj);
        }
    }

    return poly2DXML;
}

XMLElem SICommonXMLParser::createPoly2D(const std::string& name,
        const Poly2D& poly2D, XMLElem parent) const
{
    return createPoly2D(name, getDefaultURI(), poly2D, parent);
}


XMLElem SICommonXMLParser::createComplex(const std::string& name,
        six::zdouble c, XMLElem parent) const
{
    XMLElem e = newElement(name, getDefaultURI(), parent);
    createDouble("Real", getSICommonURI(), c.real(), e);
    createDouble("Imag", getSICommonURI(), c.imag(), e);
    return e;
}

XMLElem SICommonXMLParser::createRowCol(
        const std::string& name, const std::string& uri,
        const std::string& rowName, const std::string& colName,
        const RowColInt& value, XMLElem parent) const
{
    XMLElem e = newElement(name, (uri.empty()) ? getDefaultURI() : uri, parent);
    createInt(rowName, getSICommonURI(), value.row, e);
    createInt(colName, getSICommonURI(), value.col, e);
    return e;
}

XMLElem SICommonXMLParser::createRowCol(const std::string& name,
        const std::string& rowName, const std::string& colName,
        const RowColInt& value, XMLElem parent) const
{
    return createRowCol(name, "", rowName, colName, value, parent);
}

XMLElem SICommonXMLParser::createRowCol(
        const std::string& name, const std::string& uri,
        const std::string& rowName, const std::string& colName,
        const RowColDouble& value, XMLElem parent) const
{
    XMLElem e = newElement(name, (uri.empty()) ? getDefaultURI() : uri, parent);
    createDouble(rowName, getSICommonURI(), value.row, e);
    createDouble(colName, getSICommonURI(), value.col, e);
    return e;
}

XMLElem SICommonXMLParser::createRowCol(const std::string& name,
        const std::string& rowName, const std::string& colName,
        const RowColDouble& value, XMLElem parent) const
{
    return createRowCol(name, "", rowName, colName, value, parent);
}

XMLElem SICommonXMLParser::createRowCol(
        const std::string& name, const std::string& uri,
        const RowColInt& value, XMLElem parent) const
{
    return createRowCol(name, uri, "Row", "Col", value, parent);
}

XMLElem SICommonXMLParser::createRowCol(const std::string& name,
        const RowColInt& value, XMLElem parent) const
{
    return createRowCol(name, "", value, parent);
}

XMLElem SICommonXMLParser::createRowCol(
        const std::string& name, const std::string& uri,
        const RowColDouble& value, XMLElem parent) const
{
    return createRowCol(name, uri, "Row", "Col", value, parent);
}

XMLElem SICommonXMLParser::createRowCol(const std::string& name,
        const RowColDouble& value, XMLElem parent) const
{
    return createRowCol(name, "", value, parent);
}

XMLElem SICommonXMLParser::createRowCol(const std::string& name,
        const RowColLatLon& value, XMLElem parent) const
{
    XMLElem e = newElement(name, getDefaultURI(), parent);
    createLatLon("Row", value.row, e);
    createLatLon("Col", value.col, e);
    return e;
}

XMLElem SICommonXMLParser::createRangeAzimuth(const std::string& name,
        const types::RgAz<double>& value, XMLElem parent) const
{
    XMLElem e = newElement(name, getDefaultURI(), parent);
    createDouble("Range", getSICommonURI(), value.rg, e);
    createDouble("Azimuth", getSICommonURI(), value.az, e);
    return e;
}

XMLElem SICommonXMLParser::createLatLon(
        const std::string& name,
        const std::string& uri,
        const LatLon& value,
        XMLElem parent) const
{
    XMLElem e = newElement(name, uri, parent);
    createDouble("Lat", getSICommonURI(), value.getLat(), e);
    createDouble("Lon", getSICommonURI(), value.getLon(), e);
    return e;
}

XMLElem SICommonXMLParser::createLatLon(
        const std::string& name,
        const LatLon& value,
        XMLElem parent) const
{
    return createLatLon(name, getDefaultURI(), value, parent);
}

XMLElem SICommonXMLParser::createLatLonAlt(const std::string& name,
        const LatLonAlt& value, XMLElem parent) const
{
    XMLElem e = createLatLon(name, value, parent);
    createDouble("HAE", getSICommonURI(), value.getAlt(), e);
    return e;
}

void SICommonXMLParser::parseParameter(const xml::lite::Element* element, Parameter& p) const
{
    p.setName(element->getAttributes().getValue("name"));
    p.setValue(element->getCharacterData());
}

void SICommonXMLParser::parseParameters(const xml::lite::Element* paramXML,
        const std::string& paramName, ParameterCollection& props) const
{
    std::vector < XMLElem > elemXML;
    paramXML->getElementsByTagName(paramName, elemXML);

    for (std::vector<XMLElem>::iterator it = elemXML.begin(); it
            != elemXML.end(); ++it)
    {
        Parameter p;
        parseParameter(*it, p);
        props.push_back(p);
    }
}

XMLElem SICommonXMLParser::createParameter(const std::string& name,
        const std::string& uri, const Parameter& value, XMLElem parent) const
{
    XMLElem element = createString(name, uri, value.str(), parent);
    setAttribute(element, "name", value.getName());
    return element;
}

XMLElem SICommonXMLParser::createParameter(const std::string& name,
        const Parameter& value, XMLElem parent) const
{
    return createParameter(name, getDefaultURI(), value, parent);
}

void SICommonXMLParser::addParameters(const std::string& name,
        const std::string& uri, const ParameterCollection& props,
        XMLElem parent) const
{
    for (six::ParameterCollection::ConstParameterCollectionIteratorT it = props.begin(); it
            != props.end(); ++it)
    {
        createParameter(name, uri, *it, parent);
    }
}

void SICommonXMLParser::addParameters(const std::string& name,
        const ParameterCollection& props, XMLElem parent) const
{
    addParameters(name, getDefaultURI(), props, parent);
}

void SICommonXMLParser::addDecorrType(const std::string& name,
        const std::string& uri, DecorrType decorrType, XMLElem parent) const
{
    //only adds it if it needs to
    if (Init::isDefined(decorrType))
    {
        if (!Init::isUndefined<double>(decorrType.corrCoefZero)
            && !Init::isUndefined<double>(decorrType.decorrRate))
        {
            XMLElem decorrXML = newElement(name, uri, parent);
            createDouble("CorrCoefZero", uri, decorrType.corrCoefZero, decorrXML);
            createDouble("DecorrRate", uri, decorrType.decorrRate, decorrXML);
        }
    }
}
void SICommonXMLParser::addDecorrType(const std::string& name,
    const std::string& uri, const std::optional<DecorrType>& decorrType, XMLElem parent) const
{
    //only adds it if it needs to
    if (decorrType.has_value())
    {
        addDecorrType(name, uri, *decorrType, parent);
    }
}

void SICommonXMLParser::parseDecorrType(const xml::lite::Element* decorrXML,
                                        DecorrType& decorrType) const
{
    parseDouble(getFirstAndOnly(decorrXML, "CorrCoefZero"),
                decorrType.corrCoefZero);
    parseDouble(getFirstAndOnly(decorrXML, "DecorrRate"),
                decorrType.decorrRate);
}
void SICommonXMLParser::parseOptionalDecorrType(const xml::lite::Element* parent, const std::string& tag,
    DecorrType& decorrType) const
{
    const xml::lite::Element* const decorrXML = getOptional(parent, tag);
    if (decorrXML)
    {
        parseDecorrType(decorrXML, decorrType);
    }
}
void SICommonXMLParser::parseOptionalDecorrType(const xml::lite::Element* parent, const std::string& tag,
    std::optional<DecorrType>& decorrType) const
{
    const xml::lite::Element* const decorrXML = getOptional(parent, tag);
    if (decorrXML)
    {
        DecorrType result;
        parseDecorrType(decorrXML, result);
        decorrType = result;
    }
}

void SICommonXMLParser::parseLatLon(const xml::lite::Element& parent, LatLon& ll) const
{
    double lat, lon;

    parseDouble(getFirstAndOnly(parent, "Lat"), lat);
    parseDouble(getFirstAndOnly(parent, "Lon"), lon);

    ll.setLat(lat);
    ll.setLon(lon);
}
void SICommonXMLParser::parseLatLon(const xml::lite::Element* parent, LatLon& ll) const
{
    parseLatLon(*parent, ll);
}

void SICommonXMLParser::parseLatLons(const xml::lite::Element* pointsXML,
                                     const std::string& pointName,
        std::vector<LatLon>& llVec) const
{
    std::vector < XMLElem > latLonsXML;
    pointsXML->getElementsByTagName(pointName, latLonsXML);

    std::vector<LatLon> tmpll;
    std::vector<size_t> tmpIndxs;
    std::set<size_t> tmpSet;
    for (std::vector<XMLElem>::iterator it = latLonsXML.begin(); it
            != latLonsXML.end(); ++it)
    {
        LatLon ll;
        parseLatLon(*it, ll);

        //! Temporarily store the indices and LatLons in vector, so
        //  validation can be performed
        size_t index =  str::toType<size_t>((*it)->attribute("index"));
        tmpll.push_back(ll);
        tmpIndxs.push_back(index);

        //! Verify there were no duplicates
        if (tmpSet.insert(index).second == false)
        {
            throw except::Exception(Ctxt("Duplicate 'index' found in ["
                + pointsXML->getParent()->getLocalName() + "->"
                + pointsXML->getLocalName() + "]"));
        }
    }

    //! Check the last index is same as the number of children
    if (!tmpSet.empty())
    {
        if (*tmpSet.begin() != 1)
        {
            throw except::Exception(Ctxt("Index of 0 found in ["
                    + pointsXML->getParent()->getLocalName() + "->"
                    + pointsXML->getLocalName() + "]"));
        }
        else if (*tmpSet.rbegin() != (latLonsXML.size()))
        {
            throw except::Exception(Ctxt(
                    "Invalid out-of-bounds 'index' in ["
                    + pointsXML->getParent()->getLocalName() + "->"
                    + pointsXML->getLocalName() + "]"));
        }
    }

    //! Reorder into ascending order
    llVec.resize(tmpIndxs.size());
    for (size_t ii = 0; ii < tmpIndxs.size(); ++ii)
    {
        // indices are 1-based
        llVec[tmpIndxs[ii]-1] = tmpll[ii];
    }
}

void SICommonXMLParser::parseRangeAzimuth(const xml::lite::Element* parent,
                                          types::RgAz<double>& value) const
{
    parseDouble(getFirstAndOnly(parent, "Range"), value.rg);
    parseDouble(getFirstAndOnly(parent, "Azimuth"), value.az);
}

void SICommonXMLParser::parseRowColDouble(
    const xml::lite::Element* parent,
        const std::string& rowName,
        const std::string& colName,
        RowColDouble& rc) const
{
    parseDouble(getFirstAndOnly(parent, rowName), rc.row);
    parseDouble(getFirstAndOnly(parent, colName), rc.col);
}

void SICommonXMLParser::parseRowColLatLon(const xml::lite::Element* parent,
                                          RowColLatLon& rc) const
{
    parseLatLon(getFirstAndOnly(parent, "Row"), rc.row);
    parseLatLon(getFirstAndOnly(parent, "Col"), rc.col);
}

void SICommonXMLParser::parseRowColDouble(const xml::lite::Element* parent,
                                          RowColDouble& rc) const
{
    parseRowColDouble(parent, "Row", "Col", rc);
}

void SICommonXMLParser::parseRowColInt(
        const xml::lite::Element* parent,
        const std::string& rowName,
        const std::string& colName,
        RowColInt& rc) const
{
    parseInt(getFirstAndOnly(parent, rowName), rc.row);
    parseInt(getFirstAndOnly(parent, colName), rc.col);
}

void SICommonXMLParser::parseRowColInt(const xml::lite::Element* parent,
                                       RowColInt& rc) const
{
    parseRowColInt(parent, "Row", "Col", rc);
}

void SICommonXMLParser::parseRowColInts(
        const xml::lite::Element* pointsXML,
        const std::string& pointName,
        std::vector<RowColInt>& rcVec) const
{
    std::vector < XMLElem > rowColXML;
    pointsXML->getElementsByTagName(pointName, rowColXML);

    std::vector<RowColInt> tmprc;
    std::vector<size_t> tmpIndxs;
    std::set<size_t> tmpSet;
    for (std::vector<XMLElem>::iterator it = rowColXML.begin(); it
            != rowColXML.end(); ++it)
    {
        RowColInt rc;
        parseRowColInt(*it, rc);

        //! Temporarily store the indices and rowCol in vector, so
        //  validation can be performed
        size_t index =  str::toType<size_t>((*it)->attribute("index"));
        tmprc.push_back(rc);
        tmpIndxs.push_back(index);

        //! Verify there were no duplicates
        if (tmpSet.insert(index).second == false)
        {
            throw except::Exception(Ctxt("Duplicate 'index' found in ["
                + pointsXML->getParent()->getLocalName() + "->"
                + pointsXML->getLocalName() + "]"));
        }
    }

    //! Check the last index is same as the number of children
    if (!tmpSet.empty())
    {
        if (*tmpSet.begin() != 1)
        {
            throw except::Exception(Ctxt("Index of 0 found in ["
                    + pointsXML->getParent()->getLocalName() + "->"
                    + pointsXML->getLocalName() + "]"));
        }
        else if (*tmpSet.rbegin() != (rowColXML.size()))
        {
            throw except::Exception(Ctxt(
                    "Invalid out-of-bounds 'index' in ["
                    + pointsXML->getParent()->getLocalName() + "->"
                    + pointsXML->getLocalName() + "]"));
        }
    }

    //! Reorder into ascending order
    rcVec.resize(tmpIndxs.size());
    for (size_t ii = 0; ii < tmpIndxs.size(); ++ii)
    {
        // indices are 1-based
        rcVec[tmpIndxs[ii]-1] = tmprc[ii];
    }
}

XMLElem SICommonXMLParser::convertErrorStatisticsToXML(
    const ErrorStatistics* errorStatistics,
    XMLElem parent) const
{
    XMLElem errorStatsXML = newElement("ErrorStatistics", getDefaultURI(),
                                       parent);

    //! version specific implementation
    convertCompositeSCPToXML(errorStatistics, errorStatsXML);

    const Components* const components = errorStatistics->components.get();
    if (components)
    {
        XMLElem componentsXML = newElement("Components",
                                           getSICommonURI(),
                                           errorStatsXML);

        const PosVelError* const posVelError = components->posVelError.get();
        const RadarSensor* const radarSensor = components->radarSensor.get();
        const TropoError* const tropoError = components->tropoError.get();
        const IonoError* const ionoError = components->ionoError.get();

        if (posVelError)
        {
            XMLElem posVelErrXML = newElement("PosVelErr",
                                              getSICommonURI(),
                                              componentsXML);

            createSixString("Frame", getSICommonURI(), posVelError->frame, posVelErrXML);
            createDouble("P1", getSICommonURI(), posVelError->p1, posVelErrXML);
            createDouble("P2", getSICommonURI(), posVelError->p2, posVelErrXML);
            createDouble("P3", getSICommonURI(), posVelError->p3, posVelErrXML);
            createDouble("V1", getSICommonURI(), posVelError->v1, posVelErrXML);
            createDouble("V2", getSICommonURI(), posVelError->v2, posVelErrXML);
            createDouble("V3", getSICommonURI(), posVelError->v3, posVelErrXML);

            const CorrCoefs* const coefs = posVelError->corrCoefs.get();
            if (coefs)
            {
                XMLElem coefsXML = newElement("CorrCoefs",
                                              getSICommonURI(),
                                              posVelErrXML);

                createDouble("P1P2", getSICommonURI(), coefs->p1p2, coefsXML);
                createDouble("P1P3", getSICommonURI(), coefs->p1p3, coefsXML);
                createDouble("P1V1", getSICommonURI(), coefs->p1v1, coefsXML);
                createDouble("P1V2", getSICommonURI(), coefs->p1v2, coefsXML);
                createDouble("P1V3", getSICommonURI(), coefs->p1v3, coefsXML);
                createDouble("P2P3", getSICommonURI(), coefs->p2p3, coefsXML);
                createDouble("P2V1", getSICommonURI(), coefs->p2v1, coefsXML);
                createDouble("P2V2", getSICommonURI(), coefs->p2v2, coefsXML);
                createDouble("P2V3", getSICommonURI(), coefs->p2v3, coefsXML);
                createDouble("P3V1", getSICommonURI(), coefs->p3v1, coefsXML);
                createDouble("P3V2", getSICommonURI(), coefs->p3v2, coefsXML);
                createDouble("P3V3", getSICommonURI(), coefs->p3v3, coefsXML);
                createDouble("V1V2", getSICommonURI(), coefs->v1v2, coefsXML);
                createDouble("V1V3", getSICommonURI(), coefs->v1v3, coefsXML);
                createDouble("V2V3", getSICommonURI(), coefs->v2v3, coefsXML);
            }

            addDecorrType("PositionDecorr", getSICommonURI(),
                          posVelError->positionDecorr, posVelErrXML);
        }
        if (radarSensor)
        {
            XMLElem radarSensorXML = newElement("RadarSensor", getSICommonURI(),
                                                componentsXML);

            createDouble("RangeBias", getSICommonURI(), radarSensor->rangeBias,
                         radarSensorXML);

            createOptionalDouble("ClockFreqSF", getSICommonURI(),
                            radarSensor->clockFreqSF, radarSensorXML);
            createOptionalDouble("TransmitFreqSF", getSICommonURI(),
                            radarSensor->transmitFreqSF, radarSensorXML);

            addDecorrType("RangeBiasDecorr", getSICommonURI(),
                          radarSensor->rangeBiasDecorr, radarSensorXML);
        }
        if (tropoError)
        {
            XMLElem tropoErrXML = newElement("TropoError",
                                             getSICommonURI(),
                                             componentsXML);

            createOptionalDouble("TropoRangeVertical", getSICommonURI(),
                            tropoError->tropoRangeVertical, tropoErrXML);
            createOptionalDouble("TropoRangeSlant", getSICommonURI(),
                            tropoError->tropoRangeSlant, tropoErrXML);

            addDecorrType("TropoRangeDecorr", getSICommonURI(),
                          tropoError->tropoRangeDecorr, tropoErrXML);
        }
        if (ionoError)
        {
            XMLElem ionoErrXML = newElement("IonoError",
                                            getSICommonURI(),
                                            componentsXML);

            createOptionalDouble("IonoRangeVertical", getSICommonURI(),
                            ionoError->ionoRangeVertical, ionoErrXML);
            createOptionalDouble("IonoRangeRateVertical", getSICommonURI(),
                            ionoError->ionoRangeRateVertical, ionoErrXML);

            createDouble("IonoRgRgRateCC", getSICommonURI(),
                         ionoError->ionoRgRgRateCC, ionoErrXML);

            addDecorrType("IonoRangeVertDecorr", getSICommonURI(),
                          ionoError->ionoRangeVertDecorr, ionoErrXML);
        }
    }

   #define xmlNewElement(name, uri, xml) auto name##XML = newElement(name, #name, uri, xml)
    const auto Unmodeled = errorStatistics->Unmodeled.get();
    xmlNewElement(Unmodeled, getSICommonURI(), errorStatsXML);
    if (UnmodeledXML != nullptr)
    {
        #define xmlCreateElement_(name, xml) createDouble((name).tag(), getSICommonURI(), (name).value(), xml)
        #define xmlCreateElement(name, elem) xmlCreateElement_(elem->name, elem ## XML);
        xmlCreateElement(Xrow, Unmodeled);
        xmlCreateElement(Ycol, Unmodeled);
        xmlCreateElement(XrowYcol, Unmodeled);

        const auto pUnmodeledDecorr = get(Unmodeled->unmodeledDecorr);
        auto UnmodeledDecorrXML = newElement(pUnmodeledDecorr, "UnmodeledDecorr", getSICommonURI(), UnmodeledXML);
        if (UnmodeledDecorrXML != nullptr)
        {
            #define xmlCreateDouble_(name, value, elem) createDouble(#name, getSICommonURI(), (value).name, elem)
            #define xmlCreateDouble(name, elem) xmlCreateDouble_(name, *elem, elem ## XML)

            const auto& Xrow = &(pUnmodeledDecorr->Xrow);
            xmlNewElement(Xrow, getSICommonURI(), UnmodeledDecorrXML);
            xmlCreateDouble(CorrCoefZero, Xrow);
            xmlCreateDouble(DecorrRate, Xrow);

            const auto& Ycol = &(pUnmodeledDecorr->Ycol);
            xmlNewElement(Ycol, getSICommonURI(), UnmodeledDecorrXML);
            xmlCreateDouble(CorrCoefZero, Ycol);
            xmlCreateDouble(DecorrRate, Ycol);
        }
    }

    if (!errorStatistics->additionalParameters.empty())
    {
        XMLElem paramsXML = newElement("AdditionalParms",
                                       getSICommonURI(),
                                       errorStatsXML);
        addParameters("Parameter", getSICommonURI(),
                      errorStatistics->additionalParameters, paramsXML);
    }

    return errorStatsXML;
}

inline static XMLElem getOptionalUnmodeledDecorr(const xml::lite::Element& parent,
    XsElement_minOccurs0<UnmodeledS::Decorr>& unmodeledDecorr)
{
    auto retval = XmlLite::getOptional(parent, unmodeledDecorr.tag());
    if (retval != nullptr)
    {
        unmodeledDecorr.value() = UnmodeledS::Decorr{};
    }
    return retval;
}

void SICommonXMLParser::parseErrorStatisticsFromXML(
        const xml::lite::Element& errorStatsXML,
        ErrorStatistics& errorStatistics) const
{
    XMLElem tmpElem = nullptr;
    //optional

    //! version specific CompositeSCP parsing
    parseCompositeSCPFromXML(&errorStatsXML, &errorStatistics);

    XMLElem posVelErrXML = nullptr;
    XMLElem radarSensorXML = nullptr;
    XMLElem tropoErrorXML = nullptr;
    XMLElem ionoErrorXML = nullptr;

    tmpElem = getOptional(errorStatsXML, "Components");
    if (tmpElem)
    {
        //optional
        errorStatistics.components.reset(new Components());

        posVelErrXML = getOptional(tmpElem, "PosVelErr");
        if (posVelErrXML)
        {
            //optional
            errorStatistics.components->posVelError.reset(new PosVelError());
        }

        radarSensorXML = getOptional(tmpElem, "RadarSensor");
        if (radarSensorXML)
        {
            //optional
            errorStatistics.components->radarSensor.reset(new RadarSensor());
        }

        tropoErrorXML = getOptional(tmpElem, "TropoError");
        if (tropoErrorXML)
        {
            //optional
            errorStatistics.components->tropoError.reset(new TropoError());
        }

        ionoErrorXML = getOptional(tmpElem, "IonoError");
        if (ionoErrorXML)
        {
            //optional
            errorStatistics.components->ionoError.reset(new IonoError());
        }
    }

    #define xml_getOptional_reset(xml, pRoot, name) getOptional_reset(xml, #name, (pRoot).name);
    tmpElem = xml_getOptional_reset(errorStatsXML, errorStatistics, Unmodeled); // SIDD 3.0
    if (tmpElem != nullptr)
    {
        auto& Unmodeled = *(errorStatistics.Unmodeled);

        // macro to avoid copy/paste errors
        #define parseDouble_getFirstAndOnly(elem, name, storage) parseDouble(getFirstAndOnly(elem, #name), (storage).name)

        six::getFirstAndOnly(parser(), *tmpElem, Unmodeled.Xrow);
        six::getFirstAndOnly(parser(), *tmpElem, Unmodeled.Ycol);
        six::getFirstAndOnly(parser(), *tmpElem, Unmodeled.XrowYcol);

        auto unmodeledDecorrXML = getOptionalUnmodeledDecorr(*tmpElem, Unmodeled.unmodeledDecorr);
        if (unmodeledDecorrXML != nullptr)
        {
            auto& UnmodeledDecorr = *get(errorStatistics.Unmodeled->unmodeledDecorr);

            auto xrowXML = getFirstAndOnly(unmodeledDecorrXML, "Xrow");
            parseDouble_getFirstAndOnly(xrowXML, CorrCoefZero, UnmodeledDecorr.Xrow);
            parseDouble_getFirstAndOnly(xrowXML, DecorrRate, UnmodeledDecorr.Xrow);

            auto ycolXML = getFirstAndOnly(unmodeledDecorrXML, "Ycol");
            parseDouble_getFirstAndOnly(ycolXML, CorrCoefZero, UnmodeledDecorr.Ycol);
            parseDouble_getFirstAndOnly(ycolXML, DecorrRate, UnmodeledDecorr.Ycol);
        }
    }

    tmpElem = xml_getOptional_reset(errorStatsXML, errorStatistics, Unmodeled); // SIDD 3.0
    if (tmpElem != nullptr)
    {
        auto& Unmodeled = *(errorStatistics.Unmodeled);

        // macro to avoid copy/paste errors
        #define parseDouble_getFirstAndOnly(elem, name, storage) parseDouble(getFirstAndOnly(elem, #name), (storage).name)

        six::getFirstAndOnly(parser(), *tmpElem, Unmodeled.Xrow);
        six::getFirstAndOnly(parser(), *tmpElem, Unmodeled.Ycol);
        six::getFirstAndOnly(parser(), *tmpElem, Unmodeled.XrowYcol);

        auto unmodeledDecorrXML = getOptionalUnmodeledDecorr(*tmpElem, Unmodeled.unmodeledDecorr);
        if (unmodeledDecorrXML != nullptr)
        {
            auto& UnmodeledDecorr = *get(errorStatistics.Unmodeled->unmodeledDecorr);

            auto xrowXML = getFirstAndOnly(unmodeledDecorrXML, "Xrow");
            parseDouble_getFirstAndOnly(xrowXML, CorrCoefZero, UnmodeledDecorr.Xrow);
            parseDouble_getFirstAndOnly(xrowXML, DecorrRate, UnmodeledDecorr.Xrow);

            auto ycolXML = getFirstAndOnly(unmodeledDecorrXML, "Ycol");
            parseDouble_getFirstAndOnly(ycolXML, CorrCoefZero, UnmodeledDecorr.Ycol);
            parseDouble_getFirstAndOnly(ycolXML, DecorrRate, UnmodeledDecorr.Ycol);
        }
    }

    if (posVelErrXML != nullptr)
    {
        errorStatistics.components->posVelError->frame
                = six::toType<FrameType>(getFirstAndOnly(
                        posVelErrXML, "Frame")->getCharacterData());
        parseDouble(getFirstAndOnly(posVelErrXML, "P1"),
                    errorStatistics.components->posVelError->p1);
        parseDouble(getFirstAndOnly(posVelErrXML, "P2"),
                    errorStatistics.components->posVelError->p2);
        parseDouble(getFirstAndOnly(posVelErrXML, "P3"),
                    errorStatistics.components->posVelError->p3);
        parseDouble(getFirstAndOnly(posVelErrXML, "V1"),
                    errorStatistics.components->posVelError->v1);
        parseDouble(getFirstAndOnly(posVelErrXML, "V2"),
                    errorStatistics.components->posVelError->v2);
        parseDouble(getFirstAndOnly(posVelErrXML, "V3"),
                    errorStatistics.components->posVelError->v3);

        tmpElem = getOptional(posVelErrXML, "CorrCoefs");
        if (tmpElem)
        {
            //optional
            errorStatistics.components->posVelError->corrCoefs.reset(
                new CorrCoefs());
            parseDouble(getFirstAndOnly(tmpElem, "P1P2"),
                        errorStatistics.components->posVelError->corrCoefs->p1p2);
            parseDouble(getFirstAndOnly(tmpElem, "P1P3"),
                        errorStatistics.components->posVelError->corrCoefs->p1p3);
            parseDouble(getFirstAndOnly(tmpElem, "P1V1"),
                        errorStatistics.components->posVelError->corrCoefs->p1v1);
            parseDouble(getFirstAndOnly(tmpElem, "P1V2"),
                        errorStatistics.components->posVelError->corrCoefs->p1v2);
            parseDouble(getFirstAndOnly(tmpElem, "P1V3"),
                        errorStatistics.components->posVelError->corrCoefs->p1v3);
            parseDouble(getFirstAndOnly(tmpElem, "P2P3"),
                        errorStatistics.components->posVelError->corrCoefs->p2p3);
            parseDouble(getFirstAndOnly(tmpElem, "P2V1"),
                        errorStatistics.components->posVelError->corrCoefs->p2v1);
            parseDouble(getFirstAndOnly(tmpElem, "P2V2"),
                        errorStatistics.components->posVelError->corrCoefs->p2v2);
            parseDouble(getFirstAndOnly(tmpElem, "P2V3"),
                        errorStatistics.components->posVelError->corrCoefs->p2v3);
            parseDouble(getFirstAndOnly(tmpElem, "P3V1"),
                        errorStatistics.components->posVelError->corrCoefs->p3v1);
            parseDouble(getFirstAndOnly(tmpElem, "P3V2"),
                        errorStatistics.components->posVelError->corrCoefs->p3v2);
            parseDouble(getFirstAndOnly(tmpElem, "P3V3"),
                        errorStatistics.components->posVelError->corrCoefs->p3v3);
            parseDouble(getFirstAndOnly(tmpElem, "V1V2"),
                        errorStatistics.components->posVelError->corrCoefs->v1v2);
            parseDouble(getFirstAndOnly(tmpElem, "V1V3"),
                        errorStatistics.components->posVelError->corrCoefs->v1v3);
            parseDouble(getFirstAndOnly(tmpElem, "V2V3"),
                        errorStatistics.components->posVelError->corrCoefs->v2v3);
        }

        parseOptionalDecorrType(posVelErrXML, "PositionDecorr",
            errorStatistics.components->posVelError->positionDecorr);
    }

    if (radarSensorXML != nullptr)
    {
        parseDouble(getFirstAndOnly(radarSensorXML, "RangeBias"),
                    errorStatistics.components->radarSensor->rangeBias);

        parseOptionalDouble(radarSensorXML, "ClockFreqSF",
            errorStatistics.components->radarSensor->clockFreqSF);

        parseOptionalDouble(radarSensorXML, "TransmitFreqSF",
            errorStatistics.components->radarSensor->transmitFreqSF);

        parseOptionalDecorrType(radarSensorXML, "RangeBiasDecorr",
            errorStatistics.components->radarSensor->rangeBiasDecorr);
    }

    if (tropoErrorXML != nullptr)
    {
        parseOptionalDouble(tropoErrorXML, "TropoRangeVertical",
            errorStatistics.components->tropoError ->tropoRangeVertical);
        parseOptionalDouble(tropoErrorXML, "TropoRangeSlant",
            errorStatistics.components->tropoError->tropoRangeSlant);

        parseOptionalDecorrType(tropoErrorXML, "TropoRangeDecorr",
            errorStatistics.components->tropoError->tropoRangeDecorr);
    }

    if (ionoErrorXML != nullptr)
    {
        parseOptionalDouble(ionoErrorXML, "IonoRangeVertical",
            errorStatistics.components->ionoError->ionoRangeVertical);
        parseOptionalDouble(ionoErrorXML, "IonoRangeRateVertical",
            errorStatistics.components->ionoError ->ionoRangeRateVertical);

        parseDouble(getFirstAndOnly(ionoErrorXML, "IonoRgRgRateCC"),
                    errorStatistics.components->ionoError->ionoRgRgRateCC);

        parseOptionalDecorrType(ionoErrorXML, "IonoRangeVertDecorr",
            errorStatistics.components->ionoError->ionoRangeVertDecorr);
    }

    tmpElem = getOptional(errorStatsXML, "AdditionalParms");
    if (tmpElem)
    {
        //optional
        parseParameters(tmpElem, "Parameter", errorStatistics.additionalParameters);
    }
}
void SICommonXMLParser::parseErrorStatisticsFromXML(
    const xml::lite::Element* errorStatsXML,
    ErrorStatistics* errorStatistics) const
{
    parseErrorStatisticsFromXML(*errorStatsXML, *errorStatistics);
}

void SICommonXMLParser::parseFootprint(const xml::lite::Element& footprint,
                                       const std::string& cornerName,
                                       LatLonCorners &corners) const
{
    std::vector<XMLElem> vertices;
    footprint.getElementsByTagName(cornerName, vertices);

    std::set<size_t> indices;
    for (size_t ii = 0; ii < vertices.size(); ++ii)
    {
        const xml::lite::Element* vertex(vertices[ii]);

        // Check the index attr to know which corner it is
        // This is 1-based
        const size_t idx =
            str::toType<size_t>(vertex->getAttributes().getValue("index"));
        indices.insert(idx);

        parseLatLon(vertices[ii], corners.getCorner(idx - 1));
    }

    // We expect to get exactly NUM_CORNERS unique lat/lon's
    if (indices.size() != LatLonCorners::NUM_CORNERS)
    {
        throw except::Exception(Ctxt("Didn't get all expected corners"));
    }
}
void SICommonXMLParser::parseFootprint(const xml::lite::Element* footprint,
    const std::string& cornerName,
    LatLonCorners& corners) const
{
    parseFootprint(*footprint, cornerName, corners);
}

void SICommonXMLParser::parseFootprint(const xml::lite::Element* footprint,
                                       const std::string& cornerName,
                                       LatLonAltCorners& corners) const
{
    std::vector<XMLElem> vertices;
    footprint->getElementsByTagName(cornerName, vertices);

    std::set<size_t> indices;
    for (size_t ii = 0; ii < vertices.size(); ++ii)
    {
        const xml::lite::Element* vertex(vertices[ii]);

        // Check the index attr to know which corner it is
        // This is 1-based
        const size_t idx =
            str::toType<size_t>(vertex->getAttributes().getValue("index"));
        indices.insert(idx);

        parseLatLonAlt(vertices[ii], corners.getCorner(idx - 1));
    }

    // We expect to get exactly NUM_CORNERS unique lat/lon's
    if (indices.size() != LatLonCorners::NUM_CORNERS)
    {
        throw except::Exception(Ctxt("Didn't get all expected corners"));
    }
}

void SICommonXMLParser::parseCollectionInformationFromXML(
    const xml::lite::Element* collectionInfoXML,
    CollectionInformation *collInfo) const
{
    assert(collectionInfoXML != nullptr);

    parseString(getFirstAndOnly(collectionInfoXML, "CollectorName"),
                collInfo->collectorName);

    XMLElem element = getOptional(collectionInfoXML, "IlluminatorName");
    if (element)
    {
        parseString(element, collInfo->illuminatorName);
    }

    element = getOptional(collectionInfoXML, "CoreName");
    if (element)
    {
        parseString(element, collInfo->coreName);
    }

    element = getOptional(collectionInfoXML, "CollectType");
    if (element)
    {
        collInfo->collectType
                = six::toType<six::CollectType>(element->getCharacterData());
    }

    XMLElem radarModeXML = getFirstAndOnly(collectionInfoXML, "RadarMode");

    collInfo->radarMode
            = six::toType<RadarModeType>(getFirstAndOnly(radarModeXML,
                                         "ModeType")->getCharacterData());

    element = getOptional(radarModeXML, "ModeID");
    if (element)
    {
        parseString(element, collInfo->radarModeID);
    }

    const auto& classificationXML = getFirstAndOnly(*collectionInfoXML, "Classification");
    // For "new" XML processing (storing the encoding) we'll have a UTF-8 value.
    // This is important as it could be French "NON CLASSIFIÉ / UNCLASSIFIED"
    std::u8string classification_u8;
    if (parseString(classificationXML, classification_u8))
    {
        collInfo->setClassificationLevel(str::EncodedString(classification_u8));
    }
    else
    {
        std::string classification;
        parseString(classificationXML, classification);
        collInfo->setClassificationLevel(classification);
    }

    std::vector < XMLElem > countryCodeXML;
    collectionInfoXML->getElementsByTagName("CountryCode", countryCodeXML);

    //optional
    for (std::vector<XMLElem>::const_iterator it = countryCodeXML.begin(); it
            != countryCodeXML.end(); ++it)
    {
        std::string cc;
        parseString(*it, cc);
        collInfo->countryCodes.push_back(cc);
    }

    //optional
    parseParameters(collectionInfoXML, "Parameter", collInfo->parameters);
}

XMLElem SICommonXMLParser::convertCollectionInformationToXML(
    const CollectionInformation *collInfo,
    XMLElem parent) const
{
    assert(collInfo != nullptr);

    XMLElem collInfoXML = newElement("CollectionInfo", parent);

    const std::string si = getSICommonURI();

    createString("CollectorName", si, collInfo->collectorName, collInfoXML);
    if (!collInfo->illuminatorName.empty())
    {
        createString("IlluminatorName", si, collInfo->illuminatorName,
                     collInfoXML);
    }
    createString("CoreName", si, collInfo->coreName, collInfoXML);
    if (!Init::isUndefined(collInfo->collectType))
    {
        createString("CollectType", si, collInfo->collectType.toString(), collInfoXML);
    }

    XMLElem radarModeXML = newElement("RadarMode", si, collInfoXML);
    createSixString("ModeType", si, collInfo->radarMode, radarModeXML);
    if (!collInfo->radarModeID.empty())
    {
        createString("ModeID", si, collInfo->radarModeID, radarModeXML);
    }

    createString("Classification", si, collInfo->getClassificationLevel(),
                 collInfoXML);

    for (std::vector<std::string>::const_iterator it =
            collInfo->countryCodes.begin(); it != collInfo->countryCodes.end();
            ++it)
    {
        createString("CountryCode", si, *it, collInfoXML);
    }
    addParameters("Parameter", si, collInfo->parameters, collInfoXML);
    return collInfoXML;
}
}

