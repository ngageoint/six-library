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

xml::lite::Element* XMLControl::createVector3D(xml::lite::Document* doc,
        std::string name, Vector3 p)
{
    xml::lite::Element* element = newElement(doc, name);
    element->addChild(createDouble(doc, "X", p[0]));
    element->addChild(createDouble(doc, "Y", p[1]));
    element->addChild(createDouble(doc, "Z", p[2]));
    return element;
}

xml::lite::Element* XMLControl::createPoly1D(xml::lite::Document* doc,
        std::string name, const Poly1D& poly1D)
{
    int order = poly1D.order();
    xml::lite::Element* poly1DXML = newElement(doc, name);
    setAttribute(poly1DXML, "order1", str::toString(order));

    for (int i = 0; i <= order; ++i)
    {
        xml::lite::Element* coefXML = createDouble(doc, "Coef", poly1D[i]);
        setAttribute(coefXML, "exponent1", str::toString(i));
        poly1DXML->addChild(coefXML);
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
        exp = str::toType<int>(xCoeffsXML[i]->getAttributes().getValue(
                "exponent1"));
        parseDouble(xCoeffsXML[i], pXYZ[exp][0]);

        exp = str::toType<int>(yCoeffsXML[i]->getAttributes().getValue(
                "exponent1"));
        parseDouble(yCoeffsXML[i], pXYZ[exp][1]);

        exp = str::toType<int>(zCoeffsXML[i]->getAttributes().getValue(
                "exponent1"));
        parseDouble(zCoeffsXML[i], pXYZ[exp][2]);
    }

    polyXYZ = pXYZ;
}

xml::lite::Element* XMLControl::createPolyXYZ(xml::lite::Document* doc,
        std::string name, const PolyXYZ& polyXYZ)
{
    int order = polyXYZ.order();
    xml::lite::Element* polyXML = newElement(doc, name);

    xml::lite::Element* xXML = newElement(doc, "X");
    xml::lite::Element* yXML = newElement(doc, "Y");
    xml::lite::Element* zXML = newElement(doc, "Z");

    polyXML->addChild(xXML);
    polyXML->addChild(yXML);
    polyXML->addChild(zXML);

    setAttribute(xXML, "order1", str::toString(order));
    setAttribute(yXML, "order1", str::toString(order));
    setAttribute(zXML, "order1", str::toString(order));

    for (int i = 0; i <= order; ++i)
    {
        Vector3 v3 = polyXYZ[i];
        xml::lite::Element* xCoefXML = createDouble(doc, "Coef", v3[0]);
        xml::lite::Element* yCoefXML = createDouble(doc, "Coef", v3[1]);
        xml::lite::Element* zCoefXML = createDouble(doc, "Coef", v3[2]);

        setAttribute(xCoefXML, "exponent1", str::toString(i));
        setAttribute(yCoefXML, "exponent1", str::toString(i));
        setAttribute(zCoefXML, "exponent1", str::toString(i));

        xXML->addChild(xCoefXML);
        yXML->addChild(yCoefXML);
        zXML->addChild(zCoefXML);
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

xml::lite::Element* XMLControl::createPoly2D(xml::lite::Document* doc,
        std::string name, const Poly2D& poly2D)
{
    xml::lite::AttributeNode node;
    xml::lite::Element* poly2DXML = newElement(doc, name);
    setAttribute(poly2DXML, "order1", str::toString(poly2D.orderX()));
    setAttribute(poly2DXML, "order2", str::toString(poly2D.orderY()));

    for (int i = 0; i <= poly2D.orderX(); i++)
    {
        for (int j = 0; j <= poly2D.orderY(); j++)
        {
            if ((i == 0 && j == 0) || 
                std::abs(poly2D[i][j]) > std::numeric_limits<float>::epsilon())
            {
                xml::lite::Element* coefXML = createDouble(doc, "Coef",
                        poly2D[i][j]);
                setAttribute(coefXML, "exponent1", str::toString(i));
                setAttribute(coefXML, "exponent2", str::toString(j));
                poly2DXML->addChild(coefXML);
            }
        }
    }

    return poly2DXML;
}

xml::lite::Element* XMLControl::createString(xml::lite::Document* doc,
        std::string name, std::string p)
{
    xml::lite::Element* e = doc->createElement(name, mURI, p);
    xml::lite::AttributeNode node;
    node.setQName("class");
    node.setUri(mURI);
    node.setValue("xs:string");
    e->getAttributes().add(node);
    return e;
}

xml::lite::Element* XMLControl::createInt(xml::lite::Document* doc,
        std::string name, int p)
{
    xml::lite::Element* e = doc->createElement(name, mURI,
            str::toString<int>(p));
    xml::lite::AttributeNode node;
    node.setQName("class");
    node.setUri(mURI);
    node.setValue("xs:int");
    e->getAttributes().add(node);
    return e;
}

xml::lite::Element* XMLControl::createDouble(xml::lite::Document* doc,
        std::string name, double p)
{
    xml::lite::Element* e = doc->createElement(name, mURI,
            str::toString<double>(p));
    xml::lite::AttributeNode node;
    node.setQName("class");
    node.setUri(mURI);
    node.setValue("xs:double");
    e->getAttributes().add(node);
    return e;
}

xml::lite::Element* XMLControl::createComplex(xml::lite::Document* doc,
        std::string name, std::complex<double> c)
{
    xml::lite::Element* e = newElement(doc, name);
    e->addChild(createDouble(doc, "Real", c.real()));
    e->addChild(createDouble(doc, "Imag", c.imag()));
    return e;
}

xml::lite::Element* XMLControl::createBooleanType(xml::lite::Document* doc,
        std::string name, BooleanType p)
{
    if (p == six::BOOL_NOT_SET)
        return NULL;

    xml::lite::Element* e = doc->createElement(name, mURI,
            str::toString<BooleanType>(p));
    xml::lite::AttributeNode node;
    node.setQName("class");
    node.setUri(mURI);
    node.setValue("xs:boolean");
    e->getAttributes().add(node);
    return e;
}

xml::lite::Element* XMLControl::createDateTime(xml::lite::Document* doc,
        std::string name, std::string s)
{
    xml::lite::Element* e = doc->createElement(name, mURI, s);
    xml::lite::AttributeNode node;
    node.setQName("class");
    node.setUri(mURI);
    node.setValue("xs:dateTime");
    e->getAttributes().add(node);
    return e;
}

xml::lite::Element* XMLControl::createDateTime(xml::lite::Document* doc,
        std::string name, DateTime p)
{
    std::string s = str::toString<DateTime>(p);
    return createDateTime(doc, name, s);
}

xml::lite::Element* XMLControl::createDate(xml::lite::Document* doc,
        std::string name, DateTime p)
{
    char date[256];
    date[255] = 0;
    p.format("%Y-%m-%d", date, 255);
    std::string s(date);

    xml::lite::Element* e = doc->createElement(name, mURI, s);
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

void XMLControl::addOptional(xml::lite::Element* parent,
        xml::lite::Element* element)
{
    if (element)
        parent->addChild(element);
}

void XMLControl::addRequired(xml::lite::Element* parent,
        xml::lite::Element* element, std::string name)
{
    if (!element)
        throw except::Exception(Ctxt(FmtX("Required field [%s] is undefined "
                "or null.", name.c_str())));

    parent->addChild(element);
}

xml::lite::Element* XMLControl::createRowCol(xml::lite::Document* doc,
        std::string name, std::string rowName, std::string colName,
        const RowColInt& value)
{
    xml::lite::Element* e = newElement(doc, name);
    e->addChild(createInt(doc, rowName, value.row));
    e->addChild(createInt(doc, colName, value.col));
    return e;
}

xml::lite::Element* XMLControl::createRowCol(xml::lite::Document* doc,
        std::string name, std::string rowName, std::string colName,
        const RowColDouble& value)
{
    xml::lite::Element* e = newElement(doc, name);
    e->addChild(createDouble(doc, rowName, value.row));
    e->addChild(createDouble(doc, colName, value.col));
    return e;
}

xml::lite::Element* XMLControl::createRowCol(xml::lite::Document* doc,
        std::string name, const RowColInt& value)
{
    return createRowCol(doc, name, "Row", "Col", value);
}

xml::lite::Element* XMLControl::createRowCol(xml::lite::Document* doc,
        std::string name, const RowColDouble& value)
{
    return createRowCol(doc, name, "Row", "Col", value);
}

xml::lite::Element* XMLControl::createRangeAzimuth(xml::lite::Document* doc,
        std::string name, const RangeAzimuth<double>& value)
{
    xml::lite::Element* e = newElement(doc, name);
    e->addChild(createDouble(doc, "Range", value.range));
    e->addChild(createDouble(doc, "Azimuth", value.azimuth));
    return e;
}

xml::lite::Element* XMLControl::createLatLon(xml::lite::Document* doc,
        std::string name, const LatLon& value)
{
    xml::lite::Element* e = newElement(doc, name);
    e->addChild(createDouble(doc, "Lat", value.getLat()));
    e->addChild(createDouble(doc, "Lon", value.getLon()));
    return e;
}

xml::lite::Element* XMLControl::createLatLonAlt(xml::lite::Document* doc,
        std::string name, const LatLonAlt& value)
{
    xml::lite::Element* e = createLatLon(doc, name, value);
    e->addChild(createDouble(doc, "HAE", value.getAlt()));
    return e;
}

xml::lite::Element* XMLControl::createEarthModelType(xml::lite::Document* doc,
        std::string name, const EarthModelType& value)
{
    return createString(doc, name, str::toString(value));
}

xml::lite::Element* XMLControl::createSideOfTrackType(xml::lite::Document* doc,
        std::string name, const SideOfTrackType& value)
{
    return createString(doc, name, str::toString(value));
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
    value = str::toType<int>(element->getCharacterData());
}

void XMLControl::parseInt(xml::lite::Element* element, long& value)
{
    value = str::toType<long>(element->getCharacterData());
}

void XMLControl::parseUInt(xml::lite::Element* element, unsigned int& value)
{
    value = str::toType<unsigned int>(element->getCharacterData());
}

void XMLControl::parseUInt(xml::lite::Element* element, unsigned long& value)
{
    value = str::toType<unsigned long>(element->getCharacterData());
}

void XMLControl::parseDouble(xml::lite::Element* element, double& value)
{
    value = str::toType<double>(element->getCharacterData());
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
    value = str::toType<BooleanType>(element->getCharacterData());
}

Parameter XMLControl::parseParameter(xml::lite::Element* element)
{
    Parameter p(element->getCharacterData());
    p.setName(element->getAttributes().getValue("name"));
    return p;
}

void XMLControl::parseParameters(xml::lite::Element* paramXML,
        std::string paramName, std::vector<Parameter>& props)
{
    std::vector<xml::lite::Element*> elemXML;
    paramXML->getElementsByTagName(paramName, elemXML);

    for (std::vector<xml::lite::Element*>::iterator it = elemXML.begin(); it
            != elemXML.end(); ++it)
    {
        props.push_back(parseParameter(*it));
    }
}

xml::lite::Element* XMLControl::createParameter(xml::lite::Document* doc,
        xml::lite::Element* parent, std::string name, const Parameter& value)
{
    xml::lite::Element *element = createString(doc, name, value.str());
    setAttribute(element, "name", value.getName());
    return element;
}

void XMLControl::addParameters(xml::lite::Document* doc,
        xml::lite::Element* parent, std::string name,
        std::vector<Parameter>& props)
{
    for (std::vector<Parameter>::iterator it = props.begin();
            it != props.end(); ++it)
    {
        parent->addChild(createParameter(doc, parent, name, *it));
    }
}

void XMLControl::addDecorrType(xml::lite::Document* doc,
        xml::lite::Element* parent, std::string name, DecorrType& decorrType)
{
    //only adds it if it needs to
    if (!Init::isUndefined<double>(decorrType.corrCoefZero) && !Init::isUndefined<double>(
            decorrType.decorrRate))
    {
        xml::lite::Element* decorrXML = newElement(doc, name);
        parent->addChild(decorrXML);
        decorrXML->addChild(createDouble(doc, "CorrCoefZero",
                decorrType.corrCoefZero));
        decorrXML->addChild(createDouble(doc, "DecorrRate",
                decorrType.decorrRate));
    }
}

void XMLControl::parseDecorrType(xml::lite::Element* decorrXML,
        DecorrType& decorrType)
{
    parseDouble(getFirstAndOnly(decorrXML, "CorrCoefZero"),
            decorrType.corrCoefZero);
    parseDouble(getFirstAndOnly(decorrXML, "DecorrRate"),
            decorrType.decorrRate);
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
        std::string cornerName, std::vector<LatLon>& value, bool alt)
{
    std::vector<xml::lite::Element*> vertices;
    footprint->getElementsByTagName(cornerName, vertices);

    value.clear();
    value.resize(4);

    for (unsigned int i = 0; i < vertices.size(); i++)
    {
        LatLonAlt lla;
        //check the index attr to know which corner it is
        int idx = str::toType<int>(vertices[i]->getAttributes().getValue(
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

void XMLControl::parseRangeAzimuth(xml::lite::Element* parent,
        RangeAzimuth<double>& value)
{
    parseDouble(getFirstAndOnly(parent, "Range"), value.range);
    parseDouble(getFirstAndOnly(parent, "Azimuth"), value.azimuth);
}

void XMLControl::parseDateTime(xml::lite::Element* element, DateTime& value)
{
    value = str::toType<DateTime>(element->getCharacterData());
}

void XMLControl::parseRowColDouble(xml::lite::Element* parent,
        std::string rowName, std::string colName, RowColDouble& rc)
{
    parseDouble(getFirstAndOnly(parent, rowName), rc.row);
    parseDouble(getFirstAndOnly(parent, colName), rc.col);
}

void XMLControl::parseRowColDouble(xml::lite::Element* parent,
        RowColDouble& rc)
{
    parseRowColDouble(parent, "Row", "Col", rc);
}

void XMLControl::parseRowColInt(xml::lite::Element* parent,
        std::string rowName, std::string colName, RowColInt& rc)
{
    parseInt(getFirstAndOnly(parent, rowName), rc.row);
    parseInt(getFirstAndOnly(parent, colName), rc.col);
}

void XMLControl::parseRowColInt(xml::lite::Element* parent, RowColInt& rc)
{
    parseRowColInt(parent, "Row", "Col", rc);
}

xml::lite::Element* XMLControl::createFootprint(xml::lite::Document* doc,
                                                std::string name, std::string cornerName, const std::vector<LatLon>& corners, bool alt)
{
    xml::lite::Element* footprint = newElement(doc, name);
    xml::lite::AttributeNode node;
    node.setQName("size");
    node.setValue("4");

    footprint->getAttributes().add(node);

    xml::lite::Element* vertex;
    node.setQName("index");

    vertex = newElement(doc, cornerName);
    node.setValue("0");
    vertex->getAttributes().add(node);
    vertex->addChild(createDouble(doc, "Lat", corners[0].getLat()));
    vertex->addChild(createDouble(doc, "Lon", corners[0].getLon()));
    if (alt)
    {
        vertex->addChild(createDouble(doc, "HAE", corners[0].getAlt()));
    }
    footprint->addChild(vertex);

    vertex = newElement(doc, cornerName);
    node.setValue("1");
    vertex->getAttributes().add(node);
    vertex->addChild(createDouble(doc, "Lat", corners[1].getLat()));
    vertex->addChild(createDouble(doc, "Lon", corners[1].getLon()));
    if (alt)
    {
        vertex->addChild(createDouble(doc, "HAE", corners[1].getAlt()));
    }
    footprint->addChild(vertex);

    vertex = newElement(doc, cornerName);
    node.setValue("2");
    vertex->getAttributes().add(node);
    vertex->addChild(createDouble(doc, "Lat", corners[2].getLat()));
    vertex->addChild(createDouble(doc, "Lon", corners[2].getLon()));
    if (alt)
    {
        vertex->addChild(createDouble(doc, "HAE", corners[2].getAlt()));
    }
    footprint->addChild(vertex);

    vertex = newElement(doc, cornerName);
    node.setValue("3");
    vertex->getAttributes().add(node);
    vertex->addChild(createDouble(doc, "Lat", corners[3].getLat()));
    vertex->addChild(createDouble(doc, "Lon", corners[3].getLon()));
    if (alt)
    {
        vertex->addChild(createDouble(doc, "HAE", corners[3].getAlt()));
    }
    footprint->addChild(vertex);
    return footprint;
}

xml::lite::Element* XMLControl::errorStatisticsToXML(xml::lite::Document* doc,
        ErrorStatistics* errorStatistics)
{
    xml::lite::Element* errorStatsXML = newElement(doc, "ErrorStatistics");

    //TODO compositeSCP needs to be reworked

    if (errorStatistics->compositeSCP)
    {
        xml::lite::Element* scpXML = newElement(doc, "CompositeSCP");
        errorStatsXML->addChild(scpXML);

        if (errorStatistics->scpType == ErrorStatistics::SCP_RG_AZ)
        {
            xml::lite::Element* rgAzXML = newElement(doc, "RgAzErr");
            scpXML->addChild(rgAzXML);
            rgAzXML->addChild(createDouble(doc, "Rg",
                    errorStatistics->compositeSCP->xErr));
            rgAzXML->addChild(createDouble(doc, "Az",
                    errorStatistics->compositeSCP->yErr));
            rgAzXML->addChild(createDouble(doc, "RgAz",
                    errorStatistics->compositeSCP->xyErr));
        }
        else
        {
            xml::lite::Element* rgAzXML = newElement(doc, "RowColErr");
            scpXML->addChild(rgAzXML);
            rgAzXML->addChild(createDouble(doc, "Row",
                    errorStatistics->compositeSCP->xErr));
            rgAzXML->addChild(createDouble(doc, "Col",
                    errorStatistics->compositeSCP->yErr));
            rgAzXML->addChild(createDouble(doc, "RgCol",
                    errorStatistics->compositeSCP->xyErr));
        }
    }

    Components* components = errorStatistics->components;
    if (components)
    {
        xml::lite::Element* componentsXML = newElement(doc, "Components");
        errorStatsXML->addChild(componentsXML);

        PosVelError* posVelError = components->posVelError;
        RadarSensor* radarSensor = components->radarSensor;
        TropoError* tropoError = components->tropoError;
        IonoError* ionoError = components->ionoError;

        if (posVelError)
        {
            xml::lite::Element* posVelErrXML = newElement(doc, "PosVelErr");
            componentsXML->addChild(posVelErrXML);

            posVelErrXML->addChild(createString(doc, "Frame", str::toString(
                    posVelError->frame)));
            posVelErrXML->addChild(createDouble(doc, "P1", posVelError->p1));
            posVelErrXML->addChild(createDouble(doc, "P2", posVelError->p2));
            posVelErrXML->addChild(createDouble(doc, "P3", posVelError->p3));
            posVelErrXML->addChild(createDouble(doc, "V1", posVelError->v1));
            posVelErrXML->addChild(createDouble(doc, "V2", posVelError->v2));
            posVelErrXML->addChild(createDouble(doc, "V3", posVelError->v3));

            CorrCoefs *coefs = posVelError->corrCoefs;
            if (coefs)
            {
                xml::lite::Element* coefsXML = newElement(doc, "CorrCoefs");
                posVelErrXML->addChild(coefsXML);

                coefsXML->addChild(createDouble(doc, "P1P2", coefs->p1p2));
                coefsXML->addChild(createDouble(doc, "P1P3", coefs->p1p3));
                coefsXML->addChild(createDouble(doc, "P1V1", coefs->p1v1));
                coefsXML->addChild(createDouble(doc, "P1V2", coefs->p1v2));
                coefsXML->addChild(createDouble(doc, "P1V3", coefs->p1v3));
                coefsXML->addChild(createDouble(doc, "P2P3", coefs->p2p3));
                coefsXML->addChild(createDouble(doc, "P2V1", coefs->p2v1));
                coefsXML->addChild(createDouble(doc, "P2V2", coefs->p2v2));
                coefsXML->addChild(createDouble(doc, "P2V3", coefs->p2v3));
                coefsXML->addChild(createDouble(doc, "P3V1", coefs->p3v1));
                coefsXML->addChild(createDouble(doc, "P3V2", coefs->p3v2));
                coefsXML->addChild(createDouble(doc, "P3V3", coefs->p3v3));
                coefsXML->addChild(createDouble(doc, "V1V2", coefs->v1v2));
                coefsXML->addChild(createDouble(doc, "V1V3", coefs->v1v3));
                coefsXML->addChild(createDouble(doc, "V2V3", coefs->v2v3));
            }

            addDecorrType(doc, posVelErrXML, "PositionDecorr",
                    posVelError->positionDecorr);
        }
        if (radarSensor)
        {
            xml::lite::Element* radarSensorXML = newElement(doc, "RadarSensor");
            componentsXML->addChild(radarSensorXML);

            radarSensorXML->addChild(createDouble(doc, "RangeBias",
                    radarSensor->rangeBias));

            if (!Init::isUndefined<double>(radarSensor->clockFreqSF))
            {
                radarSensorXML->addChild(createDouble(doc, "ClockFreqSF",
                        radarSensor->clockFreqSF));
            }
            if (!Init::isUndefined<double>(radarSensor->transmitFreqSF))
            {
                radarSensorXML->addChild(createDouble(doc, "TransmitFreqSF",
                        radarSensor->transmitFreqSF));
            }
            addDecorrType(doc, radarSensorXML, "RangeBiasDecorr",
                    radarSensor->rangeBiasDecorr);
        }
        if (tropoError)
        {
            xml::lite::Element* tropoErrXML = newElement(doc, "TropoError");
            componentsXML->addChild(tropoErrXML);

            if (!Init::isUndefined<double>(tropoError->tropoRangeVertical))
            {
                tropoErrXML->addChild(createDouble(doc, "TropoRangeVertical",
                        tropoError->tropoRangeVertical));
            }
            if (!Init::isUndefined<double>(tropoError->tropoRangeSlant))
            {
                tropoErrXML->addChild(createDouble(doc, "TropoRangeSlant",
                        tropoError->tropoRangeSlant));
            }

            addDecorrType(doc, tropoErrXML, "TropoRangeDecorr",
                    tropoError->tropoRangeDecorr);
        }
        if (ionoError)
        {
            xml::lite::Element* ionoErrXML = newElement(doc, "IonoError");
            componentsXML->addChild(ionoErrXML);

            if (!Init::isUndefined<double>(ionoError->ionoRangeVertical))
            {
                ionoErrXML->addChild(createDouble(doc, "IonoRangeVertical",
                        ionoError->ionoRangeVertical));
            }
            if (!Init::isUndefined<double>(ionoError->ionoRangeRateVertical))
            {
                ionoErrXML->addChild(createDouble(doc, "IonoRangeRateVertical",
                        ionoError->ionoRangeRateVertical));
            }

            ionoErrXML->addChild(createDouble(doc, "IonoRgRgRateCC",
                    ionoError->ionoRgRgRateCC));

            addDecorrType(doc, ionoErrXML, "IonoRangeVertDecorr",
                    ionoError->ionoRangeVertDecorr);
        }
    }

    if (!errorStatistics->additionalParameters.empty())
    {
        xml::lite::Element* paramsXML = newElement(doc, "AdditionalParms");
        errorStatsXML->addChild(paramsXML);
        addParameters(doc, paramsXML, "Parameter",
                errorStatistics->additionalParameters);
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
    if(tmpElem)
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
        errorStatistics->components->posVelError->frame = str::toType<
                PosVelError::FrameType>(
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
            errorStatistics->components->posVelError->corrCoefs
                    = new CorrCoefs();
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
            parseDouble(tmpElem, errorStatistics->components->tropoError
                    ->tropoRangeVertical);
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
            parseDouble(tmpElem, errorStatistics->components->ionoError
                    ->ionoRangeRateVertical);
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
        radiometric->sigmaZeroSFIncidenceMap = str::toType<six::AppliedType>(
                tmpElem->getCharacterData());
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
        radiometric->gammaZeroSFIncidenceMap = str::toType<six::AppliedType>(
                tmpElem->getCharacterData());
    }
}

xml::lite::Element* XMLControl::radiometricToXML(xml::lite::Document* doc,
                                                 Radiometric *radiometric)
{
    xml::lite::Element* radiometricXML = newElement(doc, "Radiometric");

    if (radiometric->noisePoly.orderX() >= 0 && radiometric->noisePoly.orderY()
            >= 0)
        radiometricXML->addChild(createPoly2D(doc, "NoisePoly",
                radiometric->noisePoly));
    if (radiometric->rcsSFPoly.orderX() >= 0 && radiometric->rcsSFPoly.orderY()
            >= 0)
        radiometricXML->addChild(createPoly2D(doc, "RCSSFPoly",
                radiometric->rcsSFPoly));
    if (radiometric->betaZeroSFPoly.orderX() >= 0
            && radiometric->betaZeroSFPoly.orderY() >= 0)
        radiometricXML->addChild(createPoly2D(doc, "BetaZeroSFPoly",
                radiometric->betaZeroSFPoly));
    if (radiometric->sigmaZeroSFPoly.orderX() >= 0
            && radiometric->sigmaZeroSFPoly.orderY() >= 0)
        radiometricXML->addChild(createPoly2D(doc, "SigmaZeroSFPoly",
                radiometric->sigmaZeroSFPoly));

    if (radiometric->sigmaZeroSFIncidenceMap != six::APPLIED_NOT_SET)
    {
        radiometricXML->addChild(createString(doc, "SigmaZeroSFIncidenceMap",
                str::toString<six::AppliedType>(
                        radiometric->sigmaZeroSFIncidenceMap)));
    }
    if (radiometric->gammaZeroSFPoly.orderX() >= 0
            && radiometric->gammaZeroSFPoly.orderY() >= 0)
        radiometricXML->addChild(createPoly2D(doc, "GammaZeroSFPoly",
                radiometric->sigmaZeroSFPoly));
    if (radiometric->gammaZeroSFIncidenceMap != six::APPLIED_NOT_SET)
    {
        radiometricXML->addChild(createString(doc, "GammaZeroSFIncidenceMap",
                str::toString<six::AppliedType>(
                        radiometric->gammaZeroSFIncidenceMap)));
    }
    return radiometricXML;
}
