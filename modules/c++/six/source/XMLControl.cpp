/* =========================================================================
 * This file is part of six-c++ 
 * =========================================================================
 * 
 * (C) Copyright 2004 - 2009, General Dynamics - Advanced Information Systems
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

#include "six/XMLControl.h"
#include "six/Types.h"
#include "six/Utilities.h"
#include <import/str.h>

using namespace six;

typedef xml::lite::Element* XMLElem;

XMLControl::XMLControl(logging::Logger* log, bool ownLog) :
    mLog(NULL), mOwnLog(false)
{
    setLogger(log, ownLog);
}

XMLControl::~XMLControl()
{
    if (mLog && mOwnLog)
        delete mLog;
}

void XMLControl::setLogger(logging::Logger* log, bool own)
{
    if (mLog && mOwnLog && log != mLog)
        delete mLog;
    mLog = log ? log : new logging::NullLogger;
    mOwnLog = log ? own : true;
}

XMLElem XMLControl::newElement(const std::string& name, XMLElem parent)
{
    return newElement(name, getDefaultURI(), parent);
}

XMLElem XMLControl::newElement(const std::string& name,
        const std::string& uri, XMLElem parent)
{
    return newElement(name, uri, "", parent);
}

XMLElem XMLControl::newElement(const std::string& name,
        const std::string& uri, const std::string& characterData,
        XMLElem parent)
{
    XMLElem elem = new xml::lite::Element(name, uri, characterData);
    if (parent)
        parent->addChild(elem);
    return elem;
}

void XMLControl::parseVector3D(XMLElem vecXML, Vector3& vec)
{
    parseDouble(getFirstAndOnly(vecXML, "X"), vec[0]);
    parseDouble(getFirstAndOnly(vecXML, "Y"), vec[1]);
    parseDouble(getFirstAndOnly(vecXML, "Z"), vec[2]);
}

void XMLControl::parseLatLonAlt(XMLElem llaXML, LatLonAlt& lla)
{
    double lat, lon, alt;

    parseDouble(getFirstAndOnly(llaXML, "Lat"), lat);
    parseDouble(getFirstAndOnly(llaXML, "Lon"), lon);
    parseDouble(getFirstAndOnly(llaXML, "HAE"), alt);

    lla.setLat(lat);
    lla.setLon(lon);
    lla.setAlt(alt);
}

XMLElem XMLControl::createVector3D(const std::string& name, Vector3 p,
        XMLElem parent)
{
    XMLElem e = newElement(name, getDefaultURI(), parent);
    createDouble("X", getSICommonURI(), p[0], e);
    createDouble("Y", getSICommonURI(), p[1], e);
    createDouble("Z", getSICommonURI(), p[2], e);
    return e;
}

XMLElem XMLControl::createPoly1D(const std::string& name,
        const std::string& uri, const Poly1D& poly1D, XMLElem parent)
{
    int order = poly1D.order();
    XMLElem poly1DXML = newElement(name, uri, parent);
    setAttribute(poly1DXML, "order1", six::toString(order));

    for (int i = 0; i <= order; ++i)
    {
        XMLElem coefXML = createDouble("Coef", getSICommonURI(), poly1D[i],
                                       poly1DXML);
        setAttribute(coefXML, "exponent1", six::toString(i));
    }
    return poly1DXML;
}

XMLElem XMLControl::createPoly1D(const std::string& name,
        const Poly1D& poly1D, XMLElem parent)
{
    return createPoly1D(name, getDefaultURI(), poly1D, parent);
}

void XMLControl::parsePolyXYZ(XMLElem polyXML, PolyXYZ& polyXYZ)
{
    XMLElem xXML = getFirstAndOnly(polyXML, "X");
    XMLElem yXML = getFirstAndOnly(polyXML, "Y");
    XMLElem zXML = getFirstAndOnly(polyXML, "Z");

    int order = str::toType<int>(xXML->getAttributes().getValue("order1"));
    PolyXYZ pXYZ(order);

    std::vector < XMLElem > xCoeffsXML;
    std::vector < XMLElem > yCoeffsXML;
    std::vector < XMLElem > zCoeffsXML;

    xXML->getElementsByTagName("Coef", xCoeffsXML);
    yXML->getElementsByTagName("Coef", yCoeffsXML);
    zXML->getElementsByTagName("Coef", zCoeffsXML);

    int exp;
    for (int i = 0; i <= order; i++)
    {
        //check the order attr, and use that index
        exp
                = str::toType<int>(
                                   xCoeffsXML[i]->getAttributes().getValue(
                                                                           "exponent1"));
        parseDouble(xCoeffsXML[i], pXYZ[exp][0]);

        exp
                = str::toType<int>(
                                   yCoeffsXML[i]->getAttributes().getValue(
                                                                           "exponent1"));
        parseDouble(yCoeffsXML[i], pXYZ[exp][1]);

        exp
                = str::toType<int>(
                                   zCoeffsXML[i]->getAttributes().getValue(
                                                                           "exponent1"));
        parseDouble(zCoeffsXML[i], pXYZ[exp][2]);
    }

    polyXYZ = pXYZ;
}

XMLElem XMLControl::createPolyXYZ(const std::string& name,
        const PolyXYZ& polyXYZ, XMLElem parent)
{
    int order = polyXYZ.order();
    XMLElem polyXML = newElement(name, getDefaultURI(), parent);

    std::string si = getSICommonURI();

    XMLElem xXML = newElement("X", si, polyXML);
    XMLElem yXML = newElement("Y", si, polyXML);
    XMLElem zXML = newElement("Z", si, polyXML);

    setAttribute(xXML, "order1", six::toString(order));
    setAttribute(yXML, "order1", six::toString(order));
    setAttribute(zXML, "order1", six::toString(order));

    for (int i = 0; i <= order; ++i)
    {
        Vector3 v3 = polyXYZ[i];
        XMLElem xCoefXML = createDouble("Coef", si, v3[0], xXML);
        XMLElem yCoefXML = createDouble("Coef", si, v3[1], yXML);
        XMLElem zCoefXML = createDouble("Coef", si, v3[2], zXML);

        setAttribute(xCoefXML, "exponent1", six::toString(i));
        setAttribute(yCoefXML, "exponent1", six::toString(i));
        setAttribute(zCoefXML, "exponent1", six::toString(i));
    }
    return polyXML;
}

void XMLControl::parsePoly1D(XMLElem polyXML, Poly1D& poly1D)
{
    int order1 = str::toType<int>(polyXML->getAttributes().getValue("order1"));
    Poly1D p1D(order1);

    std::vector < XMLElem > coeffsXML;
    polyXML->getElementsByTagName("Coef", coeffsXML);

    int exp1;
    for (int i = 0, size = coeffsXML.size(); i < size; ++i)
    {
        XMLElem element = coeffsXML[i];
        exp1 = str::toType<int>(element->getAttributes().getValue("exponent1"));
        parseDouble(element, p1D[exp1]);
    }
    poly1D = p1D;
}

void XMLControl::parsePoly2D(XMLElem polyXML, Poly2D& poly2D)
{
    int order1 = str::toType<int>(polyXML->getAttributes().getValue("order1"));
    int order2 = str::toType<int>(polyXML->getAttributes().getValue("order2"));
    Poly2D p2D(order1, order2);

    std::vector < XMLElem > coeffsXML;
    polyXML->getElementsByTagName("Coef", coeffsXML);

    int exp1, exp2;
    for (int i = 0, size = coeffsXML.size(); i < size; ++i)
    {
        XMLElem element = coeffsXML[i];
        exp1 = str::toType<int>(element->getAttributes().getValue("exponent1"));
        exp2 = str::toType<int>(element->getAttributes().getValue("exponent2"));
        parseDouble(element, p2D[exp1][exp2]);
    }
    poly2D = p2D;
}

XMLElem XMLControl::createPoly2D(const std::string& name,
        const std::string& uri, const Poly2D& poly2D, XMLElem parent)
{
    xml::lite::AttributeNode node;
    XMLElem poly2DXML = newElement(name, uri, parent);
    setAttribute(poly2DXML, "order1", six::toString(poly2D.orderX()));
    setAttribute(poly2DXML, "order2", six::toString(poly2D.orderY()));

    for (int i = 0; i <= poly2D.orderX(); i++)
    {
        for (int j = 0; j <= poly2D.orderY(); j++)
        {
            XMLElem coefXML = createDouble("Coef", getSICommonURI(),
                                           poly2D[i][j], poly2DXML);
            setAttribute(coefXML, "exponent1", six::toString(i));
            setAttribute(coefXML, "exponent2", six::toString(j));
        }
    }

    return poly2DXML;
}

XMLElem XMLControl::createPoly2D(const std::string& name,
        const Poly2D& poly2D, XMLElem parent)
{
    return createPoly2D(name, getDefaultURI(), poly2D, parent);
}

XMLElem XMLControl::createString(const std::string& name,
        const std::string& uri, const std::string& p, XMLElem parent)
{
    return newElement(name, uri, p, parent);
}

XMLElem XMLControl::createString(const std::string& name,
        const std::string& p, XMLElem parent)
{
    return createString(name, getDefaultURI(), p, parent);
}

XMLElem XMLControl::createInt(const std::string& name, const std::string& uri,
        int p, XMLElem parent)
{
    return newElement(name, uri, six::toString<int>(p), parent);
}

XMLElem XMLControl::createInt(const std::string& name, int p, XMLElem parent)
{
    return createInt(name, getDefaultURI(), p, parent);
}

XMLElem XMLControl::createDouble(const std::string& name,
        const std::string& uri, double p, XMLElem parent)
{
    return newElement(name, uri, six::toString<double>(p), parent);
}

XMLElem XMLControl::createDouble(const std::string& name, double p,
        XMLElem parent)
{
    return createDouble(name, getDefaultURI(), p, parent);
}

XMLElem XMLControl::createComplex(const std::string& name,
        std::complex<double> c, XMLElem parent)
{
    XMLElem e = newElement(name, getDefaultURI(), parent);
    createDouble("Real", getSICommonURI(), c.real(), e);
    createDouble("Imag", getSICommonURI(), c.imag(), e);
    return e;
}

XMLElem XMLControl::createBooleanType(const std::string& name,
        const std::string& uri, BooleanType p, XMLElem parent)
{
    if (p == six::BooleanType::NOT_SET)
        return NULL;
    return newElement(name, uri, six::toString<BooleanType>(p), parent);
}

XMLElem XMLControl::createBooleanType(const std::string& name, BooleanType p,
        XMLElem parent)
{
    return createBooleanType(name, getDefaultURI(), p, parent);
}

XMLElem XMLControl::createDateTime(const std::string& name,
        const std::string& uri, const std::string& s, XMLElem parent)
{
    return newElement(name, uri, s, parent);
}

XMLElem XMLControl::createDateTime(const std::string& name,
        const std::string& s, XMLElem parent)
{
    return createDateTime(name, getDefaultURI(), s, parent);
}

XMLElem XMLControl::createDateTime(const std::string& name,
        const std::string& uri, DateTime p, XMLElem parent)
{
    std::string s = six::toString<DateTime>(p);
    return createDateTime(name, uri, s, parent);
}

XMLElem XMLControl::createDateTime(const std::string& name, DateTime p,
        XMLElem parent)
{
    return createDateTime(name, getDefaultURI(), p, parent);
}

XMLElem XMLControl::createDate(const std::string& name,
        const std::string& uri, DateTime p, XMLElem parent)
{
    char date[256];
    date[255] = 0;
    p.format("%Y-%m-%d", date, 255);
    std::string s(date);

    return newElement(name, uri, s, parent);
}

XMLElem XMLControl::createDate(const std::string& name, DateTime p,
        XMLElem parent)
{
    return createDate(name, getDefaultURI(), p, parent);
}

XMLElem XMLControl::getFirstAndOnly(XMLElem parent, const std::string& tag)
{
    std::vector < XMLElem > children;
    parent->getElementsByTagName(tag, children);
    if (children.size() != 1)
    {
        throw except::Exception(Ctxt(
                 "Expected exactly one " + tag + " but got " +
                    str::toString(children.size())));
    }
    return children[0];
}
XMLElem XMLControl::getOptional(XMLElem parent, const std::string& tag)
{
    std::vector < XMLElem > children;
    parent->getElementsByTagName(tag, children);
    if (children.size() != 1)
        return NULL;
    return children[0];
}

XMLElem XMLControl::require(XMLElem element, const std::string& name)
{
    if (!element)
        throw except::Exception(Ctxt(FmtX("Required field [%s] is undefined "
            "or null.", name.c_str())));
    return element;
}

XMLElem XMLControl::createRowCol(const std::string& name,
        const std::string& rowName, const std::string& colName,
        const RowColInt& value, XMLElem parent)
{
    XMLElem e = newElement(name, getDefaultURI(), parent);
    createInt(rowName, getSICommonURI(), value.row, e);
    createInt(colName, getSICommonURI(), value.col, e);
    return e;
}

XMLElem XMLControl::createRowCol(const std::string& name,
        const std::string& rowName, const std::string& colName,
        const RowColDouble& value, XMLElem parent)
{
    XMLElem e = newElement(name, getDefaultURI(), parent);
    createDouble(rowName, getSICommonURI(), value.row, e);
    createDouble(colName, getSICommonURI(), value.col, e);
    return e;
}

XMLElem XMLControl::createRowCol(const std::string& name,
        const RowColInt& value, XMLElem parent)
{
    return createRowCol(name, "Row", "Col", value, parent);
}

XMLElem XMLControl::createRowCol(const std::string& name,
        const RowColDouble& value, XMLElem parent)
{
    return createRowCol(name, "Row", "Col", value, parent);
}

XMLElem XMLControl::createRowCol(const std::string& name,
        const RowColLatLon& value, XMLElem parent)
{
    XMLElem e = newElement(name, getDefaultURI(), parent);
    createLatLon("Row", value.row, e);
    createLatLon("Col", value.col, e);
    return e;
}

XMLElem XMLControl::createRangeAzimuth(const std::string& name,
        const RangeAzimuth<double>& value, XMLElem parent)
{
    XMLElem e = newElement(name, getDefaultURI(), parent);
    createDouble("Range", getSICommonURI(), value.range, e);
    createDouble("Azimuth", getSICommonURI(), value.azimuth, e);
    return e;
}

XMLElem XMLControl::createLatLon(const std::string& name, const LatLon& value,
        XMLElem parent)
{
    XMLElem e = newElement(name, getDefaultURI(), parent);
    createDouble("Lat", getSICommonURI(), value.getLat(), e);
    createDouble("Lon", getSICommonURI(), value.getLon(), e);
    return e;
}

XMLElem XMLControl::createLatLonAlt(const std::string& name,
        const LatLonAlt& value, XMLElem parent)
{
    XMLElem e = createLatLon(name, value, parent);
    createDouble("HAE", getSICommonURI(), value.getAlt(), e);
    return e;
}

void XMLControl::setAttribute(XMLElem e, const std::string& name,
        const std::string& v)
{
    xml::lite::AttributeNode node;
    node.setQName(name);
    node.setValue(v);
    e->getAttributes().add(node);
}

void XMLControl::parseInt(XMLElem element, int& value)
{
    try
    {
        value = str::toType<int>(element->getCharacterData());
    }
    catch (except::BadCastException& ex)
    {
        mLog->warn(Ctxt(FmtX("Unable to parse: %s", ex.toString().c_str())));
    }
}

void XMLControl::parseInt(XMLElem element, long& value)
{
    try
    {
        value = str::toType<long>(element->getCharacterData());
    }
    catch (except::BadCastException& ex)
    {
        mLog->warn(Ctxt(FmtX("Unable to parse: %s", ex.toString().c_str())));
    }
}

void XMLControl::parseUInt(XMLElem element, unsigned int& value)
{
    try
    {
        value = str::toType<unsigned int>(element->getCharacterData());
    }
    catch (except::BadCastException& ex)
    {
        mLog->warn(Ctxt(FmtX("Unable to parse: %s", ex.toString().c_str())));
    }
}

void XMLControl::parseUInt(XMLElem element, unsigned long& value)
{
    try
    {
        value = str::toType<unsigned long>(element->getCharacterData());
    }
    catch (except::BadCastException& ex)
    {
        mLog->warn(Ctxt(FmtX("Unable to parse: %s", ex.toString().c_str())));
    }
}

void XMLControl::parseDouble(XMLElem element, double& value)
{
    try
    {
        value = str::toType<double>(element->getCharacterData());
    }
    catch (except::BadCastException& ex)
    {
        mLog->warn(Ctxt(FmtX("Unable to parse: %s", ex.toString().c_str())));
    }
}

void XMLControl::parseComplex(XMLElem element, std::complex<double>& value)
{
    double r, i;

    parseDouble(getFirstAndOnly(element, "Real"), r);
    parseDouble(getFirstAndOnly(element, "Imag"), i);

    value = std::complex<double>(r, i);
}

void XMLControl::parseString(XMLElem element, std::string& value)
{
    value = element->getCharacterData();
}

void XMLControl::parseBooleanType(XMLElem element, BooleanType& value)
{
    try
    {
        value = six::toType<BooleanType>(element->getCharacterData());
    }
    catch (except::BadCastException& ex)
    {
        mLog->warn(Ctxt(FmtX("Unable to parse: %s", ex.toString().c_str())));
    }
}

void XMLControl::parseParameter(XMLElem element, Parameter& p)
{
    p.setName(element->getAttributes().getValue("name"));
    p.setValue<std::string>(element->getCharacterData());
}

void XMLControl::parseParameters(XMLElem paramXML,
        const std::string& paramName, std::vector<Parameter>& props)
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

XMLElem XMLControl::createParameter(const std::string& name,
        const std::string& uri, const Parameter& value, XMLElem parent)
{
    XMLElem element = createString(name, uri, value.str(), parent);
    setAttribute(element, "name", value.getName());
    return element;
}

XMLElem XMLControl::createParameter(const std::string& name,
        const Parameter& value, XMLElem parent)
{
    return createParameter(name, getDefaultURI(), value, parent);
}

void XMLControl::addParameters(const std::string& name,
        const std::string& uri, const std::vector<Parameter>& props,
        XMLElem parent)
{
    for (std::vector<Parameter>::const_iterator it = props.begin(); it
            != props.end(); ++it)
    {
        createParameter(name, uri, *it, parent);
    }
}

void XMLControl::addParameters(const std::string& name,
        const std::vector<Parameter>& props, XMLElem parent)
{
    addParameters(name, getDefaultURI(), props, parent);
}

void XMLControl::addDecorrType(const std::string& name,
        const std::string& uri, DecorrType decorrType, XMLElem parent)
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

void XMLControl::parseDecorrType(XMLElem decorrXML, DecorrType& decorrType)
{
    parseDouble(getFirstAndOnly(decorrXML, "CorrCoefZero"),
                decorrType.corrCoefZero);
    parseDouble(getFirstAndOnly(decorrXML, "DecorrRate"), decorrType.decorrRate);
}

void XMLControl::parseFootprint(XMLElem footprint,
                                const std::string& cornerName,
                                LatLonCorners &corners)
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

void XMLControl::parseFootprint(XMLElem footprint,
                                const std::string& cornerName,
                                LatLonAltCorners& corners)
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

void XMLControl::parseLatLon(XMLElem parent, LatLon& ll)
{
    double lat, lon;

    parseDouble(getFirstAndOnly(parent, "Lat"), lat);
    parseDouble(getFirstAndOnly(parent, "Lon"), lon);

    ll.setLat(lat);
    ll.setLon(lon);
}

void XMLControl::parseLatLons(XMLElem pointsXML, const std::string& pointName,
        std::vector<LatLon>& llVec, std::vector<int>& indexVec)
{
    std::vector < XMLElem > latLonsXML;
    pointsXML->getElementsByTagName(pointName, latLonsXML);

    for (std::vector<XMLElem>::iterator it = latLonsXML.begin(); it
            != latLonsXML.end(); ++it)
    {
        LatLon ll;
        parseLatLon(*it, ll);
        llVec.push_back(ll);

        int index = str::toType<int>((*it)->getAttributes().getValue("index"));
        indexVec.push_back(index);
    }
}

void XMLControl::parseRangeAzimuth(XMLElem parent, RangeAzimuth<double>& value)
{
    parseDouble(getFirstAndOnly(parent, "Range"), value.range);
    parseDouble(getFirstAndOnly(parent, "Azimuth"), value.azimuth);
}

void XMLControl::parseDateTime(XMLElem element, DateTime& value)
{
    value = six::toType<DateTime>(element->getCharacterData());
}

void XMLControl::parseRowColDouble(XMLElem parent, const std::string& rowName,
        const std::string& colName, RowColDouble& rc)
{
    parseDouble(getFirstAndOnly(parent, rowName), rc.row);
    parseDouble(getFirstAndOnly(parent, colName), rc.col);
}

void XMLControl::parseRowColLatLon(XMLElem parent, RowColLatLon& rc)
{
    parseLatLon(getFirstAndOnly(parent, "Row"), rc.row);
    parseLatLon(getFirstAndOnly(parent, "Col"), rc.col);
}

void XMLControl::parseRowColDouble(XMLElem parent, RowColDouble& rc)
{
    parseRowColDouble(parent, "Row", "Col", rc);
}

void XMLControl::parseRowColInt(XMLElem parent, const std::string& rowName,
        const std::string& colName, RowColInt& rc)
{
    parseInt(getFirstAndOnly(parent, rowName), rc.row);
    parseInt(getFirstAndOnly(parent, colName), rc.col);
}

void XMLControl::parseRowColInt(XMLElem parent, RowColInt& rc)
{
    parseRowColInt(parent, "Row", "Col", rc);
}

XMLElem XMLControl::createFootprint(const std::string& name,
                                    const std::string& cornerName,
                                    const LatLonCorners& corners,
                                    XMLElem parent)
{
    XMLElem footprint = newElement(name, getDefaultURI(), parent);
    xml::lite::AttributeNode node;
    node.setQName("size");
    node.setValue(str::toString(LatLonCorners::NUM_CORNERS));

    footprint->getAttributes().add(node);

    // Write the corners out in CW order
    // The index attribute is 1-based
    node.setQName("index");

    for (size_t corner = 0; corner < LatLonCorners::NUM_CORNERS; ++corner)
    {
        node.setValue(str::toString(corner + 1));
        createLatLon(cornerName,
                     corners.getCorner(corner),
                     footprint)->getAttributes().add(node);
    }

    return footprint;
}

XMLElem XMLControl::createFootprint(const std::string& name,
                                    const std::string& cornerName,
                                    const LatLonAltCorners& corners,
                                    XMLElem parent)
{
    XMLElem footprint = newElement(name, getDefaultURI(), parent);
    xml::lite::AttributeNode node;
    node.setQName("size");
    node.setValue(str::toString(LatLonAltCorners::NUM_CORNERS));

    footprint->getAttributes().add(node);

    // Write the corners out in CW order
    // The index attribute is 1-based
    node.setQName("index");

    for (size_t corner = 0; corner < LatLonCorners::NUM_CORNERS; ++corner)
    {
        node.setValue(str::toString(corner + 1));
        createLatLonAlt(cornerName,
                        corners.getCorner(corner),
                        footprint)->getAttributes().add(node);
    }

    return footprint;
}

XMLElem XMLControl::toXML(const ErrorStatistics* errorStatistics,
        XMLElem parent)
{
    XMLElem errorStatsXML = newElement("ErrorStatistics", getDefaultURI(),
                                       parent);

    //TODO compositeSCP needs to be reworked

    std::string si = getSICommonURI();
    if (errorStatistics->compositeSCP.get())
    {
        XMLElem scpXML = newElement("CompositeSCP", si, errorStatsXML);

        if (errorStatistics->scpType == ErrorStatistics::RG_AZ)
        {
            XMLElem rgAzXML = newElement("RgAzErr", si, scpXML);
            createDouble("Rg", si, errorStatistics->compositeSCP->xErr, rgAzXML);
            createDouble("Az", si, errorStatistics->compositeSCP->yErr, rgAzXML);
            createDouble("RgAz", si, errorStatistics->compositeSCP->xyErr,
                         rgAzXML);
        }
        else
        {
            XMLElem rgAzXML = newElement("RowColErr", si, scpXML);
            createDouble("Row", si, errorStatistics->compositeSCP->xErr,
                         rgAzXML);
            createDouble("Col", si, errorStatistics->compositeSCP->yErr,
                         rgAzXML);
            createDouble("RgCol", si, errorStatistics->compositeSCP->xyErr,
                         rgAzXML);
        }
    }

    const Components* const components = errorStatistics->components.get();
    if (components)
    {
        XMLElem componentsXML = newElement("Components", si, errorStatsXML);

        const PosVelError* const posVelError = components->posVelError.get();
        const RadarSensor* const radarSensor = components->radarSensor.get();
        const TropoError* const tropoError = components->tropoError.get();
        const IonoError* const ionoError = components->ionoError.get();

        if (posVelError)
        {
            XMLElem posVelErrXML = newElement("PosVelErr", si, componentsXML);

            createString("Frame", si, six::toString(posVelError->frame),
                         posVelErrXML);
            createDouble("P1", si, posVelError->p1, posVelErrXML);
            createDouble("P2", si, posVelError->p2, posVelErrXML);
            createDouble("P3", si, posVelError->p3, posVelErrXML);
            createDouble("V1", si, posVelError->v1, posVelErrXML);
            createDouble("V2", si, posVelError->v2, posVelErrXML);
            createDouble("V3", si, posVelError->v3, posVelErrXML);

            const CorrCoefs* const coefs = posVelError->corrCoefs.get();
            if (coefs)
            {
                XMLElem coefsXML = newElement("CorrCoefs", si, posVelErrXML);

                createDouble("P1P2", si, coefs->p1p2, coefsXML);
                createDouble("P1P3", si, coefs->p1p3, coefsXML);
                createDouble("P1V1", si, coefs->p1v1, coefsXML);
                createDouble("P1V2", si, coefs->p1v2, coefsXML);
                createDouble("P1V3", si, coefs->p1v3, coefsXML);
                createDouble("P2P3", si, coefs->p2p3, coefsXML);
                createDouble("P2V1", si, coefs->p2v1, coefsXML);
                createDouble("P2V2", si, coefs->p2v2, coefsXML);
                createDouble("P2V3", si, coefs->p2v3, coefsXML);
                createDouble("P3V1", si, coefs->p3v1, coefsXML);
                createDouble("P3V2", si, coefs->p3v2, coefsXML);
                createDouble("P3V3", si, coefs->p3v3, coefsXML);
                createDouble("V1V2", si, coefs->v1v2, coefsXML);
                createDouble("V1V3", si, coefs->v1v3, coefsXML);
                createDouble("V2V3", si, coefs->v2v3, coefsXML);
            }

            addDecorrType("PositionDecorr", si, posVelError->positionDecorr,
                          posVelErrXML);
        }
        if (radarSensor)
        {
            XMLElem radarSensorXML = newElement("RadarSensor", si,
                                                componentsXML);

            createDouble("RangeBias", si, radarSensor->rangeBias,
                         radarSensorXML);

            if (!Init::isUndefined<double>(radarSensor->clockFreqSF))
            {
                createDouble("ClockFreqSF", si, radarSensor->clockFreqSF,
                             radarSensorXML);
            }
            if (!Init::isUndefined<double>(radarSensor->transmitFreqSF))
            {
                createDouble("TransmitFreqSF", si, radarSensor->transmitFreqSF,
                             radarSensorXML);
            }
            addDecorrType("RangeBiasDecorr", si, radarSensor->rangeBiasDecorr,
                          radarSensorXML);
        }
        if (tropoError)
        {
            XMLElem tropoErrXML = newElement("TropoError", si, componentsXML);

            if (!Init::isUndefined<double>(tropoError->tropoRangeVertical))
            {
                createDouble("TropoRangeVertical", si,
                             tropoError->tropoRangeVertical, tropoErrXML);
            }
            if (!Init::isUndefined<double>(tropoError->tropoRangeSlant))
            {
                createDouble("TropoRangeSlant", si,
                             tropoError->tropoRangeSlant, tropoErrXML);
            }

            addDecorrType("TropoRangeDecorr", si, tropoError->tropoRangeDecorr,
                          tropoErrXML);
        }
        if (ionoError)
        {
            XMLElem ionoErrXML = newElement("IonoError", si, componentsXML);

            if (!Init::isUndefined<double>(ionoError->ionoRangeVertical))
            {
                createDouble("IonoRangeVertical", si,
                             ionoError->ionoRangeVertical, ionoErrXML);
            }
            if (!Init::isUndefined<double>(ionoError->ionoRangeRateVertical))
            {
                createDouble("IonoRangeRateVertical", si,
                             ionoError->ionoRangeRateVertical, ionoErrXML);
            }

            createDouble("IonoRgRgRateCC", si, ionoError->ionoRgRgRateCC,
                         ionoErrXML);

            addDecorrType("IonoRangeVertDecorr", si,
                          ionoError->ionoRangeVertDecorr, ionoErrXML);
        }
    }

    if (!errorStatistics->additionalParameters.empty())
    {
        XMLElem paramsXML = newElement("AdditionalParms", si, errorStatsXML);
        addParameters("Parameter", si, errorStatistics->additionalParameters,
                      paramsXML);
    }

    return errorStatsXML;
}

void XMLControl::fromXML(const XMLElem errorStatsXML,
        ErrorStatistics* errorStatistics)
{
    XMLElem tmpElem = NULL;
    //optional
    XMLElem compositeSCPXML = getOptional(errorStatsXML, "CompositeSCP");
    //See if it's RgAzErr or RowColErr
    //optional
    if (compositeSCPXML)
    {
        errorStatistics->compositeSCP.reset(new CompositeSCP());
        tmpElem = getOptional(compositeSCPXML, "RgAzErr");
        if (tmpElem)
        {
            //errorStatistics->initialize(ErrorStatistics::RG_AZ);
            errorStatistics->scpType = ErrorStatistics::RG_AZ;
        }
        else
        {
            tmpElem = getOptional(compositeSCPXML, "RowColErr");
            if (tmpElem)
            {
                //errorStatistics->initialize(ErrorStatistics::ROW_COL);
                errorStatistics->scpType = ErrorStatistics::ROW_COL;
            }
        }

        if (tmpElem != NULL && errorStatistics->scpType
                == ErrorStatistics::RG_AZ)
        {
            parseDouble(getFirstAndOnly(tmpElem, "Rg"),
                        errorStatistics->compositeSCP->xErr);
            parseDouble(getFirstAndOnly(tmpElem, "Az"),
                        errorStatistics->compositeSCP->yErr);
            parseDouble(getFirstAndOnly(tmpElem, "RgAz"),
                        errorStatistics->compositeSCP->xyErr);
        }
        else if (tmpElem != NULL && errorStatistics->scpType
                == ErrorStatistics::ROW_COL)
        {
            parseDouble(getFirstAndOnly(tmpElem, "Row"),
                        errorStatistics->compositeSCP->xErr);
            parseDouble(getFirstAndOnly(tmpElem, "Col"),
                        errorStatistics->compositeSCP->yErr);
            parseDouble(getFirstAndOnly(tmpElem, "RowCol"),
                        errorStatistics->compositeSCP->xyErr);
        }
    }

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
                = six::toType<FrameType>(
                                         getFirstAndOnly(posVelErrXML, "Frame")->getCharacterData());
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
            parseDouble(
                        getFirstAndOnly(tmpElem, "P1P2"),
                        errorStatistics->components->posVelError->corrCoefs->p1p2);
            parseDouble(
                        getFirstAndOnly(tmpElem, "P1P3"),
                        errorStatistics->components->posVelError->corrCoefs->p1p3);
            parseDouble(
                        getFirstAndOnly(tmpElem, "P1V1"),
                        errorStatistics->components->posVelError->corrCoefs->p1v1);
            parseDouble(
                        getFirstAndOnly(tmpElem, "P1V2"),
                        errorStatistics->components->posVelError->corrCoefs->p1v2);
            parseDouble(
                        getFirstAndOnly(tmpElem, "P1V3"),
                        errorStatistics->components->posVelError->corrCoefs->p1v3);
            parseDouble(
                        getFirstAndOnly(tmpElem, "P2P3"),
                        errorStatistics->components->posVelError->corrCoefs->p2p3);
            parseDouble(
                        getFirstAndOnly(tmpElem, "P2V1"),
                        errorStatistics->components->posVelError->corrCoefs->p2v1);
            parseDouble(
                        getFirstAndOnly(tmpElem, "P2V2"),
                        errorStatistics->components->posVelError->corrCoefs->p2v2);
            parseDouble(
                        getFirstAndOnly(tmpElem, "P2V3"),
                        errorStatistics->components->posVelError->corrCoefs->p2v3);
            parseDouble(
                        getFirstAndOnly(tmpElem, "P3V1"),
                        errorStatistics->components->posVelError->corrCoefs->p3v1);
            parseDouble(
                        getFirstAndOnly(tmpElem, "P3V2"),
                        errorStatistics->components->posVelError->corrCoefs->p3v2);
            parseDouble(
                        getFirstAndOnly(tmpElem, "P3V3"),
                        errorStatistics->components->posVelError->corrCoefs->p3v3);
            parseDouble(
                        getFirstAndOnly(tmpElem, "V1V2"),
                        errorStatistics->components->posVelError->corrCoefs->v1v2);
            parseDouble(
                        getFirstAndOnly(tmpElem, "V1V3"),
                        errorStatistics->components->posVelError->corrCoefs->v1v3);
            parseDouble(
                        getFirstAndOnly(tmpElem, "V2V3"),
                        errorStatistics->components->posVelError->corrCoefs->v2v3);
        }

        tmpElem = getOptional(posVelErrXML, "PositionDecorr");
        if (tmpElem)
        {
            //optional
            parseDecorrType(
                            tmpElem,
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
            parseDouble(
                        tmpElem,
                        errorStatistics->components->radarSensor->transmitFreqSF);
        }

        tmpElem = getOptional(radarSensorXML, "RangeBiasDecorr");
        if (tmpElem)
        {
            parseDecorrType(
                            tmpElem,
                            errorStatistics->components->radarSensor->rangeBiasDecorr);
        }
    }

    if (tropoErrorXML != NULL)
    {
        tmpElem = getOptional(tropoErrorXML, "TropoRangeVertical");
        if (tmpElem)
        {
            //optional
            parseDouble(
                        tmpElem,
                        errorStatistics->components->tropoError ->tropoRangeVertical);
        }

        tmpElem = getOptional(tropoErrorXML, "TropoRangeSlant");
        if (tmpElem)
        {
            //optional
            parseDouble(
                        tmpElem,
                        errorStatistics->components->tropoError->tropoRangeSlant);
        }

        tmpElem = getOptional(tropoErrorXML, "TropoRangeDecorr");
        if (tmpElem)
        {
            parseDecorrType(
                            tmpElem,
                            errorStatistics->components->tropoError->tropoRangeDecorr);
        }
    }

    if (ionoErrorXML != NULL)
    {
        tmpElem = getOptional(ionoErrorXML, "IonoRangeVertical");
        if (tmpElem)
        {
            //optional
            parseDouble(
                        tmpElem,
                        errorStatistics->components->ionoError->ionoRangeVertical);
        }

        tmpElem = getOptional(ionoErrorXML, "IonoRangeRateVertical");
        if (tmpElem)
        {
            //optional
            parseDouble(
                        tmpElem,
                        errorStatistics->components->ionoError ->ionoRangeRateVertical);
        }

        parseDouble(getFirstAndOnly(ionoErrorXML, "IonoRgRgRateCC"),
                    errorStatistics->components->ionoError->ionoRgRgRateCC);

        tmpElem = getOptional(ionoErrorXML, "IonoRangeVertDecorr");
        if (tmpElem)
        {
            parseDecorrType(
                            tmpElem,
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

void XMLControl::fromXML(const XMLElem radiometricXML, Radiometric *radiometric)
{
    XMLElem tmpElem = NULL;

    tmpElem = getOptional(radiometricXML, "NoisePoly");
    if (tmpElem)
    {
        //optional
        parsePoly2D(tmpElem, radiometric->noisePoly);
    }

    tmpElem = getOptional(radiometricXML, "RCSSFPoly");
    if (tmpElem)
    {
        //optional
        parsePoly2D(tmpElem, radiometric->rcsSFPoly);
    }

    tmpElem = getOptional(radiometricXML, "BetaZeroSFPoly");
    if (tmpElem)
    {
        //optional
        parsePoly2D(tmpElem, radiometric->betaZeroSFPoly);
    }

    tmpElem = getOptional(radiometricXML, "SigmaZeroSFPoly");
    if (tmpElem)
    {
        //optional
        parsePoly2D(tmpElem, radiometric->sigmaZeroSFPoly);
    }

    tmpElem = getOptional(radiometricXML, "SigmaZeroSFIncidenceMap");
    if (tmpElem)
    {
        //optional
        radiometric->sigmaZeroSFIncidenceMap
                = six::toType<six::AppliedType>(tmpElem->getCharacterData());
    }

    tmpElem = getOptional(radiometricXML, "GammaZeroSFPoly");
    if (tmpElem)
    {
        //optional
        parsePoly2D(tmpElem, radiometric->gammaZeroSFPoly);
    }

    tmpElem = getOptional(radiometricXML, "GammaZeroSFIncidenceMap");
    if (tmpElem)
    {
        //optional
        radiometric->gammaZeroSFIncidenceMap
                = six::toType<six::AppliedType>(tmpElem->getCharacterData());
    }
}

XMLElem XMLControl::toXML(const Radiometric *r, XMLElem parent)
{
    XMLElem rXML = newElement("Radiometric", getDefaultURI(), parent);

    std::string si = getSICommonURI();

    if (!r->noisePoly.empty())
        createPoly2D("NoisePoly", si, r->noisePoly, rXML);
    if (!r->rcsSFPoly.empty())
        createPoly2D("RCSSFPoly", si, r->rcsSFPoly, rXML);
    if (!r->betaZeroSFPoly.empty())
        createPoly2D("BetaZeroSFPoly", si, r->betaZeroSFPoly, rXML);
    if (!r->sigmaZeroSFPoly.empty())
        createPoly2D("SigmaZeroSFPoly", si, r->sigmaZeroSFPoly, rXML);

    if (r->sigmaZeroSFIncidenceMap != AppliedType::NOT_SET)
    {
        createString("SigmaZeroSFIncidenceMap", si, six::toString<
                six::AppliedType>(r->sigmaZeroSFIncidenceMap), rXML);
    }
    if (!r->gammaZeroSFPoly.empty())
        createPoly2D("GammaZeroSFPoly", si, r->sigmaZeroSFPoly, rXML);
    if (r->gammaZeroSFIncidenceMap != AppliedType::NOT_SET)
    {
        createString("GammaZeroSFIncidenceMap", si, six::toString<
                six::AppliedType>(r->gammaZeroSFIncidenceMap), rXML);
    }
    return rXML;
}
