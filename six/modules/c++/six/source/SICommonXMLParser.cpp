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
#include <set>

#include <str/Convert.h>
#include <six/Utilities.h>
#include <six/CollectionInformation.h>
#include <six/SICommonXMLParser.h>
#include <six/ParameterCollection.h>

namespace
{
typedef xml::lite::Element* XMLElem;
}

namespace six
{
SICommonXMLParser::SICommonXMLParser(const std::string& defaultURI,
                                     bool addClassAttributes,
                                     const std::string& siCommonURI,
                                     logging::Logger* log,
                                     bool ownLog) :
    XMLParser(defaultURI, addClassAttributes, log, ownLog),
    mSICommonURI(siCommonURI)
{
}

void SICommonXMLParser::parseVector2D(XMLElem vecXML, Vector2& vec) const
{
    parseDouble(getFirstAndOnly(vecXML, "X"), vec[0]);
    parseDouble(getFirstAndOnly(vecXML, "Y"), vec[1]);
}

void SICommonXMLParser::parseVector3D(XMLElem vecXML, Vector3& vec) const
{
    parseDouble(getFirstAndOnly(vecXML, "X"), vec[0]);
    parseDouble(getFirstAndOnly(vecXML, "Y"), vec[1]);
    parseDouble(getFirstAndOnly(vecXML, "Z"), vec[2]);
}

void SICommonXMLParser::parseLatLonAlt(XMLElem llaXML, LatLonAlt& lla) const
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
    size_t order = poly1D.order();
    XMLElem poly1DXML = newElement(name, uri, parent);
    setAttribute(poly1DXML, "order1", six::toString(order));

    for (size_t ii = 0; ii <= order; ++ii)
    {
        XMLElem coefXML = createDouble("Coef", getSICommonURI(), poly1D[ii],
                                       poly1DXML);
        setAttribute(coefXML, "exponent1", six::toString(ii));
    }
    return poly1DXML;
}

XMLElem SICommonXMLParser::createPoly1D(const std::string& name,
        const Poly1D& poly1D, XMLElem parent) const
{
    return createPoly1D(name, getDefaultURI(), poly1D, parent);
}

void SICommonXMLParser::parsePolyXYZ(XMLElem polyXML, PolyXYZ& polyXYZ) const
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

void SICommonXMLParser::parsePoly(XMLElem polyXML,
                                  size_t xyzIdx,
                                  PolyXYZ& polyXYZ) const
{
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
                    "Order " + str::toString(orderIdx) + " is out of bounds"));
        }
        parseDouble(coeffsXML[ii], polyXYZ[orderIdx][xyzIdx]);
    }
}

XMLElem SICommonXMLParser::createPolyXYZ(const std::string& name,
        const PolyXYZ& polyXYZ, XMLElem parent) const
{
    size_t order = polyXYZ.order();
    XMLElem polyXML = newElement(name, getDefaultURI(), parent);

    XMLElem xXML = newElement("X", getSICommonURI(), polyXML);
    XMLElem yXML = newElement("Y", getSICommonURI(), polyXML);
    XMLElem zXML = newElement("Z", getSICommonURI(), polyXML);

    setAttribute(xXML, "order1", six::toString(order));
    setAttribute(yXML, "order1", six::toString(order));
    setAttribute(zXML, "order1", six::toString(order));

    for (size_t ii = 0; ii <= order; ++ii)
    {
        Vector3 v3 = polyXYZ[ii];
        XMLElem xCoefXML = createDouble("Coef", getSICommonURI(), v3[0], xXML);
        XMLElem yCoefXML = createDouble("Coef", getSICommonURI(), v3[1], yXML);
        XMLElem zCoefXML = createDouble("Coef", getSICommonURI(), v3[2], zXML);

        setAttribute(xCoefXML, "exponent1", six::toString(ii));
        setAttribute(yCoefXML, "exponent1", six::toString(ii));
        setAttribute(zCoefXML, "exponent1", six::toString(ii));
    }
    return polyXML;
}

void SICommonXMLParser::parsePoly1D(XMLElem polyXML, Poly1D& poly1D) const
{
    int order1 = str::toType<int>(polyXML->getAttributes().getValue("order1"));
    Poly1D p1D(order1);

    std::vector < XMLElem > coeffsXML;
    polyXML->getElementsByTagName("Coef", coeffsXML);

    int exp1;
    for (size_t ii = 0, size = coeffsXML.size(); ii < size; ++ii)
    {
        XMLElem element = coeffsXML[ii];
        exp1 = str::toType<int>(element->getAttributes().getValue("exponent1"));
        parseDouble(element, p1D[exp1]);
    }
    poly1D = p1D;
}

void SICommonXMLParser::parsePoly2D(XMLElem polyXML, Poly2D& poly2D) const
{
    int order1 = str::toType<int>(polyXML->getAttributes().getValue("order1"));
    int order2 = str::toType<int>(polyXML->getAttributes().getValue("order2"));
    Poly2D p2D(order1, order2);

    std::vector < XMLElem > coeffsXML;
    polyXML->getElementsByTagName("Coef", coeffsXML);

    int exp1, exp2;
    for (size_t ii = 0, size = coeffsXML.size(); ii < size; ++ii)
    {
        XMLElem element = coeffsXML[ii];
        exp1 = str::toType<int>(element->getAttributes().getValue("exponent1"));
        exp2 = str::toType<int>(element->getAttributes().getValue("exponent2"));
        parseDouble(element, p2D[exp1][exp2]);
    }
    poly2D = p2D;
}

XMLElem SICommonXMLParser::createPoly2D(const std::string& name,
        const std::string& uri, const Poly2D& poly2D, XMLElem parent) const
{
    xml::lite::AttributeNode node;
    XMLElem poly2DXML = newElement(name, uri, parent);
    setAttribute(poly2DXML, "order1", six::toString(poly2D.orderX()));
    setAttribute(poly2DXML, "order2", six::toString(poly2D.orderY()));

    for (size_t ii = 0; ii <= poly2D.orderX(); ii++)
    {
        for (size_t jj = 0; jj <= poly2D.orderY(); jj++)
        {
            XMLElem coefXML = createDouble("Coef", getSICommonURI(),
                                           poly2D[ii][jj], poly2DXML);
            setAttribute(coefXML, "exponent1", six::toString(ii));
            setAttribute(coefXML, "exponent2", six::toString(jj));
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
        std::complex<double> c, XMLElem parent) const
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
    createInt(rowName, getSICommonURI(), static_cast<int>(value.row), e);
    createInt(colName, getSICommonURI(), static_cast<int>(value.col), e);
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
        const LatLon& value,
        XMLElem parent) const
{
    XMLElem e = newElement(name, getDefaultURI(), parent);
    createDouble("Lat", getSICommonURI(), value.getLat(), e);
    createDouble("Lon", getSICommonURI(), value.getLon(), e);
    return e;
}

XMLElem SICommonXMLParser::createLatLonAlt(const std::string& name,
        const LatLonAlt& value, XMLElem parent) const
{
    XMLElem e = createLatLon(name, value, parent);
    createDouble("HAE", getSICommonURI(), value.getAlt(), e);
    return e;
}

void SICommonXMLParser::parseParameter(XMLElem element, Parameter& p) const
{
    p.setName(element->getAttributes().getValue("name"));
    p.setValue<std::string>(element->getCharacterData());
}

void SICommonXMLParser::parseParameters(XMLElem paramXML,
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
    if (!Init::isUndefined<double>(decorrType.corrCoefZero)
            && !Init::isUndefined<double>(decorrType.decorrRate))
    {
        XMLElem decorrXML = newElement(name, uri, parent);
        createDouble("CorrCoefZero", uri, decorrType.corrCoefZero, decorrXML);
        createDouble("DecorrRate", uri, decorrType.decorrRate, decorrXML);
    }
}

void SICommonXMLParser::parseDecorrType(XMLElem decorrXML,
                                        DecorrType& decorrType) const
{
    parseDouble(getFirstAndOnly(decorrXML, "CorrCoefZero"),
                decorrType.corrCoefZero);
    parseDouble(getFirstAndOnly(decorrXML, "DecorrRate"),
                decorrType.decorrRate);
}

void SICommonXMLParser::parseLatLon(XMLElem parent, LatLon& ll) const
{
    double lat, lon;

    parseDouble(getFirstAndOnly(parent, "Lat"), lat);
    parseDouble(getFirstAndOnly(parent, "Lon"), lon);

    ll.setLat(lat);
    ll.setLon(lon);
}

void SICommonXMLParser::parseLatLons(XMLElem pointsXML,
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

void SICommonXMLParser::parseRangeAzimuth(XMLElem parent,
                                          types::RgAz<double>& value) const
{
    parseDouble(getFirstAndOnly(parent, "Range"), value.rg);
    parseDouble(getFirstAndOnly(parent, "Azimuth"), value.az);
}

void SICommonXMLParser::parseRowColDouble(
        XMLElem parent,
        const std::string& rowName,
        const std::string& colName,
        RowColDouble& rc) const
{
    parseDouble(getFirstAndOnly(parent, rowName), rc.row);
    parseDouble(getFirstAndOnly(parent, colName), rc.col);
}

void SICommonXMLParser::parseRowColLatLon(XMLElem parent,
                                          RowColLatLon& rc) const
{
    parseLatLon(getFirstAndOnly(parent, "Row"), rc.row);
    parseLatLon(getFirstAndOnly(parent, "Col"), rc.col);
}

void SICommonXMLParser::parseRowColDouble(XMLElem parent,
                                          RowColDouble& rc) const
{
    parseRowColDouble(parent, "Row", "Col", rc);
}

void SICommonXMLParser::parseRowColInt(
        XMLElem parent,
        const std::string& rowName,
        const std::string& colName,
        RowColInt& rc) const
{
    parseInt(getFirstAndOnly(parent, rowName), rc.row);
    parseInt(getFirstAndOnly(parent, colName), rc.col);
}

void SICommonXMLParser::parseRowColInt(XMLElem parent,
                                       RowColInt& rc) const
{
    parseRowColInt(parent, "Row", "Col", rc);
}

void SICommonXMLParser::parseRowColInts(
        XMLElem pointsXML,
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

            createString("Frame", getSICommonURI(),
                         six::toString(posVelError->frame), posVelErrXML);
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

            if (!Init::isUndefined<double>(radarSensor->clockFreqSF))
            {
                createDouble("ClockFreqSF", getSICommonURI(),
                             radarSensor->clockFreqSF, radarSensorXML);
            }
            if (!Init::isUndefined<double>(radarSensor->transmitFreqSF))
            {
                createDouble("TransmitFreqSF", getSICommonURI(),
                             radarSensor->transmitFreqSF, radarSensorXML);
            }
            addDecorrType("RangeBiasDecorr", getSICommonURI(),
                          radarSensor->rangeBiasDecorr, radarSensorXML);
        }
        if (tropoError)
        {
            XMLElem tropoErrXML = newElement("TropoError",
                                             getSICommonURI(),
                                             componentsXML);

            if (!Init::isUndefined<double>(tropoError->tropoRangeVertical))
            {
                createDouble("TropoRangeVertical", getSICommonURI(),
                             tropoError->tropoRangeVertical, tropoErrXML);
            }
            if (!Init::isUndefined<double>(tropoError->tropoRangeSlant))
            {
                createDouble("TropoRangeSlant", getSICommonURI(),
                             tropoError->tropoRangeSlant, tropoErrXML);
            }

            addDecorrType("TropoRangeDecorr", getSICommonURI(),
                          tropoError->tropoRangeDecorr, tropoErrXML);
        }
        if (ionoError)
        {
            XMLElem ionoErrXML = newElement("IonoError",
                                            getSICommonURI(),
                                            componentsXML);

            if (!Init::isUndefined<double>(ionoError->ionoRangeVertical))
            {
                createDouble("IonoRangeVertical", getSICommonURI(),
                             ionoError->ionoRangeVertical, ionoErrXML);
            }
            if (!Init::isUndefined<double>(ionoError->ionoRangeRateVertical))
            {
                createDouble("IonoRangeRateVertical", getSICommonURI(),
                             ionoError->ionoRangeRateVertical, ionoErrXML);
            }

            createDouble("IonoRgRgRateCC", getSICommonURI(),
                         ionoError->ionoRgRgRateCC, ionoErrXML);

            addDecorrType("IonoRangeVertDecorr", getSICommonURI(),
                          ionoError->ionoRangeVertDecorr, ionoErrXML);
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

void SICommonXMLParser::parseErrorStatisticsFromXML(
        const XMLElem errorStatsXML,
        ErrorStatistics* errorStatistics) const
{
    XMLElem tmpElem = NULL;
    //optional

    //! version specific CompositeSCP parsing
    parseCompositeSCPFromXML(errorStatsXML, errorStatistics);

    XMLElem posVelErrXML = NULL;
    XMLElem radarSensorXML = NULL;
    XMLElem tropoErrorXML = NULL;
    XMLElem ionoErrorXML = NULL;

    tmpElem = getOptional(errorStatsXML, "Components");
    if (tmpElem)
    {
        //optional
        errorStatistics->components.reset(new Components());

        posVelErrXML = getOptional(tmpElem, "PosVelErr");
        if (posVelErrXML)
        {
            //optional
            errorStatistics->components->posVelError.reset(new PosVelError());
        }

        radarSensorXML = getOptional(tmpElem, "RadarSensor");
        if (radarSensorXML)
        {
            //optional
            errorStatistics->components->radarSensor.reset(new RadarSensor());
        }

        tropoErrorXML = getOptional(tmpElem, "TropoError");
        if (tropoErrorXML)
        {
            //optional
            errorStatistics->components->tropoError.reset(new TropoError());
        }

        ionoErrorXML = getOptional(tmpElem, "IonoError");
        if (ionoErrorXML)
        {
            //optional
            errorStatistics->components->ionoError.reset(new IonoError());
        }
    }

    if (posVelErrXML != NULL)
    {
        errorStatistics->components->posVelError->frame
                = six::toType<FrameType>(getFirstAndOnly(
                        posVelErrXML, "Frame")->getCharacterData());
        parseDouble(getFirstAndOnly(posVelErrXML, "P1"),
                    errorStatistics->components->posVelError->p1);
        parseDouble(getFirstAndOnly(posVelErrXML, "P2"),
                    errorStatistics->components->posVelError->p2);
        parseDouble(getFirstAndOnly(posVelErrXML, "P3"),
                    errorStatistics->components->posVelError->p3);
        parseDouble(getFirstAndOnly(posVelErrXML, "V1"),
                    errorStatistics->components->posVelError->v1);
        parseDouble(getFirstAndOnly(posVelErrXML, "V2"),
                    errorStatistics->components->posVelError->v2);
        parseDouble(getFirstAndOnly(posVelErrXML, "V3"),
                    errorStatistics->components->posVelError->v3);

        tmpElem = getOptional(posVelErrXML, "CorrCoefs");
        if (tmpElem)
        {
            //optional
            errorStatistics->components->posVelError->corrCoefs.reset(
                new CorrCoefs());
            parseDouble(getFirstAndOnly(tmpElem, "P1P2"),
                        errorStatistics->components->posVelError->corrCoefs->p1p2);
            parseDouble(getFirstAndOnly(tmpElem, "P1P3"),
                        errorStatistics->components->posVelError->corrCoefs->p1p3);
            parseDouble(getFirstAndOnly(tmpElem, "P1V1"),
                        errorStatistics->components->posVelError->corrCoefs->p1v1);
            parseDouble(getFirstAndOnly(tmpElem, "P1V2"),
                        errorStatistics->components->posVelError->corrCoefs->p1v2);
            parseDouble(getFirstAndOnly(tmpElem, "P1V3"),
                        errorStatistics->components->posVelError->corrCoefs->p1v3);
            parseDouble(getFirstAndOnly(tmpElem, "P2P3"),
                        errorStatistics->components->posVelError->corrCoefs->p2p3);
            parseDouble(getFirstAndOnly(tmpElem, "P2V1"),
                        errorStatistics->components->posVelError->corrCoefs->p2v1);
            parseDouble(getFirstAndOnly(tmpElem, "P2V2"),
                        errorStatistics->components->posVelError->corrCoefs->p2v2);
            parseDouble(getFirstAndOnly(tmpElem, "P2V3"),
                        errorStatistics->components->posVelError->corrCoefs->p2v3);
            parseDouble(getFirstAndOnly(tmpElem, "P3V1"),
                        errorStatistics->components->posVelError->corrCoefs->p3v1);
            parseDouble(getFirstAndOnly(tmpElem, "P3V2"),
                        errorStatistics->components->posVelError->corrCoefs->p3v2);
            parseDouble(getFirstAndOnly(tmpElem, "P3V3"),
                        errorStatistics->components->posVelError->corrCoefs->p3v3);
            parseDouble(getFirstAndOnly(tmpElem, "V1V2"),
                        errorStatistics->components->posVelError->corrCoefs->v1v2);
            parseDouble(getFirstAndOnly(tmpElem, "V1V3"),
                        errorStatistics->components->posVelError->corrCoefs->v1v3);
            parseDouble(getFirstAndOnly(tmpElem, "V2V3"),
                        errorStatistics->components->posVelError->corrCoefs->v2v3);
        }

        tmpElem = getOptional(posVelErrXML, "PositionDecorr");
        if (tmpElem)
        {
            //optional
            parseDecorrType(tmpElem,
                errorStatistics->components->posVelError->positionDecorr);
        }
    }

    if (radarSensorXML != NULL)
    {
        parseDouble(getFirstAndOnly(radarSensorXML, "RangeBias"),
                    errorStatistics->components->radarSensor->rangeBias);

        tmpElem = getOptional(radarSensorXML, "ClockFreqSF");
        if (tmpElem)
        {
            //optional
            parseDouble(tmpElem,
                errorStatistics->components->radarSensor->clockFreqSF);
        }

        tmpElem = getOptional(radarSensorXML, "TransmitFreqSF");
        if (tmpElem)
        {
            //optional
            parseDouble(tmpElem,
                errorStatistics->components->radarSensor->transmitFreqSF);
        }

        tmpElem = getOptional(radarSensorXML, "RangeBiasDecorr");
        if (tmpElem)
        {
            parseDecorrType(tmpElem,
                errorStatistics->components->radarSensor->rangeBiasDecorr);
        }
    }

    if (tropoErrorXML != NULL)
    {
        tmpElem = getOptional(tropoErrorXML, "TropoRangeVertical");
        if (tmpElem)
        {
            //optional
            parseDouble(tmpElem,
                errorStatistics->components->tropoError ->tropoRangeVertical);
        }

        tmpElem = getOptional(tropoErrorXML, "TropoRangeSlant");
        if (tmpElem)
        {
            //optional
            parseDouble(tmpElem,
                errorStatistics->components->tropoError->tropoRangeSlant);
        }

        tmpElem = getOptional(tropoErrorXML, "TropoRangeDecorr");
        if (tmpElem)
        {
            parseDecorrType(tmpElem,
                errorStatistics->components->tropoError->tropoRangeDecorr);
        }
    }

    if (ionoErrorXML != NULL)
    {
        tmpElem = getOptional(ionoErrorXML, "IonoRangeVertical");
        if (tmpElem)
        {
            //optional
            parseDouble(tmpElem,
                errorStatistics->components->ionoError->ionoRangeVertical);
        }

        tmpElem = getOptional(ionoErrorXML, "IonoRangeRateVertical");
        if (tmpElem)
        {
            //optional
            parseDouble(tmpElem,
                errorStatistics->components->ionoError ->ionoRangeRateVertical);
        }

        parseDouble(getFirstAndOnly(ionoErrorXML, "IonoRgRgRateCC"),
                    errorStatistics->components->ionoError->ionoRgRgRateCC);

        tmpElem = getOptional(ionoErrorXML, "IonoRangeVertDecorr");
        if (tmpElem)
        {
            parseDecorrType(tmpElem,
                errorStatistics->components->ionoError->ionoRangeVertDecorr);
        }
    }

    tmpElem = getOptional(errorStatsXML, "AdditionalParms");
    if (tmpElem)
    {
        //optional
        parseParameters(tmpElem, "Parameter",
                        errorStatistics->additionalParameters);
    }
}

void SICommonXMLParser::parseFootprint(XMLElem footprint,
                                       const std::string& cornerName,
                                       LatLonCorners &corners) const
{
    std::vector<XMLElem> vertices;
    footprint->getElementsByTagName(cornerName, vertices);

    std::set<size_t> indices;
    for (size_t ii = 0; ii < vertices.size(); ++ii)
    {
        const XMLElem vertex(vertices[ii]);

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

void SICommonXMLParser::parseFootprint(XMLElem footprint,
                                       const std::string& cornerName,
                                       LatLonAltCorners& corners) const
{
    std::vector<XMLElem> vertices;
    footprint->getElementsByTagName(cornerName, vertices);

    std::set<size_t> indices;
    for (size_t ii = 0; ii < vertices.size(); ++ii)
    {
        const XMLElem vertex(vertices[ii]);

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

void SICommonXMLParser::parseMatchInformationFromXML(
    const XMLElem matchInfoXML,
    MatchInformation* matchInfo) const
{
    size_t numMatchTypes = 0;
    parseInt(getFirstAndOnly(matchInfoXML, "NumMatchTypes"), numMatchTypes);
    if (numMatchTypes == 0)
    {
        throw except::Exception(Ctxt("NumMatchTypes cannot be zero"));
    }

    std::vector < XMLElem > typesXML;
    matchInfoXML->getElementsByTagName("MatchType", typesXML);

    //! validate the numMatchTypes
    if (typesXML.size() != numMatchTypes)
    {
        throw except::Exception(
            Ctxt("NumMatchTypes does not match number of MatchType fields"));
    }

    matchInfo->types.reserve(typesXML.size());
    for (size_t ii = 0; ii < typesXML.size(); ii++)
    {
        // The MatchInformation object was given a MatchType when
        // it was instantiated.  The first time through, just populate it.
        if (ii != 0)
        {
            matchInfo->types.push_back(
                mem::ScopedCopyablePtr<MatchType>(new MatchType()));
        }
        MatchType* type = matchInfo->types[ii].get();

        parseString(getFirstAndOnly(typesXML[ii], "TypeID"), type->typeID);

        XMLElem curIndexElem = getOptional(typesXML[ii], "CurrentIndex");
        if (curIndexElem)
        {
            //optional
            parseInt(curIndexElem, type->currentIndex);
        }

        int numMatchCollections = 0;
        parseInt(getFirstAndOnly(typesXML[ii], "NumMatchCollections"),
                 numMatchCollections);

        std::vector < XMLElem > matchCollectionsXML;
        typesXML[ii]->getElementsByTagName("MatchCollection", matchCollectionsXML);

        //! validate the numMatchTypes
        if (matchCollectionsXML.size() !=
            static_cast<size_t>(numMatchCollections))
        {
            throw except::Exception(
                Ctxt("NumMatchCollections does not match number of " \
                     "MatchCollect fields"));
        }

        // Need to make sure this is resized properly - at MatchType
        // construction time, matchCollects is initialized to size 1, but in
        // SICD 1.1 this entire block may be missing.
        type->matchCollects.resize(matchCollectionsXML.size());
        for (size_t jj = 0; jj < matchCollectionsXML.size(); jj++)
        {
            MatchCollect& collect(type->matchCollects[jj]);

            parseString(getFirstAndOnly(
                matchCollectionsXML[jj], "CoreName"), collect.coreName);

            XMLElem matchIndexXML =
                getOptional(matchCollectionsXML[jj], "MatchIndex");
            if (matchIndexXML)
            {
                parseInt(matchIndexXML, collect.matchIndex);
            }

            parseParameters(
                matchCollectionsXML[jj], "Parameter", collect.parameters);
        }
    }
}

XMLElem SICommonXMLParser::convertMatchInformationToXML(
    const MatchInformation* matchInfo,
    XMLElem parent) const
{
    XMLElem matchInfoXML = newElement("MatchInfo", parent);

    createInt("NumMatchTypes",
              static_cast<int>(matchInfo->types.size()),
              matchInfoXML);

    for (size_t ii = 0; ii < matchInfo->types.size(); ++ii)
    {
        const MatchType* mt = matchInfo->types[ii].get();
        XMLElem mtXML = newElement("MatchType", matchInfoXML);
        setAttribute(mtXML, "index", str::toString(ii + 1));

        createString("TypeID", mt->typeID, mtXML);
        createInt("CurrentIndex", mt->currentIndex, mtXML);
        createInt("NumMatchCollections",
                  static_cast<int>(mt->matchCollects.size()), mtXML);

        for (size_t jj = 0; jj < mt->matchCollects.size(); ++jj)
        {
            XMLElem mcXML = newElement("MatchCollection", mtXML);
            setAttribute(mcXML, "index", str::toString(jj + 1));

            createString("CoreName", mt->matchCollects[jj].coreName, mcXML);
            createInt("MatchIndex", mt->matchCollects[jj].matchIndex, mcXML);
            addParameters("Parameter", mt->matchCollects[jj].parameters, mcXML);
        }
    }

    return matchInfoXML;
}

XMLElem SICommonXMLParser::convertCollectionInformationToXML(
    const CollectionInformation *collInfo,
    XMLElem parent) const
{
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
        createString("CollectType", si,
                     six::toString<six::CollectType>(collInfo->collectType),
                     collInfoXML);
    }

    XMLElem radarModeXML = newElement("RadarMode", si, collInfoXML);
    createString("ModeType", si, six::toString(collInfo->radarMode),
                 radarModeXML);
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

void SICommonXMLParser::parseCollectionInformationFromXML(
    const XMLElem collectionInfoXML,
    CollectionInformation *collInfo) const
{
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

    std::string classification;
    parseString(getFirstAndOnly(collectionInfoXML, "Classification"),
                classification);
    collInfo->setClassificationLevel(classification);

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

}

