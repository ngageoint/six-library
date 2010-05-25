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
#include "six/XMLControl.h"
#include "six/Types.h"
#include "six/Utilities.h"
#include <import/str.h>

using namespace six;

XMLControl::XMLControl(logging::Logger* log) :
    mLog(NULL), mOwnLog(false)
{
    setLogger(log);
}

XMLControl::~XMLControl()
{
    if (mLog && mOwnLog)
        delete mLog;
}

void XMLControl::setLogger(logging::Logger* log)
{
    if (mLog && mOwnLog && log != mLog)
        delete mLog;
    // create a dummy logger if one wasn't supplied - this lets us use the log
    // without checking for NULL every time
    mLog = log ? log : new logging::Logger("XMLControl");
    mOwnLog = !log;
}

xml::lite::Element* XMLControl::newElement(std::string name,
                                           xml::lite::Element* parent)
{
    return newElement(name, "", parent);
}

xml::lite::Element* XMLControl::newElement(std::string name,
                                           std::string characterData,
                                           xml::lite::Element* parent)
{
    xml::lite::Element* elem =
            new xml::lite::Element(name, mURI, characterData);
    if (parent)
        parent->addChild(elem);
    return elem;
}

void XMLControl::parseVector3D(xml::lite::Element* vecXML, Vector3& vec)
{
    parseDouble(getFirstAndOnly(vecXML, "X"), vec[0]);
    parseDouble(getFirstAndOnly(vecXML, "Y"), vec[1]);
    parseDouble(getFirstAndOnly(vecXML, "Z"), vec[2]);
}

void XMLControl::parseLatLonAlt(xml::lite::Element* llaXML, LatLonAlt& lla)
{
    double lat, lon, alt;

    parseDouble(getFirstAndOnly(llaXML, "Lat"), lat);
    parseDouble(getFirstAndOnly(llaXML, "Lon"), lon);
    parseDouble(getFirstAndOnly(llaXML, "HAE"), alt);

    lla.setLat(lat);
    lla.setLon(lon);
    lla.setAlt(alt);
}

xml::lite::Element* XMLControl::createVector3D(std::string name, Vector3 p,
                                               xml::lite::Element* parent)
{
    xml::lite::Element* e = newElement(name, parent);
    createDouble("X", p[0], e);
    createDouble("Y", p[1], e);
    createDouble("Z", p[2], e);
    return e;
}

xml::lite::Element* XMLControl::createPoly1D(std::string name,
                                             const Poly1D& poly1D,
                                             xml::lite::Element* parent)
{
    int order = poly1D.order();
    xml::lite::Element* poly1DXML = newElement(name, parent);
    setAttribute(poly1DXML, "order1", str::toString(order));

    for (int i = 0; i <= order; ++i)
    {
        xml::lite::Element* coefXML =
                createDouble("Coef", poly1D[i], poly1DXML);
        setAttribute(coefXML, "exponent1", str::toString(i));
    }
    return poly1DXML;
}

void XMLControl::parsePolyXYZ(xml::lite::Element* polyXML, PolyXYZ& polyXYZ)
{
    xml::lite::Element* xXML = getFirstAndOnly(polyXML, "X");
    xml::lite::Element* yXML = getFirstAndOnly(polyXML, "Y");
    xml::lite::Element* zXML = getFirstAndOnly(polyXML, "Z");

    int order = str::toType<int>(xXML->getAttributes().getValue("order1"));
    PolyXYZ pXYZ(order);

    std::vector<xml::lite::Element*> xCoeffsXML;
    std::vector<xml::lite::Element*> yCoeffsXML;
    std::vector<xml::lite::Element*> zCoeffsXML;

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

xml::lite::Element* XMLControl::createPolyXYZ(std::string name,
                                              const PolyXYZ& polyXYZ,
                                              xml::lite::Element* parent)
{
    int order = polyXYZ.order();
    xml::lite::Element* polyXML = newElement(name, parent);

    xml::lite::Element* xXML = newElement("X", polyXML);
    xml::lite::Element* yXML = newElement("Y", polyXML);
    xml::lite::Element* zXML = newElement("Z", polyXML);

    setAttribute(xXML, "order1", str::toString(order));
    setAttribute(yXML, "order1", str::toString(order));
    setAttribute(zXML, "order1", str::toString(order));

    for (int i = 0; i <= order; ++i)
    {
        Vector3 v3 = polyXYZ[i];
        xml::lite::Element* xCoefXML = createDouble("Coef", v3[0], xXML);
        xml::lite::Element* yCoefXML = createDouble("Coef", v3[1], yXML);
        xml::lite::Element* zCoefXML = createDouble("Coef", v3[2], zXML);

        setAttribute(xCoefXML, "exponent1", str::toString(i));
        setAttribute(yCoefXML, "exponent1", str::toString(i));
        setAttribute(zCoefXML, "exponent1", str::toString(i));
    }
    return polyXML;
}

void XMLControl::parsePoly1D(xml::lite::Element* polyXML, Poly1D& poly1D)
{
    int order1 = str::toType<int>(polyXML->getAttributes().getValue("order1"));
    Poly1D p1D(order1);

    std::vector<xml::lite::Element*> coeffsXML;
    polyXML->getElementsByTagName("Coef", coeffsXML);

    int exp1;
    for (int i = 0, size = coeffsXML.size(); i < size; ++i)
    {
        xml::lite::Element *element = coeffsXML[i];
        exp1 = str::toType<int>(element->getAttributes().getValue("exponent1"));
        parseDouble(element, p1D[exp1]);
    }
    poly1D = p1D;
}

void XMLControl::parsePoly2D(xml::lite::Element* polyXML, Poly2D& poly2D)
{
    int order1 = str::toType<int>(polyXML->getAttributes().getValue("order1"));
    int order2 = str::toType<int>(polyXML->getAttributes().getValue("order2"));
    Poly2D p2D(order1, order2);

    std::vector<xml::lite::Element*> coeffsXML;
    polyXML->getElementsByTagName("Coef", coeffsXML);

    int exp1, exp2;
    for (int i = 0, size = coeffsXML.size(); i < size; ++i)
    {
        xml::lite::Element *element = coeffsXML[i];
        exp1 = str::toType<int>(element->getAttributes().getValue("exponent1"));
        exp2 = str::toType<int>(element->getAttributes().getValue("exponent2"));
        parseDouble(element, p2D[exp1][exp2]);
    }
    poly2D = p2D;
}

xml::lite::Element* XMLControl::createPoly2D(std::string name,
                                             const Poly2D& poly2D,
                                             xml::lite::Element* parent)
{
    xml::lite::AttributeNode node;
    xml::lite::Element* poly2DXML = newElement(name, parent);
    setAttribute(poly2DXML, "order1", str::toString(poly2D.orderX()));
    setAttribute(poly2DXML, "order2", str::toString(poly2D.orderY()));

    for (int i = 0; i <= poly2D.orderX(); i++)
    {
        for (int j = 0; j <= poly2D.orderY(); j++)
        {
            xml::lite::Element* coefXML = createDouble("Coef", poly2D[i][j],
                                                       poly2DXML);
            setAttribute(coefXML, "exponent1", str::toString(i));
            setAttribute(coefXML, "exponent2", str::toString(j));
        }
    }

    return poly2DXML;
}

xml::lite::Element* XMLControl::createString(std::string name, std::string p,
                                             xml::lite::Element* parent)
{
    xml::lite::Element* e = newElement(name, p, parent);
    xml::lite::AttributeNode node;
    node.setQName("class");
    node.setUri(mURI);
    node.setValue("xs:string");
    e->getAttributes().add(node);
    return e;
}

xml::lite::Element* XMLControl::createInt(std::string name, int p,
                                          xml::lite::Element* parent)
{
    xml::lite::Element* e = newElement(name, str::toString<int>(p), parent);
    xml::lite::AttributeNode node;
    node.setQName("class");
    node.setUri(mURI);
    node.setValue("xs:int");
    e->getAttributes().add(node);
    return e;
}

xml::lite::Element* XMLControl::createDouble(std::string name, double p,
                                             xml::lite::Element* parent)
{
    xml::lite::Element* e = newElement(name, str::toString<double>(p), parent);
    xml::lite::AttributeNode node;
    node.setQName("class");
    node.setUri(mURI);
    node.setValue("xs:double");
    e->getAttributes().add(node);
    return e;
}

xml::lite::Element* XMLControl::createComplex(std::string name, std::complex<
        double> c, xml::lite::Element* parent)
{
    xml::lite::Element* e = newElement(name, parent);
    createDouble("Real", c.real(), e);
    createDouble("Imag", c.imag(), e);
    return e;
}

xml::lite::Element* XMLControl::createBooleanType(std::string name,
                                                  BooleanType p,
                                                  xml::lite::Element* parent)
{
    if (p == six::BOOL_NOT_SET)
        return NULL;

    xml::lite::Element* e = newElement(name, str::toString<BooleanType>(p),
                                       parent);
    xml::lite::AttributeNode node;
    node.setQName("class");
    node.setUri(mURI);
    node.setValue("xs:boolean");
    e->getAttributes().add(node);
    return e;
}

xml::lite::Element* XMLControl::createDateTime(std::string name, std::string s,
                                               xml::lite::Element* parent)
{
    xml::lite::Element* e = newElement(name, s, parent);
    xml::lite::AttributeNode node;
    node.setQName("class");
    node.setUri(mURI);
    node.setValue("xs:dateTime");
    e->getAttributes().add(node);
    return e;
}

xml::lite::Element* XMLControl::createDateTime(std::string name, DateTime p,
                                               xml::lite::Element* parent)
{
    std::string s = str::toString<DateTime>(p);
    return createDateTime(name, s, parent);
}

xml::lite::Element* XMLControl::createDate(std::string name, DateTime p,
                                           xml::lite::Element* parent)
{
    char date[256];
    date[255] = 0;
    p.format("%Y-%m-%d", date, 255);
    std::string s(date);

    xml::lite::Element* e = newElement(name, s, parent);
    xml::lite::AttributeNode node;
    node.setQName("class");
    node.setUri(mURI);
    node.setValue("xs:date");
    e->getAttributes().add(node);
    return e;
}

xml::lite::Element* XMLControl::getFirstAndOnly(xml::lite::Element* parent,
                                                std::string tag)
{
    std::vector<xml::lite::Element*> children;
    parent->getElementsByTagName(tag, children);
    if (children.size() != 1)
    {
        throw except::Exception(
                                Ctxt(std::string("Expected exactly one ") + tag));
    }
    return children[0];
}
xml::lite::Element* XMLControl::getOptional(xml::lite::Element* parent,
                                            std::string tag)
{
    std::vector<xml::lite::Element*> children;
    parent->getElementsByTagName(tag, children);
    if (children.size() != 1)
        return NULL;
    return children[0];
}

xml::lite::Element* XMLControl::require(xml::lite::Element* element,
                                        std::string name)
{
    if (!element)
        throw except::Exception(Ctxt(FmtX("Required field [%s] is undefined "
            "or null.", name.c_str())));
    return element;
}

xml::lite::Element* XMLControl::createRowCol(std::string name,
                                             std::string rowName,
                                             std::string colName,
                                             const RowColInt& value,
                                             xml::lite::Element* parent)
{
    xml::lite::Element* e = newElement(name, parent);
    createInt(rowName, value.row, e);
    createInt(colName, value.col, e);
    return e;
}

xml::lite::Element* XMLControl::createRowCol(std::string name,
                                             std::string rowName,
                                             std::string colName,
                                             const RowColDouble& value,
                                             xml::lite::Element* parent)
{
    xml::lite::Element* e = newElement(name, parent);
    createDouble(rowName, value.row, e);
    createDouble(colName, value.col, e);
    return e;
}

xml::lite::Element* XMLControl::createRowCol(std::string name,
                                             const RowColInt& value,
                                             xml::lite::Element* parent)
{
    return createRowCol(name, "Row", "Col", value, parent);
}

xml::lite::Element* XMLControl::createRowCol(std::string name,
                                             const RowColDouble& value,
                                             xml::lite::Element* parent)
{
    return createRowCol(name, "Row", "Col", value, parent);
}

xml::lite::Element* XMLControl::createRowCol(std::string name,
                                             const RowColLatLon& value,
                                             xml::lite::Element* parent)
{
    xml::lite::Element* e = newElement(name, parent);
    createLatLon("Row", value.row, e);
    createLatLon("Col", value.col, e);
    return e;
}

xml::lite::Element* XMLControl::createRangeAzimuth(
                                                   std::string name,
                                                   const RangeAzimuth<double>& value,
                                                   xml::lite::Element* parent)
{
    xml::lite::Element* e = newElement(name, parent);
    createDouble("Range", value.range, e);
    createDouble("Azimuth", value.azimuth, e);
    return e;
}

xml::lite::Element* XMLControl::createLatLon(std::string name,
                                             const LatLon& value,
                                             xml::lite::Element* parent)
{
    xml::lite::Element* e = newElement(name, parent);
    createDouble("Lat", value.getLat(), e);
    createDouble("Lon", value.getLon(), e);
    return e;
}

xml::lite::Element* XMLControl::createLatLonAlt(std::string name,
                                                const LatLonAlt& value,
                                                xml::lite::Element* parent)
{
    xml::lite::Element* e = createLatLon(name, value, parent);
    createDouble("HAE", value.getAlt(), e);
    return e;
}

xml::lite::Element* XMLControl::createEarthModelType(
                                                     std::string name,
                                                     const EarthModelType& value,
                                                     xml::lite::Element* parent)
{
    return createString(name, str::toString(value), parent);
}

xml::lite::Element* XMLControl::createSideOfTrackType(
                                                      std::string name,
                                                      const SideOfTrackType& value,
                                                      xml::lite::Element* parent)
{
    return createString(name, str::toString(value), parent);
}

void XMLControl::setAttribute(xml::lite::Element* e, std::string name,
                              std::string v)
{
    xml::lite::AttributeNode node;
    node.setQName(name);
    node.setValue(v);
    e->getAttributes().add(node);
}

void XMLControl::parseInt(xml::lite::Element* element, int& value)
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

void XMLControl::parseInt(xml::lite::Element* element, long& value)
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

void XMLControl::parseUInt(xml::lite::Element* element, unsigned int& value)
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

void XMLControl::parseUInt(xml::lite::Element* element, unsigned long& value)
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

void XMLControl::parseDouble(xml::lite::Element* element, double& value)
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

void XMLControl::parseComplex(xml::lite::Element* element,
                              std::complex<double>& value)
{
    double r, i;

    parseDouble(getFirstAndOnly(element, "Real"), r);
    parseDouble(getFirstAndOnly(element, "Imag"), i);

    value = std::complex<double>(r, i);
}

void XMLControl::parseString(xml::lite::Element* element, std::string& value)
{
    value = element->getCharacterData();
}

void XMLControl::parseBooleanType(xml::lite::Element* element,
                                  BooleanType& value)
{
    try
    {
        value = str::toType<BooleanType>(element->getCharacterData());
    }
    catch (except::BadCastException& ex)
    {
        mLog->warn(Ctxt(FmtX("Unable to parse: %s", ex.toString().c_str())));
    }
}

void XMLControl::parseParameter(xml::lite::Element* element, Parameter& p)
{
    p.setName(element->getAttributes().getValue("name"));
    p.setValue<std::string>(element->getCharacterData());
}

void XMLControl::parseParameters(xml::lite::Element* paramXML,
                                 std::string paramName,
                                 std::vector<Parameter>& props)
{
    std::vector<xml::lite::Element*> elemXML;
    paramXML->getElementsByTagName(paramName, elemXML);

    for (std::vector<xml::lite::Element*>::iterator it = elemXML.begin(); it
            != elemXML.end(); ++it)
    {
        Parameter p;
        parseParameter(*it, p);
        props.push_back(p);
    }
}

xml::lite::Element* XMLControl::createParameter(std::string name,
                                                const Parameter& value,
                                                xml::lite::Element* parent)
{
    xml::lite::Element *element = createString(name, value.str(), parent);
    setAttribute(element, "name", value.getName());
    return element;
}

void XMLControl::addParameters(std::string name, std::vector<Parameter>& props,
                               xml::lite::Element* parent)
{
    for (std::vector<Parameter>::iterator it = props.begin(); it != props.end(); ++it)
    {
        createParameter(name, *it, parent);
    }
}

void XMLControl::addDecorrType(std::string name, DecorrType& decorrType,
                               xml::lite::Element* parent)
{
    //only adds it if it needs to
    if (!Init::isUndefined<double>(decorrType.corrCoefZero)
            && !Init::isUndefined<double>(decorrType.decorrRate))
    {
        xml::lite::Element* decorrXML = newElement(name, parent);
        createDouble("CorrCoefZero", decorrType.corrCoefZero, decorrXML);
        createDouble("DecorrRate", decorrType.decorrRate, decorrXML);
    }
}

void XMLControl::parseDecorrType(xml::lite::Element* decorrXML,
                                 DecorrType& decorrType)
{
    parseDouble(getFirstAndOnly(decorrXML, "CorrCoefZero"),
                decorrType.corrCoefZero);
    parseDouble(getFirstAndOnly(decorrXML, "DecorrRate"), decorrType.decorrRate);
}

void XMLControl::parseEarthModelType(xml::lite::Element* element,
                                     EarthModelType& value)
{
    value = str::toType<EarthModelType>(element->getCharacterData());
}

void XMLControl::parseSideOfTrackType(xml::lite::Element* element,
                                      SideOfTrackType& value)
{
    value = str::toType<SideOfTrackType>(element->getCharacterData());
}

void XMLControl::parseFootprint(xml::lite::Element* footprint,
                                std::string cornerName,
                                std::vector<LatLon>& value, bool alt)
{
    std::vector<xml::lite::Element*> vertices;
    footprint->getElementsByTagName(cornerName, vertices);

    value.clear();
    value.resize(4);

    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        LatLonAlt lla;
        //check the index attr to know which corner it is
        int
                idx =
                        str::toType<int>(
                                         vertices[i]->getAttributes().getValue(
                                                                               "index"));

        if (alt)
            parseLatLonAlt(vertices[i], lla);
        else
            parseLatLon(vertices[i], lla);

        value[idx] = lla;
    }
}

void XMLControl::parseLatLon(xml::lite::Element* parent, LatLon& ll)
{
    double lat, lon;

    parseDouble(getFirstAndOnly(parent, "Lat"), lat);
    parseDouble(getFirstAndOnly(parent, "Lon"), lon);

    ll.setLat(lat);
    ll.setLon(lon);
}

void XMLControl::parseLatLons(xml::lite::Element* pointsXML,
                              std::string pointName, std::vector<LatLon>& llVec)
{
    std::vector<xml::lite::Element*> latLonsXML;
    pointsXML->getElementsByTagName(pointName, latLonsXML);

    for (std::vector<xml::lite::Element*>::iterator it = latLonsXML.begin(); it
            != latLonsXML.end(); ++it)
    {
        LatLon ll;
        parseLatLon(*it, ll);
        llVec.push_back(ll);
    }
}

void XMLControl::parseRangeAzimuth(xml::lite::Element* parent, RangeAzimuth<
        double>& value)
{
    parseDouble(getFirstAndOnly(parent, "Range"), value.range);
    parseDouble(getFirstAndOnly(parent, "Azimuth"), value.azimuth);
}

void XMLControl::parseDateTime(xml::lite::Element* element, DateTime& value)
{
    value = str::toType<DateTime>(element->getCharacterData());
}

void XMLControl::parseRowColDouble(xml::lite::Element* parent,
                                   std::string rowName, std::string colName,
                                   RowColDouble& rc)
{
    parseDouble(getFirstAndOnly(parent, rowName), rc.row);
    parseDouble(getFirstAndOnly(parent, colName), rc.col);
}

void XMLControl::parseRowColLatLon(xml::lite::Element* parent, RowColLatLon& rc)
{
    parseLatLon(getFirstAndOnly(parent, "Row"), rc.row);
    parseLatLon(getFirstAndOnly(parent, "Col"), rc.col);
}

void XMLControl::parseRowColDouble(xml::lite::Element* parent, RowColDouble& rc)
{
    parseRowColDouble(parent, "Row", "Col", rc);
}

void XMLControl::parseRowColInt(xml::lite::Element* parent,
                                std::string rowName, std::string colName,
                                RowColInt& rc)
{
    parseInt(getFirstAndOnly(parent, rowName), rc.row);
    parseInt(getFirstAndOnly(parent, colName), rc.col);
}

void XMLControl::parseRowColInt(xml::lite::Element* parent, RowColInt& rc)
{
    parseRowColInt(parent, "Row", "Col", rc);
}

xml::lite::Element* XMLControl::createFootprint(
                                                std::string name,
                                                std::string cornerName,
                                                const std::vector<LatLon>& corners,
                                                bool alt,
                                                xml::lite::Element* parent)
{
    xml::lite::Element* footprint = newElement(name, parent);
    xml::lite::AttributeNode node;
    node.setQName("size");
    node.setValue("4");

    footprint->getAttributes().add(node);

    xml::lite::Element* vertex;
    node.setQName("index");

    vertex = newElement(cornerName, footprint);
    node.setValue("0");
    vertex->getAttributes().add(node);
    createDouble("Lat", corners[0].getLat(), vertex);
    createDouble("Lon", corners[0].getLon(), vertex);
    if (alt)
    {
        createDouble("HAE", corners[0].getAlt(), vertex);
    }

    vertex = newElement(cornerName, footprint);
    node.setValue("1");
    vertex->getAttributes().add(node);
    createDouble("Lat", corners[1].getLat(), vertex);
    createDouble("Lon", corners[1].getLon(), vertex);
    if (alt)
    {
        createDouble("HAE", corners[1].getAlt(), vertex);
    }

    vertex = newElement(cornerName, footprint);
    node.setValue("2");
    vertex->getAttributes().add(node);
    createDouble("Lat", corners[2].getLat(), vertex);
    createDouble("Lon", corners[2].getLon(), vertex);
    if (alt)
    {
        createDouble("HAE", corners[2].getAlt(), vertex);
    }

    vertex = newElement(cornerName, footprint);
    node.setValue("3");
    vertex->getAttributes().add(node);
    createDouble("Lat", corners[3].getLat(), vertex);
    createDouble("Lon", corners[3].getLon(), vertex);
    if (alt)
    {
        createDouble("HAE", corners[3].getAlt(), vertex);
    }
    return footprint;
}

xml::lite::Element* XMLControl::errorStatisticsToXML(
                                                     ErrorStatistics* errorStatistics,
                                                     xml::lite::Element* parent)
{
    xml::lite::Element* errorStatsXML = newElement("ErrorStatistics", parent);

    //TODO compositeSCP needs to be reworked

    if (errorStatistics->compositeSCP)
    {
        xml::lite::Element* scpXML = newElement("CompositeSCP", errorStatsXML);

        if (errorStatistics->scpType == ErrorStatistics::SCP_RG_AZ)
        {
            xml::lite::Element* rgAzXML = newElement("RgAzErr", scpXML);
            createDouble("Rg", errorStatistics->compositeSCP->xErr, rgAzXML);
            createDouble("Az", errorStatistics->compositeSCP->yErr, rgAzXML);
            createDouble("RgAz", errorStatistics->compositeSCP->xyErr, rgAzXML);
        }
        else
        {
            xml::lite::Element* rgAzXML = newElement("RowColErr", scpXML);
            createDouble("Row", errorStatistics->compositeSCP->xErr, rgAzXML);
            createDouble("Col", errorStatistics->compositeSCP->yErr, rgAzXML);
            createDouble("RgCol", errorStatistics->compositeSCP->xyErr, rgAzXML);
        }
    }

    Components* components = errorStatistics->components;
    if (components)
    {
        xml::lite::Element* componentsXML = newElement("Components",
                                                       errorStatsXML);

        PosVelError* posVelError = components->posVelError;
        RadarSensor* radarSensor = components->radarSensor;
        TropoError* tropoError = components->tropoError;
        IonoError* ionoError = components->ionoError;

        if (posVelError)
        {
            xml::lite::Element* posVelErrXML = newElement("PosVelErr",
                                                          componentsXML);

            createString("Frame", str::toString(posVelError->frame),
                         posVelErrXML);
            createDouble("P1", posVelError->p1, posVelErrXML);
            createDouble("P2", posVelError->p2, posVelErrXML);
            createDouble("P3", posVelError->p3, posVelErrXML);
            createDouble("V1", posVelError->v1, posVelErrXML);
            createDouble("V2", posVelError->v2, posVelErrXML);
            createDouble("V3", posVelError->v3, posVelErrXML);

            CorrCoefs *coefs = posVelError->corrCoefs;
            if (coefs)
            {
                xml::lite::Element* coefsXML = newElement("CorrCoefs",
                                                          posVelErrXML);

                createDouble("P1P2", coefs->p1p2, coefsXML);
                createDouble("P1P3", coefs->p1p3, coefsXML);
                createDouble("P1V1", coefs->p1v1, coefsXML);
                createDouble("P1V2", coefs->p1v2, coefsXML);
                createDouble("P1V3", coefs->p1v3, coefsXML);
                createDouble("P2P3", coefs->p2p3, coefsXML);
                createDouble("P2V1", coefs->p2v1, coefsXML);
                createDouble("P2V2", coefs->p2v2, coefsXML);
                createDouble("P2V3", coefs->p2v3, coefsXML);
                createDouble("P3V1", coefs->p3v1, coefsXML);
                createDouble("P3V2", coefs->p3v2, coefsXML);
                createDouble("P3V3", coefs->p3v3, coefsXML);
                createDouble("V1V2", coefs->v1v2, coefsXML);
                createDouble("V1V3", coefs->v1v3, coefsXML);
                createDouble("V2V3", coefs->v2v3, coefsXML);
            }

            addDecorrType("PositionDecorr", posVelError->positionDecorr,
                          posVelErrXML);
        }
        if (radarSensor)
        {
            xml::lite::Element* radarSensorXML = newElement("RadarSensor",
                                                            componentsXML);

            createDouble("RangeBias", radarSensor->rangeBias, radarSensorXML);

            if (!Init::isUndefined<double>(radarSensor->clockFreqSF))
            {
                createDouble("ClockFreqSF", radarSensor->clockFreqSF,
                             radarSensorXML);
            }
            if (!Init::isUndefined<double>(radarSensor->transmitFreqSF))
            {
                createDouble("TransmitFreqSF", radarSensor->transmitFreqSF,
                             radarSensorXML);
            }
            addDecorrType("RangeBiasDecorr", radarSensor->rangeBiasDecorr,
                          radarSensorXML);
        }
        if (tropoError)
        {
            xml::lite::Element* tropoErrXML = newElement("TropoError",
                                                         componentsXML);

            if (!Init::isUndefined<double>(tropoError->tropoRangeVertical))
            {
                createDouble("TropoRangeVertical",
                             tropoError->tropoRangeVertical, tropoErrXML);
            }
            if (!Init::isUndefined<double>(tropoError->tropoRangeSlant))
            {
                createDouble("TropoRangeSlant", tropoError->tropoRangeSlant,
                             tropoErrXML);
            }

            addDecorrType("TropoRangeDecorr", tropoError->tropoRangeDecorr,
                          tropoErrXML);
        }
        if (ionoError)
        {
            xml::lite::Element* ionoErrXML = newElement("IonoError",
                                                        componentsXML);

            if (!Init::isUndefined<double>(ionoError->ionoRangeVertical))
            {
                createDouble("IonoRangeVertical", ionoError->ionoRangeVertical,
                             ionoErrXML);
            }
            if (!Init::isUndefined<double>(ionoError->ionoRangeRateVertical))
            {
                createDouble("IonoRangeRateVertical",
                             ionoError->ionoRangeRateVertical, ionoErrXML);
            }

            createDouble("IonoRgRgRateCC", ionoError->ionoRgRgRateCC,
                         ionoErrXML);

            addDecorrType("IonoRangeVertDecorr",
                          ionoError->ionoRangeVertDecorr, ionoErrXML);
        }
    }

    if (!errorStatistics->additionalParameters.empty())
    {
        xml::lite::Element* paramsXML = newElement("AdditionalParms",
                                                   errorStatsXML);
        addParameters("Parameter", errorStatistics->additionalParameters,
                      paramsXML);
    }

    return errorStatsXML;
}

void XMLControl::xmlToErrorStatistics(xml::lite::Element* errorStatsXML,
                                      ErrorStatistics* errorStatistics)
{
    xml::lite::Element* tmpElem = NULL;
    //optional
    xml::lite::Element* compositeSCPXML = getOptional(errorStatsXML,
                                                      "CompositeSCP");
    //See if it's RgAzErr or RowColErr
    //optional
    if (compositeSCPXML)
    {
        errorStatistics->compositeSCP = new CompositeSCP();
        tmpElem = getOptional(compositeSCPXML, "RgAzErr");
        if (tmpElem)
        {
            //errorStatistics->initialize(ErrorStatistics::SCP_RG_AZ);
            errorStatistics->scpType = ErrorStatistics::SCP_RG_AZ;
        }
        else
        {
            tmpElem = getOptional(compositeSCPXML, "RowColErr");
            if (tmpElem)
            {
                //errorStatistics->initialize(ErrorStatistics::SCP_ROW_COL);
                errorStatistics->scpType = ErrorStatistics::SCP_ROW_COL;
            }
        }

        if (tmpElem != NULL && errorStatistics->scpType
                == ErrorStatistics::SCP_RG_AZ)
        {
            parseDouble(getFirstAndOnly(tmpElem, "Rg"),
                        errorStatistics->compositeSCP->xErr);
            parseDouble(getFirstAndOnly(tmpElem, "Az"),
                        errorStatistics->compositeSCP->yErr);
            parseDouble(getFirstAndOnly(tmpElem, "RgAz"),
                        errorStatistics->compositeSCP->xyErr);
        }
        else if (tmpElem != NULL && errorStatistics->scpType
                == ErrorStatistics::SCP_ROW_COL)
        {
            parseDouble(getFirstAndOnly(tmpElem, "Row"),
                        errorStatistics->compositeSCP->xErr);
            parseDouble(getFirstAndOnly(tmpElem, "Col"),
                        errorStatistics->compositeSCP->yErr);
            parseDouble(getFirstAndOnly(tmpElem, "RowCol"),
                        errorStatistics->compositeSCP->xyErr);
        }
    }

    xml::lite::Element* posVelErrXML = NULL;
    xml::lite::Element* radarSensorXML = NULL;
    xml::lite::Element* tropoErrorXML = NULL;
    xml::lite::Element* ionoErrorXML = NULL;

    tmpElem = getOptional(errorStatsXML, "Components");
    if (tmpElem)
    {
        //optional
        errorStatistics->components = new Components();

        posVelErrXML = getOptional(tmpElem, "PosVelErr");
        if (posVelErrXML)
        {
            //optional
            errorStatistics->components->posVelError = new PosVelError();
        }

        radarSensorXML = getOptional(tmpElem, "RadarSensor");
        if (radarSensorXML)
        {
            //optional
            errorStatistics->components->radarSensor = new RadarSensor();
        }

        tropoErrorXML = getOptional(tmpElem, "TropoError");
        if (tropoErrorXML)
        {
            //optional
            errorStatistics->components->tropoError = new TropoError();
        }

        ionoErrorXML = getOptional(tmpElem, "IonoError");
        if (ionoErrorXML)
        {
            //optional
            errorStatistics->components->ionoError = new IonoError();
        }
    }

    if (posVelErrXML != NULL)
    {
        errorStatistics->components->posVelError->frame
                = str::toType<PosVelError::FrameType>(
                                                      getFirstAndOnly(
                                                                      posVelErrXML,
                                                                      "Frame")->getCharacterData());
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
            errorStatistics->components->posVelError->corrCoefs
                    = new CorrCoefs();
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

void XMLControl::xmlToRadiometric(xml::lite::Element* radiometricXML,
                                  Radiometric *radiometric)
{
    xml::lite::Element* tmpElem = NULL;

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
                = str::toType<six::AppliedType>(tmpElem->getCharacterData());
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
                = str::toType<six::AppliedType>(tmpElem->getCharacterData());
    }
}

xml::lite::Element* XMLControl::radiometricToXML(Radiometric *r,
                                                 xml::lite::Element* parent)
{
    xml::lite::Element* rXML = newElement("Radiometric", parent);

    if (r->noisePoly.orderX() >= 0 && r->noisePoly.orderY() >= 0)
        createPoly2D("NoisePoly", r->noisePoly, rXML);
    if (r->rcsSFPoly.orderX() >= 0 && r->rcsSFPoly.orderY() >= 0)
        createPoly2D("RCSSFPoly", r->rcsSFPoly, rXML);
    if (r->betaZeroSFPoly.orderX() >= 0 && r->betaZeroSFPoly.orderY() >= 0)
        createPoly2D("BetaZeroSFPoly", r->betaZeroSFPoly, rXML);
    if (r->sigmaZeroSFPoly.orderX() >= 0 && r->sigmaZeroSFPoly.orderY() >= 0)
        createPoly2D("SigmaZeroSFPoly", r->sigmaZeroSFPoly, rXML);

    if (r->sigmaZeroSFIncidenceMap != six::APPLIED_NOT_SET)
    {
        createString(
                     "SigmaZeroSFIncidenceMap",
                     str::toString<six::AppliedType>(r->sigmaZeroSFIncidenceMap),
                     rXML);
    }
    if (r->gammaZeroSFPoly.orderX() >= 0 && r->gammaZeroSFPoly.orderY() >= 0)
        createPoly2D("GammaZeroSFPoly", r->sigmaZeroSFPoly, rXML);
    if (r->gammaZeroSFIncidenceMap != six::APPLIED_NOT_SET)
    {
        createString(
                     "GammaZeroSFIncidenceMap",
                     str::toString<six::AppliedType>(r->gammaZeroSFIncidenceMap),
                     rXML);
    }
    return rXML;
}
